//
//  BaseClassifier.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/26/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#ifdef __APPLE__
    #include <OpenCL/OpenCL.h>
#else
    #include <stdio.h>
    #include <string.h>
    #include "CL/cl.h"
    #define FALSE false
    #define TRUE true
#endif


#include "BaseClassifier.h"



// this is a tool from NVIDIA
//////////////////////////////////////////////////////////////////////////////
//! Loads a Program file and prepends the cPreamble to the code.
//!
//! @return the source string if succeeded, 0 otherwise
//! @param cFilename        program filename
//! @param cPreamble        code that is prepended to the loaded file, typically a set of #defines or a header
//! @param szFinalLength    returned length of the code string
//////////////////////////////////////////////////////////////////////////////
char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
{
    // locals 
    FILE* pFileStream = NULL;
    size_t szSourceLength;
    

    // open the OpenCL source code file
    #ifdef _WIN32   // Windows version
        if(fopen_s(&pFileStream, cFilename, "rb") != 0) 
        {                   
            return NULL;    
        }   
    #else           // Linux version

        pFileStream = fopen(cFilename, "rb");
        if(pFileStream == 0) 
        {       
            return NULL;
        }
    #endif
    
    size_t szPreambleLength = strlen(cPreamble);
    
    // get the length of the source code
    fseek(pFileStream, 0, SEEK_END); 
    szSourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET); 
    
    // allocate a buffer for the source code string and read it in
    char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1); 
    memcpy(cSourceString, cPreamble, szPreambleLength);
    if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
    {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }
    
    // close the file and return the total length of the combined (preamble + source) string
    fclose(pFileStream);
    if(szFinalLength != 0)
    {
        *szFinalLength = szSourceLength + szPreambleLength;
    }
    cSourceString[szSourceLength + szPreambleLength] = '\0';
    
    return cSourceString;
}



// this is really just a dummy method
// will get overloaded with Classify (combine into map)
// and Eval (print into output rootfile) methods
int BaseClassifier::ProcessOutput(int* output_data, long nevents){
    
    for (int i=0; i<nevents; i++) {
        for (int j=0; j<ndim; j++) {
            for (int k=0; k<mdim; k++) {
                if ((i+j+k)%10==0) std::cout<<" here is the output"<<k<<" "<<i<<" "<<j<<", I think "<<output_data[i*mdim*ndim+j*mdim+k]<<std::endl;
            }
        }
    }
    
    std::cout<<" done! "<<std::endl;
    
    return 0;
    
}

// this is really just the dummy method
// will get overloaded with Classify and Eval methods
int BaseClassifier::InputData(long nevents, float* data_in){

    // crazily simple
    for (int i=0; i<nevents; i++) {
        
        for (int j=0; j<ndim; j++) {
            
            data_in[i*ndim+j]=(cl_float)((float)(1+i)/(float)(nevents+ndim+1));
            std::cout<<" here is the input "<<j<<" "<<i<<" value "<<data_in[i*ndim+j]<<std::endl;
            
        }
    }
    
    return 0;
    
}

