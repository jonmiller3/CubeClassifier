//
//  Eval.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_Eval_h
#define CubeClassifier_Eval_h

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <stdio.h>
#include <string.h>
#include "CL/cl.h"
#define FALSE false
#define TRUE true
#endif

#include <vector>
#include <map>

#include "BaseClassifier.h"
#include "Interface.h"

#include "TTree.h"
#include "TFile.h"

class Eval:public BaseClassifier {

    // can I make a map with an array?
    // maybe these things have to exist for both Eval and Cubify?
    std::map< std::vector<int>,float_triple> cubemap;
    std::map< std::vector<int>,float_triple>::iterator cubeit;


    // just the same as classify
    int SetMaxMin(float* max, float* min);

    
    float* var;

    
private:
    
    Interface* interface;

    int ProcessCubeMap(int*,long);
    
public:
    
    Eval(Interface*);
    ~Eval(){}
    int Process(){
        
        StartQueue();
        ProcessQueue();
        
        return 0;
        
    }
    int WriteOutput();
    
};


#endif
