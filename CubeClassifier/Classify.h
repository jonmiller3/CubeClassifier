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

#include "TTree.h"
#include "TFile.h"

// I need this to inherent..

class Classify:public BaseClassifier {
    
    // these are the cubemap
    std::map< std::vector<int>,float_triple> cubemap;
    std::map< std::vector<int>,float_triple>::iterator cubeit;
    
    // array of edim*2 (setting,weight for each element)
    
    
    /*
    float* data0; // signal
    float* data1; // background
    float* data2; // mc
    
    long e1;
    long e2;
    long e0;
     */
    float* data;
    
    long edim;
    
    // these are the redone methods
    int InputData(long,float*);
    int ProcessOutput(int*,long);
    long EventsToProcess();
    int SetMaxMin(float* max, float* min);
    
    
private:
    int maxelem;
    // I need queue number, max array, min array, data array
    
    
    // no longer needed
    //void NormData(int,float*,float*,float*);
    //void CalcCubeResult(int, int, float_triple*);
    
    // this is used in the output
    int CreateCubeMap(int*,long);
 
    Interface* interface;
    
    // variables for keeping track of input
    TFile* currenttfile;
    TTree* currentttree;
    long enumber;
    int currenttype;
    int currentelem;
    
    
    
public:
    Classify(Interface*);
    ~Classify(){}
    void Process();    
    

    
};

#endif

// this is to classify

// one classify, I have multiple base classifier classes where I can run the calc