int BaseClassifier::ProcessSet(cl_device_id device, char* device_name, cl_kernel kernel, long ne, cl_event gpudone_event, float* data_in, int* data_out, size_t  mem_size_output_data, size_t  mem_size_input_data){
    

    
    
    // I don't know if I need to do something else to define this
    //cl_command_queue commandQueue = 0;
    // I set this somewhere else
    
    // I don't know if I should be using CL_MEM_USE_HOST_PTR
    
    
    // this will be done in general
    //cl_mem input_data = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
    //                                   mem_size_input_data, data_in, &ciErrNum);
    
    cl_mem input_data = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, mem_size_input_data, NULL, &ciErrNum);
    
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout<<" problem creating buffer "<<std::endl;
        //shrLog("Error: clCreateBuffer\n");
        return ciErrNum;
    }
    
    
    cl_mem output_data = 
        clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY,  mem_size_output_data, NULL, &ciErrNum);

    if (ciErrNum != CL_SUCCESS)
    {
        std::cout<<" problem creating output buffer "<<std::endl;
        //shrLog("Error: clCreateBuffer\n");
        return ciErrNum;
    }
    
    ciErrNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &input_data);
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout<<" problem setting argument 0 "<<std::endl;
        //shrLog("Error: clCreateBuffer\n");
        return ciErrNum;
    }
    ciErrNum = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *) &output_data);
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout<<" problem setting argument 3 "<<std::endl;
        //shrLog("Error: clCreateBuffer\n");
        return ciErrNum;
    }
    
    // I need to think of how I deal with the min/max arrays, maybe I set this as global
    // but the special template?
    
    
    //input_size=mem_size_input_data/ciDeviceCount;
    size_t input_size=mem_size_input_data;
    
    //input_offset=0+i*work_size;
    size_t input_offset=0;
    
    // I am not sure about the input size, it should be work size * float * size of job?
    //cl_mem dest_data = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY,
    //                                       mem_size_input_data, NULL, NULL);
    
    /*
    ciErrNum = clEnqueueCopyBuffer(commandQueue, 
                                   input_data, // src_buffer
                                   dest_data, // dst_buffer
                                   input_offset, // src_offset
                                   0, // dst_offset
                                   input_size,  // size_of_bytes to copy
                                   0,  // number_events_in_waitlist
                                   NULL, /// event_wait_list
                                   NULL); // event
     */
    
    ciErrNum = clEnqueueWriteBuffer(commandQueue, input_data,  // que and clmem
                                    CL_TRUE, input_offset, // blocking? offset
                                    input_size, data_in, // size? and input mem
                                    0, NULL, NULL); // event wait list, events in wait list, event
    
	if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" Error: Failure to copy buffer "<<std::endl;
		//shrLog("clEnqueueCopyBuffer() Error: Failed to copy buffer!\n");
		return -1;
	}
    
    
    //cl_event GPUExecution;
    
    
    // CL_DEVICE_MAX_COMPUTE_UNITS for wgs?
    // CL_DEVICE_MAX_WORK_ITEM_SIZES
    // CL_DEVICE_MAX_WORK_GROUP_SIZE
    
    size_t workitem_size[3];
    ciErrNum = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
    if (ciErrNum != CL_SUCCESS)
    {
        // there is a problem
        std::cout<<" problem with work item size "<<ciErrNum<<std::endl;
    }
    
    size_t maxworkitem_size;
    ciErrNum = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxworkitem_size), &maxworkitem_size, NULL);
        if (ciErrNum != CL_SUCCESS)
    {
        // there is a problem
        std::cout<<" problem max size "<<ciErrNum<<std::endl;
    }
    printf("what is max %lu",maxworkitem_size);
    
    size_t testworkgroup;
    ciErrNum = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(testworkgroup), &testworkgroup, NULL);
    if (ciErrNum != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", ciErrNum);
        return ciErrNum;
        
    }
    printf(" what is workgroup test %lu",testworkgroup);
    
    
    
    
    // wgs is the number of threads or something of the GPU
    // I think that the dim1/dim2/dim3 are the sizes due to mdim/ndim
    
    // mdim (has units of int/float)
    size_t dim3_size=mdim; //mem_size_output_data/mem_size_input_data; 
    // ndim (has units of float)
    size_t dim2_size=ndim; //mem_size_input_data/ne;
    // number of events
    size_t dim1_size=ne;
    
    size_t globalWorkSize[] = {dim1_size, dim2_size, dim3_size};

    
    // now we try to get a more intelligent workgroup size
    size_t localws=maxworkitem_size/dim2_size/dim3_size;
    printf("our starting value %lu",localws);
    
    // it would be better if I have some sort of recursive function for this
    // right now it tries first two times and then gives up (=1)
    if (dim1_size%localws==0){
        
        // this is correct
        localws=localws;
        
    } else {
        
        
        size_t remws = dim1_size%localws;
        printf("got it in second go %lu",remws);
        if (localws%remws==0) {

            
        localws=remws;
        } else {
            printf("got it in third go %lu ?",localws%remws);
            
            localws=1;
        }
    }
    
    size_t localWorkSize[] = {localws,dim2_size,dim3_size};
    //size_t localWorkSize[] = {dim1_size/workitem_size[0], dim2_size/workitem_size[1], 
    //        dim3_size/workitem_size[2]};
    
    printf("this is the worksize %lu %lu %lu",workitem_size[0],workitem_size[1],workitem_size[2]);
    printf("this is the local worksize %lu %lu %lu",
           localWorkSize[0],localWorkSize[1],localWorkSize[2]);
    printf("this is the global worksize %lu %lu %lu",
           globalWorkSize[0],globalWorkSize[1],globalWorkSize[2]);
    //clEnqueueNDRangeKernel(commandQueue, kernel, 3, 0, globalWorkSize, localWorkSize,
	//	                   0, NULL, &GPUExecution);
    
    // for test
    //ciErrNum=clEnqueueNDRangeKernel(commandQueue, kernel, 3, 0, globalWorkSize, localWorkSize,
    //                                0, NULL, NULL);
    
    // for CPU
    clFinish(commandQueue);
    ciErrNum=clEnqueueNDRangeKernel(commandQueue, kernel, 3, 0, globalWorkSize, localWorkSize,
                                    0, NULL, NULL);
    
    // should be go?
    //clEnqueueNDRangeKernel(commandQueue, kernel, 3, 0, globalWorkSize, localWorkSize,
    //                           0, NULL, &gpudone_event);
    
    
    if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" Error: in running kernel "<<ciErrNum<<std::endl;
		//shrLog("clEnqueueReadBuffer() Error: Failed to write buffer!\n");
		return -1;
	}
    
    // not sure what this does
    //clFlush(commandQueue);
    
    // lets just do it all here now....
    clFinish(commandQueue);
    printf(" ready to download ");
    
    // Download result from GPU.
	//cl_event gpudone_event;
    
    // I am not sure about the input size, it should be work size * float * size of job?
    //cl_mem dest_out = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY,
    //                                  mem_size_output_data, NULL, NULL);
    
	//ciErrNum = clEnqueueReadBuffer(commandQueue, dest_out, CL_FALSE, 0, mem_size_output_data, data_out, NULL, NULL, &GPUExecution);
    ciErrNum = clEnqueueReadBuffer(commandQueue, output_data, CL_FALSE, 0, mem_size_output_data, data_out, NULL, NULL, &gpudone_event);
	if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" Error: in reading buffer "<<std::endl;
		//shrLog("clEnqueueReadBuffer() Error: Failed to write buffer!\n");
		return -1;
	}
    
    clFinish(commandQueue);

    printf("done writing");
	// Set callback that will launch another CPU thread to finish the work when the download from GPU has finished
	//ciErrNum = clSetEventCallback(gpudone_event, CL_COMPLETE, &event_callback, arg);
    
    return 0;
    
    
}

