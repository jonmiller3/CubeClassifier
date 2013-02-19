//
//  BaseClassifier.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/26/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>
#include <OpenCL/OpenCL.h>

#include "BaseClassifier.h"
#include "fillcube.cl"
#include "fillcube2.cl"
#include "fillcube3.cl"

#include "fillcubefull.cl"
#include "fillcube2full.cl"
#include "fillcube3full.cl"

// this is really just a dummy method
// will get overloaded with Classify (combine into map)
// and Eval (print into output rootfile) methods
int BaseClassifier::ProcessOutput(int* output_data, int mdim, int ndim, long nevents){
    
    for (int i=0; i<nevents; i++) {
        for (int j=0; j<ndim; j++) {
            for (int k=0; k<mdim; k++) {
                std::out<<" here is the output, I think "<<output_data[]<<std::endl;
            }
        }
    }
    
}

// this is really just the dummy method
// will get overloaded with Classify and Eval methods
int BaseClassifier::InputData(long nevents, float* data_in, int ndim){

    // crazily simple
    for (int i=0; i<nevents; i++) {
        
        for (int j=0; j<ndim; j++) {
            
            data_in[i*ndim+j]=(cl_float)((1+i)/(nevents+ndim+1));
                
            
        }
    }
    
    return 0;
    
}

int BaseClassifier::ProcessSet(cl_device_id device, char* device_name, cl_kernal kernel, int id, cl_event gpudone_event, float* data_in, int* data_out, size_t  mem_size_output_data, size_t  mem_size_input_data){
    

    
    
    // I don't know if I need to do something else to define this
    //cl_command_queue commandQueue = 0;
    // I set this somewhere else
    
    // I don't know if I should be using CL_MEM_USE_HOST_PTR
    
    
    // this will be done in general
    cl_mem input_data = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
				                mem_size_input_data, data_in, &ciErrNum);
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout<<" problem creating buffer "<<std::endl;
        //shrLog("Error: clCreateBuffer\n");
        return ciErrNum;
    }
    
    
    cl_mem output_data = 
        clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY,  mem_size_output_data, NULL, NULL);
    
    
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &input_data);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *) &output_data);
    
    // I need to think of how I deal with the min/max arrays, maybe I set this as global
    // but the special template?
    
    
    //input_size=mem_size_input_data/ciDeviceCount;
    size_t input_size=mem_size_input_data;
    
    //input_offset=0+i*work_size;
    size_t input_offset=0;
    
    // Copy only assigned rows (h_A) from CPU host to GPU device    
    ciErrNum = clEnqueueCopyBuffer(commandQueue, 
                                   data_in, // src_buffer
                                   input_data, // dst_buffer
                                   input_offset, // src_offset
                                   0, // dst_offset
                                   input_size,  // size_of_bytes to copy
                                   0,  // number_events_in_waitlist
                                   NULL, /// event_wait_list
                                   NULL); // event
	if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" Error: Failure to copy buffer "<<std::endl;
		//shrLog("clEnqueueCopyBuffer() Error: Failed to copy buffer!\n");
		return -1;
	}
    
    
    cl_event GPUExecution;
    
    
    // CL_DEVICE_MAX_COMPUTE_UNITS for wgs?
    // CL_DEVICE_MAX_WORK_ITEM_SIZES
    // CL_DEVICE_MAX_WORK_GROUP_SIZE
    
    size_t workitem_size[3];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
    
    // wgs is the number of threads or something of the GPU
    size_t globalWorkSize[] = {dim1_size, dim2_size, dim3_size};
    size_t localWorkSize[] = {dim1_size/workitem_size[0], dim2_size/workitem_size[1], 
        dim3_size/workitem_size[2]};
    
    clEnqueueNDRangeKernel(commandQueue, kernel, 3, 0, globalWorkSize, localWorkSize,
		                   0, NULL, &GPUExecution);
    // not sure what this does
    clFlush(commandQueue);
    
    // Download result from GPU.
	//cl_event gpudone_event;
	ciErrNum = clEnqueueReadBuffer(commandQueue, output_data, CL_FALSE, 0, mem_size_output_data, 0, NULL, &gpudone_event);
	if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" Error: in reading buffer "<<std::endl;
		//shrLog("clEnqueueReadBuffer() Error: Failed to write buffer!\n");
		return -1;
	}
	// Set callback that will launch another CPU thread to finish the work when the download from GPU has finished
	//ciErrNum = clSetEventCallback(gpudone_event, CL_COMPLETE, &event_callback, arg);
    
    return 0;
    
    
}

