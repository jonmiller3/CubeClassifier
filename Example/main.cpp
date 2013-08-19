//
//  main.cpp
//  Example
//
//  Created by Jonathan Miller on 8/16/13.
//  Copyright (c) 2013 UTFSM. All rights reserved.
//

#include <iostream>

#include "TTree.h"
#include "TH1F.h"
#include "TFile.h"
#include "TH2F.h"


int main(int argc, const char * argv[])
{

    // insert code here...
    
    // this is an example of how to use cubeclassifer
    // as a tool (using ROOT as UI)
    
    
    std::cout << "Hello, World!\n";
    
    TFile *rtfile = new TFile("results.root");
    TFile *vtfile = new TFile("DefaultTest.root");
    TTree *rttree = (TTree*)rtfile->Get("rtree");
    TTree *vttree = (TTree*)vtfile->Get("treeDefaultTest");

    // for this test just hardcode it
    int const mdim=5;
    
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
    
    // this is a bit pointless, the information is already there
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

    // I should take advantage of the ease of ROOT
    
    // this is pretty hardcoded again
    TFile* outfile = new TFile("/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/presults.root","RECREATE");
    TTree *outtree = new TTree("ptree", "Result of Evalulation");
    outtree->AddFriend("treeDefaultTest","/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/DefaultTest.root");
    outtree->AddFriend("rtree","/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/results.root");
    
    
    float pnumsig;
    float pnumdata;
    float pnummc;
    float pratios;
    float pratiom;
    int pelemnum;
    int pkdim;
    
    TBranch *numsigb = outtree->Branch("numsig",&pnumsig,"numsig/F");
    TBranch *numdatab = outtree->Branch("numdata",&pnumdata,"numdata/F");
    TBranch *nummcb = outtree->Branch("nummc",&pnummc,"nummc/F");
    
    TBranch *ratiosb = outtree->Branch("ratios",&pratios,"ratios/F");
    TBranch *ratiosm = outtree->Branch("ratiom",&pratiom,"ratiom/F");
    
    TBranch *elemb   = outtree->Branch("event",&pelemnum,"event/I");
    
    TBranch *kb = outtree->Branch("kdim",&pkdim,"kdim/I");

    
    int ecounter=0;
    
    Long64_t nentries = vttree->GetEntries();
    
    
    for (Long64_t i=0; i<nentries; i++){
    //for (Long64_t i=0; i<20; i++){
        
        vttree->GetEntry(i);
        
        // I think that I can just pull out things together
        

        // it can take a k to limit
        float nsign=-1;
        float ndata=-1;
        float nmcar=-1;
        float rats=-1;
        float ratm=-1;
        for (int j=0; j<mdim; j++){
            
            if (numdata[j]>0.000003&&numsig[j]>0) {
                nsign=numsig[j];
                ndata=numdata[j];
                nmcar=nummc[j];
                rats=ratios[j];
                ratm=ratiom[j];
                pkdim=j;
            }
            // selects best ratio/ec number
            // these should be inputs, maybe
            // even complicated inputs (a TF1)
            
        }
        
        
        pnumdata=ndata;
        pnumsig=nsign;
        pnummc=nmcar;
        pratiom=ratm;
        pratios=rats;
        
        pelemnum=elemnum;
        outtree->Fill();
        
        
        if (rats>0.9) {
            //std::cout<<" We found one "<<std::endl;
            //std::cout<<" this looks like "<<v0<<std::endl;
            //std::cout<<" this looks like "<<v1<<std::endl;
            //std::cout<<" this looks like "<<v2<<std::endl;
            //std::cout<<" this looks like "<<v3<<std::endl;
            //std::cout<<" this looks like "<<v4<<std::endl;

            
            //std::cout<<" this looks like "<<rats<<std::endl;
            //std::cout<<" this looks like "<<ndata<<std::endl;
            //std::cout<<" this looks like "<<nsign<<std::endl;
            ecounter++;
            
            
        }
        
        
    }
    
    outfile->Write();
    
    Printf("This is the total number of passing events %d",ecounter);
    
    return 0;
}

