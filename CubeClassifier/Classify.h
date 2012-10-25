//
//  Classify.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_Classify_h
#define CubeClassifier_Classify_h

#include <OpenCL/OpenCL.h>
#include "fillcube.cl.h"
#include "normdata.cl.h"
#include "calccuberesult.cl.h"

class Classify {
    dispatch_queue_t* queue;
    void* mem_in;
    void* mem_out;
    
    // only needed for one calc
    void* memmax_in;
    void* memmin_in;
    
    // these are the cubemap
    std::map< std::vector<int>,float_triple> cubemap;
    std::map< std::vector<int>,float_triple>::iterator cubeit;
    
    // this is to set what kernels to run and gives mdim
    int cubelevel;
    int cubesetting;
    
    // total number of possible queues
    int queuenumber;
    
    <#instance variables#>
    
    
private:
    
public:
    // constructor (cubelevel and cubesetting)/deconstructor
    Classify(int,int);
    ~Classify();
    
    int GetQueueNum(){return queuenumber;}
    // I need queue number, max array, min array, data array
    NormData(int,float*,float*,float*);
    <#member functions#>
};

#endif

// this is to classify

// I should have multiple versions of the thing here?
// or just one?
// I can have multiple runs/qeues/etc?


// do I read in data from within classify or from without?