//
//  PreProcess.hpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 11/10/15.
//  Copyright © 2015 UTFSM. All rights reserved.
//

#ifndef PreProcess_hpp
#define PreProcess_hpp

#include <stdio.h>

#endif /* PreProcess_hpp */

#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

#include <sstream>
#include <string>


#include "IO.h"
#include "Interface.h"

// I keep thinking that maybe I should inheret from BaseClassifier
// or BaseClassifier and PreProcess should inheret from BaseClass ?
class PreProcess {
    
    IO<TTree, TFile>* currentIO;
    Interface* interface;
    
    std::map<int,std::vector<float> > data_in;

    std::map<int,std::vector<int> > diff_set;
    
    float* var;
    int ndim;
    int cubelevel;
    
    float* max;
    float* min;
    
public:
    
    float GetDifference(std::vector<float>,float);
    const char* CreateOpenCLBuffer();
    PreProcess(Interface*);
    long InputData(long);
    int Process(int);
    int GetNewTree(int);
    int SetMaxMin();
    
};