// this looks like it should be in classify... but it also looks like it should be in baseclassifier
int BaseClassifier::ProcessQueue(int mdim, int ndim){
    
    // 

    commandQueue = 0;

    //I would set max/min here
    // Queue is started, and compiled
    size_t mem_size_const_in = sizeof(cl_float)*ndim;
    max_in = (float*)malloc(mem_size_const_in);
    min_in = (float*)malloc(mem_size_const_in);
    SetMaxMin(max_in,min_in,ndim);
    
    
    for (int i=0; i<ciDeviceCount; i++){
        char* kernelname;
        if (cubesetting=1){
            CompileOCLKernel(cdDevices[i], "fillcubefull.cl", &program[i]);
            kernelname="fillcubefull";
        } else if (cubesetting==2){
            kernelname="fillcube2full";
            CompileOCLKernel(cdDevices[i], "fillcube2full.cl", &program[i]);
        } else if (cubesetting=3){
            kernelname="fillcube3full";
            CompileOCLKernel(cdDevices[i], "fillcube3full.cl", &program[i]);
        
        } else {
            std::cout<<" that setting is undefined "<<std::endl;
            return -1;
        }
        // this will be done in general
        cl_mem input_max = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY,
                                       mem_size_const_in, max_in, &ciErrNum);
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" problem creating max buffer "<<std::endl;
            //shrLog("Error: clCreateBuffer\n");
            return ciErrNum;
        }
        // this will be done in general
        cl_mem input_min = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY,
                                       mem_size_const_in, min_in, &ciErrNum);
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" problem creating min buffer "<<std::endl;
            //shrLog("Error: clCreateBuffer\n");
            return ciErrNum;
        }
        kernel[i]= clCreateKernel(program[i], kernelname, &ciErrNum);
        if (ciErrNum != CL_SUCCESS) {
            std<<cout<<" problem with creating kernel "<<std::endl;
            return ciErrNum;
        }
        
        // define the arguments
        clSetKernelArg(kernel[i], 1, sizeof(cl_mem), (void *) &input_max);
        clSetKernelArg(kernel[i], 2, sizeof(cl_mem), (void *) &input_min);
        
        
        // Copy only assigned rows (h_A) from CPU host to GPU device    
        ciErrNum = clEnqueueCopyBuffer(commandQueue, 
                                       max_in, // src_buffer
                                       input_max, // dst_buffer
                                       0, // src_offset
                                       0, // dst_offset
                                       mem_size_const_in,  // size_of_bytes to copy
                                       0,  // number_events_in_waitlist
                                       NULL, /// event_wait_list
                                       NULL); // event
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout<<" Error: Failure to copy max buffer "<<std::endl;
            //shrLog("clEnqueueCopyBuffer() Error: Failed to copy buffer!\n");
            return -1;
        }
        // Copy only assigned rows (h_A) from CPU host to GPU device    
        ciErrNum = clEnqueueCopyBuffer(commandQueue, 
                                       min_in, // src_buffer
                                       input_min, // dst_buffer
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
            InputData(number_events,data_in,ndim);
            // returns != 0 if fails
        
            eventstobeproc-=number_events;
            ProcessSet(cdDevices[i], cDevicesName[i], kernel[i], i, gpudone_event, data_in, data_out, mem_size_output_data, mem_size_input_data);
            // maybe this last one needs to be done later
            clWaitForEvents(ciDeviceCount, gpudone_event);
            // not sure what this does?
            ProcessOutput(data_out,mdim,ndim,number_events);
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
	*cdDevices     = NULL;
	ciErrNum      = CL_SUCCESS;
	ciDeviceCount = 0;
    
    
    bEnableProfile = false; // This is to enable/disable OpenCL based profiling
    
    ciErrNum = oclGetPlatformID(&cpPlatform);
	if (ciErrNum != CL_SUCCESS)
	{
        std::cout<<" failed "<<std::endl;
		//shrLog("Error: Failed to create OpenCL context!\n");
		return ciErrNum;
	}
    
    
    //Retrieve of the available GPU type OpenCL devices
	ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &ciDeviceCount);
	cdDevices = (cl_device_id *)malloc(ciDeviceCount * sizeof(cl_device_id) );
	ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, ciDeviceCount, cdDevices, NULL);
    
    // Allocate a buffer array to store the names GPU device(s)
    char (*cDevicesName)[256] = new char[ciDeviceCount][256];
    
    if (ciErrNum != CL_SUCCESS) 
    {
        std::cout<<" failed "<<std::endl;
		//shrLog("Error: Failed to create OpenCL context!\n");
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
        std::cout<<" failed "<<std::endl;
		//shrLog("Error: Failed to create OpenCL context!\n");
		return ciErrNum;
	}
    
    // we don't need any of these
    /*
     if(shrCheckCmdLineFlag(argc, (const char**)argv, "profile"))
     {
     bEnableProfile = true;
     }
     
     if(shrCheckCmdLineFlag(argc, (const char**)argv, "device"))
     {
     // User specified GPUs
     char* deviceList;
     char* deviceStr;
     char* next_token;
     shrGetCmdLineArgumentstr(argc, (const char**)argv, "device", &deviceList);
     */
    
    
    deviceStr = strtok (deviceList," ,.-");
    
            
    ciDeviceCount = 0;
    
    while(deviceStr != NULL) 
    {
        // get and print the device for this queue
        cl_device_id device = oclGetDev(cxGPUContext, atoi(deviceStr));
        if( device == (cl_device_id) -1  ) {
            std::cout<<" failed "<<std::endl;
            //shrLog(" Device %s does not exist!\n", deviceStr);
            return -1;
        }
        
        /*
         shrLog("Device %s: ", deviceStr);
         oclPrintDevName(LOGBOTH, device);            
         shrLog("\n");
         
         */
        std::cout<<" device is "<<deviceStr<<std::endl;
        
        ++ciDeviceCount;
        
        deviceStr = strtok (NULL," ,.-");
        
    }
    
    free(deviceList);



    std::cout<<" I think that I have produced all the contexts that I need "<<std::endl;

    return 0;

}

