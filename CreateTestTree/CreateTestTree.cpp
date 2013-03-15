//
//  CreateTestTree.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 3/14/13.
//  Copyright (c) 2013 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "CreateTestTree.h"

CreateTestTree::CreateTestTree(TString name){
    
    TFile* file = new TFile(Form("%s.root",name));
    TTree* tree = new TTree("tree"+name);
    
}

void CreateTestTree::WriteROOTFile(){
    
    file->Write();
    file->Class();
    
}