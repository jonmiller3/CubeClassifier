//
//  BaseClassifier.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/26/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_BaseClassifier_h
#define CubeClassifier_BaseClassifier_h

#ifdef __APPLE__
    #include <OpenCL/OpenCL.h>
#else
    #include <stdio.h>
    #include <string.h>
    #include "CL/cl.h"
    #define FALSE false
    #define TRUE true
#endif

#include "Interface.h"

// I think you do it like this:
#define MAX_GPU_COUNT 16

class BaseClassifier {

// can't access outside, can access with daughters
protected:  


    
    float* max_in;
    float* min_in;
    
    cl_command_queue commandQueue[MAX_GPU_COUNT];
    
    // new CL variables
    cl_context       cxGPUContext;
    cl_kernel        kernel[MAX_GPU_COUNT];
    cl_program       program;
    cl_bool          cDevicesAvailable[MAX_GPU_COUNT];
    
    char (*cDevicesName)[256];
    
    cl_platform_id  cpPlatform;
	cl_device_id   *cdDevices;
	cl_int          ciErrNum;
	cl_uint         ciDeviceCount;
    
    
    int mdim;
    int ndim;
 
    // this is to set what kernels to run and gives mdim
    int cubelevel;
    // setting is needed, level not yet?
    int cubesetting;
    
    // total number of possible queues
    int queuenumber;
    
    
    bool bEnableProfile;

    // this is what should be overloaded
    virtual int InputData(long, float*);
    virtual long EventsToProcess(){return 10;}
    
    
    /*
    virtual int SetMaxMin(float* max, float* min){
        for (int i=0; i<ndim; i++){
            max[i]=1;
            min[i]=0;
        }
        return 0;
    }
     */
    
    virtual int ProcessOutput(int*, long);
    
    // not sure if this should be private or virtual
    // this tells one device to process... theoretically I can 
    int ProcessSet(cl_command_queue, cl_device_id, char*, cl_kernel, long, cl_event, float*, int*, size_t, size_t);
    
    
    Interface* interface;
    
// can't access outside, can't access with daughters
private:
   
    int CompileOCLKernel(cl_device_id, const char*, cl_program*);
    

// can access outside, can access with daughters    
public:
    
    BaseClassifier(){cubesetting=1;}
    BaseClassifier(int nd, int md){cubesetting=2;mdim=md;ndim=nd;}

    int SetMaxMin(float* max, float* min);

    
    int GetQueueNum(){return queuenumber;}
    
    int StartQueue();
    
    int ProcessQueue();    
    
};


#endif
