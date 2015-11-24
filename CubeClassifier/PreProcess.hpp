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


#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>


#include "IO.h"
#include "Interface.h"

#include "Base.hpp"

// I keep thinking that maybe I should inheret from BaseClassifier
// or BaseClassifier and PreProcess should inheret from BaseClass ?
class PreProcess:public Base {
    
    std::map<int,std::vector<float> > data_in;

    std::map<int,std::vector<int> > diff_set;

    

    float* max;
    float* min;
    
public:
    
    float GetDifference(std::vector<float>,float);
    std::string CreateOpenCLBuffer();
    PreProcess(Interface*);
    //long InputData(long);
    int Process(int);
    //int GetNewTree(int);
    int SetMaxMin();
    int WriteOutput(std::string);
    int SetData(int,int,int);
    int WriteOutput(){std::string filename=Form("preprocess.cl"); return WriteOutput(filename);}
    
};

#endif /* PreProcess_hpp */
