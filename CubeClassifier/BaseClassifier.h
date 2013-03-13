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


//#include "fillcube.cl.h"
//#include "fillcube.cl"
//#include "fillcube2.cl.h"
//#include "fillcube3.cl.h"
//#include "normdata.cl.h"
//#include "calccuberesult.cl.h"


//#include "fillcubefull.cl.h"
//#include "fillcube2full.cl.h"
//#include "fillcube3full.cl.h"

//#include "fillcubefull.cl"
//#include "fillcube2full.cl"
//#include "fillcube3full.cl"

struct float_triple {
    float x[3];
};

// I think you do it like this:
#define MAX_GPU_COUNT 16

class BaseClassifier {

private:
    
    // I am not sure what should be private or public?
    
public:
    int CompileOCLKernel(cl_device_id, const char*, cl_program*);
    
    float* max_in;
    float* min_in;
    
    cl_command_queue commandQueue;
    
    // new CL variables
    cl_context       cxGPUContext;
    cl_kernel        kernel[MAX_GPU_COUNT];
    cl_program       program[MAX_GPU_COUNT];
    
    char (*cDevicesName)[256];
    
    cl_platform_id  cpPlatform;
	cl_device_id   *cdDevices;
	cl_int          ciErrNum;
	cl_uint         ciDeviceCount;
    
    
    int mdim;
    int ndim;
    
    bool bEnableProfile;
    
    //dispatch_queue_t* queue;
    
    // I might not longer be using these
    //void* mem_in;
    //void* mem_out;
    
    // this is to set what kernels to run and gives mdim
    int cubelevel;
    // setting is needed, level not yet?
    int cubesetting;
    
    // total number of possible queues
    int queuenumber;
   
    // only needed for one calc
    // I think I am no longer using this
    //void* memmax_in;
    //void* memmin_in;
    
    BaseClassifier(){cubesetting=1;}
    BaseClassifier(int nd, int md){cubesetting=2;mdim=md;ndim=nd;}

    
    
    int GetQueueNum(){return queuenumber;}
    
    // old and outdated
    //void NormData(int, int, int, float*);
    
    int StartQueue();
    
    int ProcessQueue();
    
    // I don't know, this is as simple as I could think of it?
    long EventsToProcess(){return 10;}
    int SetMaxMin(float* max, float* min, int ndim){
        for (int i=0; i<ndim; i++){
            max[i]=1;
            min[i]=0;
        }
        return 0;
    }
    int InputData(long, float*);
    int ProcessOutput(int*, long);
    
    // this tells one device to process... theoretically I can 
    int ProcessSet(cl_device_id, char*, cl_kernel, long, cl_event, float*, int*, size_t, size_t);
    
    // old and outdated
    //void FillCube(int, int, int, int, int*);
    //void FillCube2(int, int, int, int, int*);
    //void FillCube3(int, int, int, int, int*);
    
    
    
};


#endif
