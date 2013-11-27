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
    
    // this is now hardcoded
    int nvar;
    
    TRandom3 rndm;
    
public:
    
    CreateTestTree(TString);
    ~CreateTestTree(){file->Close();delete file; delete tree;}
    void WriteROOTFile();
    void AddVariables();
    void AddVariable(int,float*);
    void Loop(int,int);
    void CreateGausVariable(int,double,double,double,double,double);
    void CreateUniformVariable(int,double,double);
    
    double Length(double,double,double,double);
    double Energy(double,double);
    
    double Sanitize(double);
    
};



#endif
