//
//  Classify.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_Classify_h
#define CubeClassifier_Classify_h

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
#include "IO.h"


#include "TTree.h"
#include "TFile.h"


class Classify:public BaseClassifier {
    
    // these are the cubemap
    std::map< std::vector<int>,float_qc> cubemap;
    std::map< std::vector<int>,float_qc>::iterator cubeit;
    
    // array of edim*2 (setting,weight for each element)
    float_d* data;
    
    long edim;
    
    // these are the redone methods
    int InputData(long,float*);
    int ProcessOutput(int*,long);
    long EventsToProcess();
    //int SetMaxMin(float* max, float* min);
    
    // this isn't well protected, but this works for now
    float* var;
    int nvars;
    
    // this is just for tests, remove
    float* test_float;
    
    IO<TTree, TFile>* outIO;
    IO<TTree, TFile>* currentIO;
    
    
private:
    int maxelem;
    // I need queue number, max array, min array, data array
    
    // this is used in the output
    int CreateCubeMap(int*,long);
    
    // variables for keeping track of input
    //TFile* currenttfile;
    //TTree* currentttree;
    long enumber;
    //int currenttype;
    //int currentelem;
    
    
    int beginelem;
    long beginenum;
    
    int CreateNewTree(int);
    int GetNewTree(int);
    
public:
    Classify(Interface*);
    ~Classify(){delete outIO;}
    int Process(){
        
        StartQueue();
        ProcessQueue();
    
        return 0;
        
    }
    int WriteOutput();
    

    
};

#endif

// this is to classify