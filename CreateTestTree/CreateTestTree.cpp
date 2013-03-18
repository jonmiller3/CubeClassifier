//
//  CreateTestTree.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 3/14/13.
//  Copyright (c) 2013 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "CreateTestTree.h"

CreateTestTree::CreateTestTree(TString name, int nv){
    
    TString filename = Form("/Users/jonathanmiller/Desktop/CubeClassifier/CubeClassifier/%s.root",name.Data());
    std::cout<<" file name is "<<filename<<std::endl;
    
    file = new TFile(filename,"RECREATE");
    tree = new TTree("tree"+name,name);
    
    nvar=nv;
    var = new float[nvar];
    func = new TF1*[nvar];
    
}



void CreateTestTree::WriteROOTFile(){
    
    tree->Write();
    file->Write();
    
}

void CreateTestTree::AddVariables(int max=5){
    
    for (int i=0; i<max; i++) {
        AddVariable(i,&var[i]);
    }
    
}

void CreateTestTree::AddVariable(int elem,float* v){
    
    TString name = Form("Variable%d",elem);
    tree->Branch(name,v,Form("%s/F",name.Data()));
    func[elem] = new TF1(Form("%s",name.Data()),"gaus(0)",0,1);
    
}


void CreateTestTree::CreateDefaultSignal(int en){
    
    for (int i=0; i<nvar; i++) {
        
        //double r=rndm.Rndm(en);
        
        func[i]->SetParameter(0,0.1);
        func[i]->SetParameter(1,0.1);
        func[i]->SetParameter(2,0.1);
        
        //std::cout<<" the signal "<<func[i]->Eval(r)<<std::endl;
        
        //var[i]=func[i]->Eval(r);
        var[i]=func[i]->GetRandom();
    }
    
}

void CreateTestTree::CreateDefaultBackground(int en){
    
    for (int i=0; i<nvar; i++) {
        
        //double r=rndm.Rndm(en);
        
        func[i]->SetParameters(100,0.8,0.4);
        
        //var[i]=func[i]->Eval(r);
        var[i]=func[i]->GetRandom();
        
    }
    
}

void CreateTestTree::Loop(int nevents, bool type){
    
    for (int i=0; i<nevents; i++) {
        
        if (type){
            
            CreateDefaultSignal(0);
            
        } else {
            
            CreateDefaultBackground(0);
            
        }
        
        tree->Fill();
        
    }
        
    
}
