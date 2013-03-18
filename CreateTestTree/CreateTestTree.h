//
//  CreateTestTree.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 3/14/13.
//  Copyright (c) 2013 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_CreateTestTree_h
#define CubeClassifier_CreateTestTree_h

#include "TTree.h"
#include "TFile.h"
#include "TF1.h"
#include "TString.h"
#include "TRandom3.h"

class CreateTestTree {
    TFile* file;
    TTree* tree;
    float* var;
    TF1** func;
    int nvar;
    
    TRandom3 rndm;
    
public:
    
    CreateTestTree(TString,int);
    ~CreateTestTree(){file->Close();delete file; delete tree;}
    void WriteROOTFile();
    void CreateDefaultSignal(int);
    void CreateDefaultBackground(int);
    void AddVariables(int);
    void AddVariable(int,float*);
    void Loop(int,bool);
    
};



#endif