// this looks like it should be in classify... but it also looks like it should be in baseclassifier
int BaseClassifier::ProcessQueue(){
    
    // 

    commandQueue = 0;

    commandQueue = clCreateCommandQueue(cxGPUContext, cdDevices[0], (bEnableProfile ? CL_QUEUE_PROFILING_ENABLE : 0), &ciErrNum);
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout<<" problem creating max command queue "<<ciErrNum<<std::endl;       
	    //cutIncrementBarrier(&barrier);
		return ciErrNum;
    }
    
    //I would set max/min here
    // Queue is started, and compiled
    size_t mem_size_const_in = sizeof(cl_float)*ndim;
    max_in = (float*)malloc(mem_size_const_in);
    min_in = (float*)malloc(mem_size_const_in);
    SetMaxMin(max_in,min_in);
    
    printf("setting basics\n");
    
    for (int i=0; i<ciDeviceCount; i++){
        char* kernelname;
        if (cubesetting==1){
            #ifdef __APPLE__
                CompileOCLKernel(cdDevices[i], "/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/fillcubefull.cl", &program[i]);
            #else
                CompileOCLKernel(cdDevices[i], "classifier/src/fillcubefull.cl", &program[i]);  
            #endif
            kernelname="fillcubefull";
        } else if (cubesetting==2){
            kernelname="fillcube2full";
            #ifdef __APPLE__
                CompileOCLKernel(cdDevices[i], "/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/fillcube2full.cl", &program[i]);
            #else
                CompileOCLKernel(cdDevices[i], "classifier/src/fillcube2full.cl", &program[i]);
            #endif
        } else if (cubesetting==3){
            kernelname="fillcube3full";
            #ifdef __APPLE__
                CompileOCLKernel(cdDevices[i], "/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/fillcube3full.cl", &program[i]);
            #else
                CompileOCLKernel(cdDevices[i], "classifier/src/fillcube3full.cl", &program[i]);  
            #endif
            CompileOCLKernel(cdDevices[i], "fillcube3full.cl", &program[i]);
        
        } else {
            std::cout<<" that setting is undefined "<<std::endl;
            return -1;
        }
        // this will be done in general
        cl_mem input_max = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                       mem_size_const_in, max_in, &ciErrNum);
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" problem creating max buffer "<<ciErrNum<<std::endl;
            //shrLog("Error: clCreateBuffer\n");
            return ciErrNum;
        }
        // this will be done in general
        cl_mem input_min = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                       mem_size_const_in, min_in, &ciErrNum);
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" problem creating min buffer "<<std::endl;
            //shrLog("Error: clCreateBuffer\n");
            return ciErrNum;
        }
        kernel[i]= clCreateKernel(program[i], kernelname, &ciErrNum);
        if (ciErrNum != CL_SUCCESS) {
            std::cout<<" problem with creating kernel "<<std::endl;
            return ciErrNum;
        }
        
        // define the arguments
        ciErrNum=clSetKernelArg(kernel[i], 1, sizeof(cl_mem), (void *) &input_max);
        
        
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" problem setting argument 1 "<<std::endl;
            //shrLog("Error: clCreateBuffer\n");
            return ciErrNum;
        }
        ciErrNum=clSetKernelArg(kernel[i], 2, sizeof(cl_mem), (void *) &input_min);
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" problem setting argument 2 "<<std::endl;
            //shrLog("Error: clCreateBuffer\n");
            return ciErrNum;
        }
        
        cl_mem dest_max = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY,
                                          mem_size_const_in, NULL, NULL);

        cl_mem dest_min = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY,
                                         mem_size_const_in, NULL, NULL);
        
        // Copy only assigned rows (h_A) from CPU host to GPU device    
        ciErrNum = clEnqueueCopyBuffer(commandQueue, 
                                       input_max, // src_buffer
                                       dest_max, // dst_buffer
                                       0, // src_offset
                                       0, // dst_offset
                                       mem_size_const_in,  // size_of_bytes to copy
                                       0,  // number_events_in_waitlist
                                       NULL, /// event_wait_list
                                       NULL); // event
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" Error: Failure to copy max buffer "<<ciErrNum<<std::endl;
            //shrLog("clEnqueueCopyBuffer() Error: Failed to copy buffer!\n");
            return -1;
        }
        // Copy only assigned rows (h_A) from CPU host to GPU device    
        ciErrNum = clEnqueueCopyBuffer(commandQueue, 
                                       input_min, // src_buffer
                                       dest_min, // dst_buffer
                                       0, // src_offset
                                       0, // dst_offset
                                       mem_size_const_in,  // size_of_bytes to copy
                                       0,  // number_events_in_waitlist
                                       NULL, /// event_wait_list
                                       NULL); // event
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" Error: Failure to copy min buffer "<<std::endl;
            //shrLog("clEnqueueCopyBuffer() Error: Failed to copy buffer!\n");
            return -1;
        }
        
    }

    // not sure what this does, but I think it means 'do what I have told you to do'
    clFlush(commandQueue);
   
    bool finishproc = FALSE;
    // this will be a method in Eval, Classify, and BaseClassify
    long eventstobeproc = EventsToProcess(); // I actually want this to be Entries in the root files
    
    
    printf("processing!\n");
    
    while (eventstobeproc!=0||!finishproc) {

        
        finishproc=FALSE;
    
        for (int i=0; i<ciDeviceCount; i++) {

            if (eventstobeproc==0){
                std::cout<<" all events processed, continueing"<<std::endl;
                continue;
                
            }
            cl_ulong max_mem_alloc_size;
            clGetDeviceInfo(cdDevices[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
            // remember that I will need to send out mdim*ndim*nevents
            // and input ndim*nevents
            // as well as 2*ndim
            
            // mem_alloc/ndim/mdim/2 ?... I want a total number of events
            // I can compare it to how many entries are left
            
            cl_event gpudone_event; //[ciDeviceCount];
            // here is where I read in the data set :(
            
            long number_events = max_mem_alloc_size/ndim/mdim/2.;
            if (number_events>eventstobeproc) number_events=eventstobeproc;
            // this should be safe?
            
            size_t mem_size_input_data = sizeof(cl_float)*number_events*ndim;
            size_t mem_size_output_data = sizeof(cl_int)*number_events*ndim*mdim;
        
            float* data_in = (float*)malloc(mem_size_input_data);
            int* data_out = (int*)malloc(mem_size_output_data);   
        
            // what if data_in is bigger then the number of events left within the file?
            InputData(number_events,data_in);
            // returns != 0 if fails
        
            eventstobeproc-=number_events;
            ProcessSet(cdDevices[i], cDevicesName[i], kernel[i], number_events, gpudone_event, data_in, data_out, mem_size_output_data, mem_size_input_data);
            // maybe this last one needs to be done later
            
            /// this causes it to break right now
            //clWaitForEvents(ciDeviceCount, &gpudone_event);
            // not sure what this does?
            ProcessOutput(data_out,number_events);
        }
        
        // I need todo the adding the events into the cube array here
        // or into the output tree
        // possibly the difference between the codes is primarily this function being overloaded?
        
        finishproc=TRUE;
    
    }    
    
    // I will end up wanting to put in the calccuberesult processing here, as well as the other component?
    
    clReleaseContext(cxGPUContext);
    
    return 0;
}



// this needs to be run before I use or do anything with the GPU devices
int BaseClassifier::StartQueue(){
    
    // initialize
	cpPlatform    = NULL;
	cdDevices     = NULL; // should that be *cdDevices ??
	ciErrNum      = CL_SUCCESS;
	ciDeviceCount = 0;
    
    
    bEnableProfile = false; // This is to enable/disable OpenCL based profiling
    
    cl_uint num_platforms;
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" failed "<<std::endl;
		return ciErrNum;
	}

    // this is to select..
    cl_platform_id* clPlatformIDs;    
    (clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id)));
    ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
    //ciErrNum = oclGetPlatformIDs(&cpPlatform);
	if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" failed "<<std::endl;
		//shrLog("Error: Failed to create OpenCL context!\n");
		return ciErrNum;
	}
    cpPlatform=clPlatformIDs[0];
    // I should be able to handle multiple platforms here
    
    #ifdef USECPU
        ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_CPU, 0, NULL, &ciDeviceCount);
        cdDevices = (cl_device_id *)malloc(ciDeviceCount * sizeof(cl_device_id) );
        ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_CPU, ciDeviceCount, cdDevices, NULL);
    #else
        ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &ciDeviceCount);
        cdDevices = (cl_device_id *)malloc(ciDeviceCount * sizeof(cl_device_id) );
        ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, ciDeviceCount, cdDevices, NULL);
    #endif
    // Allocate a buffer array to store the names GPU device(s)
    cDevicesName = new char[ciDeviceCount][256];
    
    std::cout<<" device count "<<ciDeviceCount<<std::endl;
    
    if (ciErrNum != CL_SUCCESS) 
    {
        std::cout<<" failed to get IDs"<<ciErrNum<<std::endl;
		return ciErrNum;
	} else 
    {
		for (int i=0; i<(int)ciDeviceCount; i++) {
			clGetDeviceInfo(cdDevices[i], CL_DEVICE_NAME, sizeof(cDevicesName[i]), &cDevicesName[i], NULL);
            std::cout<<"> OpenCL Device "<<cDevicesName[i]<<", cl_device_id: "<<cdDevices[i]<<std::endl;
		}
	}
    
    //Create the OpenCL context
	cxGPUContext = clCreateContext(0, ciDeviceCount, cdDevices, NULL, NULL, &ciErrNum);
	if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" failed to create context"<<ciErrNum<<std::endl;
		return ciErrNum;
	}

    std::cout<<" I think that I have produced all the contexts that I need "<<std::endl;

    return 0;

}

