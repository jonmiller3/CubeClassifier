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

class CreateTestTree {
    TFile* file;
    TTree* tree;
    
public:
    
    CreateTestTree(TString);
    ~CreateTestTree(){delete file; delete tree;}
    void WriteROOTFile();
    void AddVariable();
    
};



#endif
