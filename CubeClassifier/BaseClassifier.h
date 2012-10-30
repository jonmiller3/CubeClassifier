//
//  BaseClassifier.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/26/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_BaseClassifier_h
#define CubeClassifier_BaseClassifier_h

#include <OpenCL/OpenCL.h>
#include "fillcube.cl.h"
#include "fillcube2.cl.h"
#include "fillcube3.cl.h"
#include "normdata.cl.h"
#include "calccuberesult.cl.h"

#include "fillcubefull.cl.h"
#include "fillcube2full.cl.h"
#include "fillcube3full.cl.h"

struct float_triple {
    float x[3];
};

// I think you do it like this:
#define MAX_GPU_COUNT 16

class BaseClassifier {

public:
    
    // new CL variables
    cl_context       cxGPUContext;
    cl_kernel        kernel[MAX_GPU_COUNT];
    cl_program       program[MAX_GPU_COUNT];
    
    
    cl_platform_id  cpPlatform;
	cl_device_id   *cdDevices;
	cl_int          ciErrNum;
	cl_uint         ciDeviceCount;
    
    bool bEnableProfile;
    
    //dispatch_queue_t* queue;
    
    void* mem_in;
    void* mem_out;
    
    // this is to set what kernels to run and gives mdim
    int cubelevel;
    int cubesetting;
    
    // total number of possible queues
    int queuenumber;
   
    // only needed for one calc
    void* memmax_in;
    void* memmin_in;
    
    BaseClassifier(){}
    
    int GetQueueNum(){return queuenumber;}
    void NormData(int, int, int, float*);
    
    int CompileOCLKernel(cl_context, cl_device_id, const char*, cl_program*);
    
    void StartQueue();
    
    void ProcessQue(int,int);
    
    // I don't know, this is as simple as I could think of it?
    long EventsToProcess(){return 1000000}
    int InputData(long, float*, int);
    int ProcessOutput(int*, int, int, long);
    
    int ProcessSet(cl_device_id, char*, cl_kernal, int, cl_event);
    
    void FillCube(int, int, int, int, int*);
    void FillCube2(int, int, int, int, int*);
    void FillCube3(int, int, int, int, int*);
    
};


#endif
