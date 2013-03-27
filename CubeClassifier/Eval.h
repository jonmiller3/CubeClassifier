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


struct float_q {
    float x[5];
};


class Eval:public BaseClassifier {

    // can I make a map with an array?
    // maybe these things have to exist for both Eval and Cubify?
    std::map< std::vector<int>,float_q> cubemap;
    std::map< std::vector<int>,float_q>::iterator cubeit;


    // just the same as classify
    int SetMaxMin(float* max, float* min);

    int InputData(long,float*);
    int ProcessOutput(int*,long);
    long EventsToProcess();
    
    float* var;

    // array of length 2xedim?
    float* data;
    long edim;

    // variables for keeping track of output
    TFile* outfile;
    TTree* outtree;
    int outelem;
    
    // variables for keeping track of input
    TFile* currenttfile;
    TTree* currentttree;
    long enumber;
    int currenttype;
    int currentelem;
    
    // new method to get new tree
    int CreateNewTree(int,int);
    
    // better descriptor of old method
    int GetNewTree(int,int);
    
    // this is just for tests, remove
    float* test_float;
    
private:
    
    // this is to add the new branches to the tree
    // this is a bit dangerous, in the future it would be safer to 
    // have all the tree stuff in some other class (here and Classify)
    // which could be replaced by non-tree stuff/etc
    int beginelem;
    long beginenum;
    
    
    Interface* interface;

    int LoadCubeMap();
    
    // seems useful, would be better to do in general?
    TTree* SelectTree(int);
    
public:
    
    Eval(Interface*);
    ~Eval(){}
    int Process(){
        
        LoadCubeMap();
        StartQueue();
        ProcessQueue();
        
        return 0;
        
    }
    int WriteOutput();
    
};


#endif
