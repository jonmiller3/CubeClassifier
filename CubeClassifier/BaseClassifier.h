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

struct float_triple {
    float x[3];
};

class BaseClassifier {

public:
    dispatch_queue_t* queue;
    
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
    
    void FillCube(int, int, int, int, int*);
    void FillCube2(int, int, int, int, int*);
    void FillCube3(int, int, int, int, int*);
    
};


#endif