// should should be after I initialize, but before I use the GPU devices
// I need to do this once per device?
int BaseClassifier::CompileOCLKernel(cl_device_id cdDevice, 
                                     const char *ocl_source_filename, cl_program *cpProgram){
    
    
    
    // this is adapted from Nvidia example code
    
    size_t program_length;
    // argv[0] should be some setting... I can just set it
    const char* source_path = shrFindFilePath(ocl_source_filename, argv[0]);
    char *source = oclLoadProgSource(source_path,"",&program_length);
    
    *cpProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&source, 
                                           &program_length, &ciErrNum);
    
    
    
    if (ciErrNum != CL_SUCCESS) {
        std::cout<<"Error: Failed to create program\n")<<std::endl;
		return ciErrNum;
    } else {
        std::cout<<"clCreateProgramWithSource "<<cl_source_filename<<" succeeded, program_length="<<program_length<<std::endl;
    }
	free(source);
    
    // OK, we created program, now it needs to be built
    cl_build_status build_status;
    
	ciErrNum = clBuildProgram(*cpProgram, 0, NULL, "-cl-fast-relaxed-math -cl-nv-verbose", NULL, NULL);
	if (ciErrNum != CL_SUCCESS)
	{
		// write out standard error, Build Log and PTX, then return error
        std::cout<<" problem "<<std::endl;
		return ciErrNum;
    } else {
        ciErrNum = clGetProgramBuildInfo(*cpProgram, cdDevice, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
        shrLog("clGetProgramBuildInfo returned: ");
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