// should should be after I initialize, but before I use the GPU devices
// I need to do this once per device?
int BaseClassifier::CompileOCLKernel(cl_device_id cdDevice, 
                                     const char *ocl_source_filename, cl_program *cpProgram){
    
    
    
    // this is adapted from Nvidia example code
    
    size_t program_length=0;
    program_length=strlen(ocl_source_filename);
    // argv[0] should be some setting... I can just set it
    
    char *source = oclLoadProgSource(ocl_source_filename,"",&program_length);
    
    *cpProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&source, 
                                           &program_length, &ciErrNum);
    
    if (ciErrNum != CL_SUCCESS) {
        std::cout<<"Error: Failed to create program\n"<<std::endl;
		return ciErrNum;
    } else {
        std::cout<<"clCreateProgramWithSource "<<source<<" succeeded, program_length="<<program_length<<std::endl;
    }
	free(source);
    
    // OK, we created program, now it needs to be built
    cl_build_status build_status;
    
    // I think I need to include the devices built here... 
	//ciErrNum = clBuildProgram(*cpProgram, 0, NULL, "-cl-fast-relaxed-math -cl-nv-verbose", NULL, NULL);
    ciErrNum = clBuildProgram(*cpProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
	if (ciErrNum != CL_SUCCESS)
	{
		// write out standard error, Build Log and PTX, then return error
        std::cout<<" problem with building program"<<ciErrNum<<std::endl;
		return ciErrNum;
    } else {
        ciErrNum = clGetProgramBuildInfo(*cpProgram, cdDevice, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
        //shrLog("clGetProgramBuildInfo returned: ");
        if (build_status == CL_SUCCESS) {
            std::cout<<"CL_SUCCESS"<<std::endl;
        } else {
            std::cout<<"CLErrorNumber = "<<ciErrNum<<std::endl;
        }
    }
    
    // print out the build log, note in the case where there is nothing shown, some OpenCL PTX->SASS caching has happened
    /*
    {
        char *build_log;
        size_t ret_val_size;
        ciErrNum = clGetProgramBuildInfo(*cpProgram, cdDevices, CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
        if (ciErrNum != CL_SUCCESS) {
            shrLog("clGetProgramBuildInfo device %d, failed to get the log size at line %d\n", cdDevices, __LINE__);
        }
        build_log = (char *)malloc(ret_val_size+1);
        ciErrNum = clGetProgramBuildInfo(*cpProgram, cdDevices, CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
        if (ciErrNum != CL_SUCCESS) {
            shrLog("clGetProgramBuildInfo device %d, failed to get the build log at line %d\n", cdDevices, __LINE__);
        }
        // to be carefully, terminate with \0
        // there's no information in the reference whether the string is 0 terminated or not
        build_log[ret_val_size] = '\0';
        shrLog("%s\n", build_log );
    }
     */
    return 0;  
    
}



// very simple, wish there was some nicer way to do this but it goes quickly at least?
int BaseClassifier::SetMaxMin(float* max, float* min){
    
    int ls=interface->GetVarListSize();
    
    for (int i=0; i<ls; i++) {
        
        max[i]=interface->GetMax(i);
        min[i]=interface->GetMin(i);
        
    }
    
    return 0;
    
}
