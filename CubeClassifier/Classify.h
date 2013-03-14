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

#include <vector>
#include <map>

#include "BaseClassifier.h"
#include "Interface.h"

// I need this to inherent..

class Classify:public BaseClassifier {
    
    // these are the cubemap
    std::map< std::vector<int>,float_triple> cubemap;
    std::map< std::vector<int>,float_triple>::iterator cubeit;
    
    // array of edim*2 (setting,weight for each element)
    double* data;
    int edim;
    
private:
    // I need queue number, max array, min array, data array
    //void NormData(int,float*,float*,float*);
    void CreateCubeMap(int*);
    //void CalcCubeResult(int, int, float_triple*);
    
    Interface* interface;
    
public:
    Classify(Interface*);
    ~Classify(){}
    void Process();    
    
    int InputData();
    
};

#endif

// this is to classify

// one classify, I have multiple base classifier classes where I can run the calc