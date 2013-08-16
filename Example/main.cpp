//
//  main.cpp
//  Example
//
//  Created by Jonathan Miller on 8/16/13.
//  Copyright (c) 2013 UTFSM. All rights reserved.
//

#include <iostream>

#include "TTree.h"
#include "TFile.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    
    // this is an example of how to use cubeclassifer
    // as a tool (using ROOT as UI)
    
    
    std::cout << "Hello, World!\n";
    
    TFile *rtfile = new TFile("result.root");
    TFile *vtfile = new TFile("DefaultTest.root");
    TTree *rttree = (TTree*)rtfile->Get("rtree");
    TTree *vttree = (TTree*)vtfile->Get("treeDefaultTest");

    // for this test just hardcode it
    int mdim=5;
    
    // results tree
    float ratios[mdim];
    float ratiom[mdim];
    float numsig[mdim];
    float numdata[mdim];
    float nummc[mdim];
    int   kdim[mdim];
    int   elemnum;
    
    
    rttree->SetBranchAddress("numsig",numsig);
    rttree->SetBranchAddress("numdata",numdata);
    rttree->SetBranchAddress("nummc",nummc);
    
    rttree->SetBranchAddress("ratios",ratios);
    rttree->SetBranchAddress("ratiom",ratiom);
    
    rttree->SetBranchAddress("kdim", kdim);
    
    rttree->SetBranchAddress("event",&elemnum);
    
    
    float v0,v1,v2,v3,v4;
    
    // who is this?
    vttree->SetBranchAddress("Variable0",&v0);
    vttree->SetBranchAddress("Variable1",&v1);
    vttree->SetBranchAddress("Variable2",&v2);
    vttree->SetBranchAddress("Variable3",&v3);
    vttree->SetBranchAddress("Variable4",&v4);
    
    
    vttree->AddFriend(rttree);

    
    Long64_t nentries = vttree->GetEntries();
    
    //for (Long64_t i=0; i<nentries; i++){
    for (Long64_t i=0; i<20; i++){
        
        vttree->GetEntry(i);
        
        // I think that I can just pull out things together
        
        
        // test things together somehow
        std::cout<<" oh "<<v0<<" "<<v1<<" "<<ratios[0]<<std::endl();
        
        
        
        
    }
    
    
    
    
    return 0;
}

