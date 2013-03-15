//
//  main.cpp
//  CreateTestTree
//
//  Created by Jonathan Miller on 3/14/13.
//  Copyright (c) 2013 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "CreateTestTree.h"

int main (int argc, const char * argv[])
{

    std::string vname1="Variable1";
    
    float v1;

    
    {
    
        TFile* signal = new TFile("TestSignal.root");
    
        TTree* tree = new TTree("treeS", "Signal Tree");
    
        tree->SetBranch(vname1.c_str(),v1,Form("%s/F",vname1));
        
        
        
        
        
        
        signal->Close();
        
    }
    
    
    
    
    
    {
        
        
        TFile* background = new TFile("TestBackground.root");
    
        TTree* tree = new TTree("treeB", "Signal Tree");
        
        
        
        
        
        
        background->Close();
        
    }
    
    return 0;
    
}

