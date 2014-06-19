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
    
    TString filename = Form("/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/%s.root",name.Data());
    std::cout<<" file name is "<<filename<<std::endl;
    
    file = new TFile(filename,"RECREATE");
    tree = new TTree("tree"+name,name);
    
    nvar=14;
    var = new float[nvar];
    
    func = new TF1*[8];
    
}



void CreateTestTree::WriteROOTFile(){
    
    tree->Write();
    file->Write();
    
}

void CreateTestTree::AddVariables(){
    
    for (int i=0; i<nvar; i++) {
        AddVariable(i,&var[i]);
    }
    
}

void CreateTestTree::AddVariable(int elem,float* v){
    
    TString name = Form("Variable%d",elem);
    tree->Branch(name,v,Form("%s/F",name.Data()));
    
}


void CreateTestTree::CreateGausVariable(int elem, double a, double mu, double sigma, double low, double high){

    // this works for background, noise, signal, etc
    TString name = Form("V%d",elem);
    func[elem] = new TF1(Form("%s",name.Data()),"gaus(0)",low,high);
    func[elem]->SetParameters(a,mu,sigma);
    
}

void CreateTestTree::CreateUniformVariable(int elem, double low, double high){
    
    // this works for background, noise, signal, etc
    TString name = Form("V%d",elem);
    func[elem] = new TF1(Form("%s",name.Data()),"[0]",low,high);
    func[elem]->SetParameter(0,1);
    
}

double CreateTestTree::Sanitize(double f){
    
    double rn = rndm.Uniform();
    
    if (f<0||f>1) return rn;
    
    return f;
    
}


double CreateTestTree::Length(double energy, double start, double coef, double region){
    
    if (start>region) return 0;
    
    double length=energy/coef;
    if (length>region-start) return (region-start);
    
    return length;
    
}

double CreateTestTree::Energy(double length, double coef){
    
    double energy=coef*length;
    return energy;
    
}


void CreateTestTree::Loop(int nevents, int type){
    
    // type is signal
    // this is where I setup functions
    
    // this is the energy variable (MeV, so 100s)
    CreateGausVariable(0, 1, type, 20, 100, 200);
    CreateUniformVariable(1, -100, 100);
    
    // pedestal region 1
    CreateGausVariable(2, 100, 0, 10, 0, 100);
    // pedestal region 2
    CreateGausVariable(3, 100, 0, 15, 0, 100);
    
    // coef for region 1
    CreateGausVariable(4, 1, 1, .2, 0, 2);
    
    // coef for region 2
    CreateGausVariable(5, 1, 1.5, .3, 0, 2);
    
   
    //region 1 energy observation
    CreateGausVariable(6, 1, .6, .1, 0, 1);
    //region 2 energy observation
    CreateGausVariable(7, 1, .3, .2, 0, 1);

    
    for (int i=0; i<nevents; i++) {
        
        // energy, start
        // ped region 1 & 2
        for (int i=0; i<6; i++) {
            var[i]=func[i]->GetRandom();
        }
        
        // true energy and lengths (region1)
        var[6]=Length(var[0],var[1],var[4],50);
        var[8]=Energy(var[6],var[4]);
        // true energy and lengths (region2)
        var[7]=Length(var[0]-var[8],50,var[5],100);
        var[9]=Energy(var[7], var[5]);
        
        // observed energy
        if (var[6]>50) {
            var[6]=50;
            var[8]=Energy(var[6],var[4]);
        }
        var[10]=var[8]*func[6]->GetRandom()+var[2];
        var[11]=var[9]*func[7]->GetRandom()+var[3];
        
        double rn = rndm.Uniform(0,10);
   
        // observed lengths
        var[12]=Sanitize((int(var[6])+int(50*var[2])+rn)/100.);
        var[13]=Sanitize((int(var[7])+int(50*var[3])+rn)/100.);
        
        tree->Fill();
        
    }
        
    
}
