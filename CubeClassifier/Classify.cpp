//
//  Classify.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "Classify.h"
#include "TFile.h"
#include <math.h>

#include "TTree.h"

int Classify::WriteOutput(){


    if (outIO==0) CreateNewTree(interface->FindElem("classtree"));
    
    float cvar[nvars+1];
    
    //float_q cubevars;
    //float numc[NUMCLASS];
    float_q numc;
    float_qr ratio;
    
    outIO->SetOutTreeVar("cubedepth", &cvar[0]);
    
    int i=1;
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    
    for (std::vector<std::string>::iterator it = varnamelist.begin(); it!=varnamelist.end(); ) {
        
        if (interface->GetVarParameter(i-1)>0) {
            it++;
            continue;
        }
        std::string varname = *it; 
        outIO->SetOutTreeVar(varname.c_str(), &cvar[i]);
        i++;
        it++;
    }
    
    // I think in this case I shouldn't do the ratio but should just do the PARAMETER
    for (int j=0; j<NUMCLASS; j++) {
        outIO->SetOutTreeVar(Form("num%d",j), &numc.x[j]);
        if (j==NUMCLASS-1) continue;
        outIO->SetOutTreeVar(Form("ratio%d",j+1), &ratio.x[j]);
    }
    
    
    for (int j=0; j<NUMPARAMETERS-2; j++) {
        outIO->SetOutTreeVar(Form("par%d",j), &numc.y[j]);
    }
    
    // I need to set up the tree...
    
    for (cubeit=cubemap.begin(); cubeit!=cubemap.end(); ++cubeit) {
        
        //nummc=0;
        //numdata=0;
        //numsig=0;
        
        for (int l=0; l<NUMCLASS; l++) {
            numc.x[l]=0;
            if (l==NUMCLASS-1) continue;
            ratio.x[l]=0;
        }
        
        std::vector<int> classification = ((*cubeit).first);
        float_qc cinformation = ((*cubeit).second);
        
        for (int l=0; l<NUMCLASS; l++) {
            numc.x[l]=cinformation.x[l];
        }

        for (int l=0; l<NUMPARAMETERS-2; l++) {
            numc.y[l]=cinformation.y[l];
        }
        
        i=0;
        for (std::vector<int>::iterator it = classification.begin(); it!=classification.end(); ++it){
            
            int cval=*it;
            cvar[i]=(float)cval;
            i++;
        }
        
        for (int l=0; l<NUMCLASS-1; l++) {
            ratio.x[l]=numc.x[l+1]/(numc.x[l+1]+numc.x[0]);
            
        }
        
        //ratiom=numc.x[2]/(numc.x[2]+numc.x[0]);
        
        //ratios=numc.x[1]/(numc.x[1]+numc.x[0]);
        
        // alternate definition
        //ratios = -expf(numsig)+expf(numdata+numsig);
        
        
        
        if (interface->GetPruneStat()>numc.x[0]) continue;
        
        if (NUMCLASS>2) {
            if ((interface->GetPruneSyst()!=0)&&(interface->GetPruneSyst()<abs(0.5-ratio.x[1]))) continue;
        } else {
            if ((interface->GetPruneSyst()!=0)) continue;
            
        }
        outIO->Fill();
        
    }
    outIO->Write();
    
    return 0;
    
}

int Classify::SetMaxMin(float* max, float* min){
    
    int j=0;
    for (int i=0; i<nvars; i++) {
        int par=interface->GetVarParameter(i);
        if (par>0) continue;
        max[j]=interface->GetMax(i);
        min[j]=interface->GetMin(i);
        j++;
    }
    
    return 0;
    
}


int Classify::ProcessOutput(int* output_data, long nevents){
    
    this->CreateCubeMap(output_data,nevents);    
    
    return 0;
    
}

int Classify::CreateCubeMap(int* cubeset_out,long nevents){
    
    float_qc nullfloatqc;
    for (int l=0; l<NUMCLASS; l++) {
        nullfloatqc.x[l]=0;
    }

    for (int l=2; l<NUMPARAMETERS; l++) {
        nullfloatqc.y[l]=0;
    }
    
    //int *varj = (int*)malloc(sizeof(int)*(ndim+1));
    //std::string varjstr;
    
    for (int i=0; i<nevents; i++) {
        for (int k=0; k<mdim; k++) {
            std::vector<int> varj;
            varj.push_back(k);
            for (int j=0; j<ndim; j++) {
                varj.push_back(cubeset_out[i*ndim*mdim+k*ndim+j]);
                 
            }
            cubeit=cubemap.begin();
            
            // I don't know about this
            if (cubemap.find(varj)==cubemap.end()){
                cubemap.insert(cubeit, std::pair< std::vector<int>,float_qc>(varj,nullfloatqc) );
            }
            
            
            cubeit=cubemap.find(varj);
            ((*cubeit).second).x[(int)data[i].x[0]]+=data[i].x[1]; // + (*cubeit).second[(int)data[i][0]];
            for (int j=2; j<NUMPARAMETERS; j++) {
                ((*cubeit).second).y[j-2]+=data[i].x[j];
            }
            if (i%60000==1){
                std::cout<<" here it is (4) "<<varj[0]<<varj[1]<<varj[2]<<varj[3]<<varj[4]<<std::endl;
                std::cout<<" here it is (3) "<<((*cubeit).second).x[0]
                <<" "<<((*cubeit).second).x[1]<<" "<<((*cubeit).second).x[2]<<std::endl;
                std::cout<<" here it is (2) "<<data[i].x[0]<<" "<<data[i].x[1]<<std::endl;
                std::cout<<" here it is (1) "<<test_float[i*5+0]<<" "<<test_float[i*5+1]
                <<" "<<test_float[i*5+2]<<" "<<test_float[i*5+3]<<" i is "<<i<<std::endl;
            }
            
        }
    }  
  
    
    return 0;
}


Classify::Classify(Interface* inputinterface){
    
    // right now this is a big function
    
    interface=inputinterface;
    
    // is mdim just cubelevel?
    mdim=interface->GetCubeLevel();
    
    // obviously
    cubelevel=interface->GetCubeLevel();
    cubesetting=interface->GetCubeSetting();
    
    // not sure about this, this forces the whole set of names to be used...
    // but maybe that is right? if you are not interested just don't include them
    ndim=(interface->GetVarNumber());
    nvars=(interface->GetVarListSize());
     
    currentIO=0;
    outIO=0;
    
    enumber=0;
    
    var = new float[nvars];
    
    return;
    
}

// bad name, this should be
// GetTree or something
int Classify::CreateNewTree(int celem){
    
    
    outIO = new IO<TTree, TFile>(interface->GetOutFileName(celem),
                                 "classtree",celem,
                                 interface->GetType(celem),1);

    
    
    return 0;
    
}



// bad name
int Classify::GetNewTree(int newelem){
    
    
    
    currentIO = new IO<TTree, TFile>(interface->GetFileName(newelem),
                                     interface->GetTreeName(newelem),newelem,
                                     interface->GetType(newelem),0);
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        currentIO->SetTreeVars(varname.c_str(), &(var[i]));
        
    }
    
    interface->SetTreeEntries(newelem, currentIO->GetEntries());
    
    return 0;
}

int Classify::InputData(long nevents, float* data_in){
    
    // for now this is just using default, later I will need to set it here
    float weight=1;
    int currentelem=0;
    
    beginenum=enumber;
    
    if (currentIO==0) {
        beginelem = interface->GetNextElem(0, -1);
        currentelem=beginelem;
    } else {
        beginelem=currentIO->GetElement();
        currentelem = interface->GetNextElem(0, beginelem);
    }
    GetNewTree(currentelem);
    weight=1.0/((float)currentIO->GetEntries());
    
    // let's assume it is root for now
    // this is maybe not necessary?
    
    
    data = new float_d[nevents];
    long cnum=0;
    
    while (cnum<nevents) {
        
        
        int tinfo = currentIO->GetEntry(cnum+enumber);
        
        if (tinfo==0){
            enumber=-cnum;
            currentelem = interface->GetNextElem(0, currentelem);
            GetNewTree(currentelem);
            
            weight=1.0/((float)currentIO->GetEntries());
            
            tinfo = currentIO->GetEntry(cnum+enumber);
        }
        
        for (int i=0; i<nvars; i++) {
            int par=interface->GetVarParameter(i);
            if (par>0) {
                data[cnum].x[par]=var[i];
            } else {
                data_in[cnum*ndim+i]=var[i];
            }
        }
        
        // if there is a weight variable
        // then it should be multiplied here
        if (interface->GetWeightSetting()>0){
            data[cnum].x[1]*=weight;
        } else {
            data[cnum].x[1]=weight;
        }
        data[cnum].x[0]=(float)currentIO->GetType();
        
        cnum++;
    }
    
    test_float = data_in;
    
    enumber+=cnum;
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");
    
    return 0;
    
}


// obvious, but I need to remember that this should be initalized in the constructor
long Classify::EventsToProcess(){    
    
    long val=0;
    
    long v1=0;
    long v2=0;
    long v0=0;
    
    
    // in the end I should replace this (maybe I get these values somewhere else)
    // but I don't think this is where my error is
    
    std::vector<std::string> namelist=interface->GetNameList();
    std::vector<std::string> treelist=interface->GetTreeList();
    std::vector<int> typelist=interface->GetTypeList();
    std::vector<std::string>::iterator tt=treelist.begin();
    std::vector<int>::iterator yt=typelist.begin();
    for (std::vector<std::string>::iterator it=namelist.begin(); it!=namelist.end(); ++it,++tt,++yt){
        
        // paths are relative to home by default
        
        char* pPath;
        pPath = getenv ("HOME");
        std::string basename = Form("%s/",pPath);
        
        std::string filename=*it;
        
        filename = basename + filename;
        
        TFile* tfile= new TFile(filename.c_str());
        std::string treename=*tt;
        TTree* ttree=(TTree*)gDirectory->Get(treename.c_str());
        int typet = *yt;
        
        if (typet<0) continue;
        
        if (typet==0) v0++;
        if (typet==1) v1++;
        if (typet==2) v2++;
        
        
        val += ttree->GetEntries();

    }
        
    //data0 = new float[v0];
    //data1 = new float[v1];
    //data2 = new float[v2];
    
    //val=20;
   // currentttree->Print();
    
    edim=val;
    return val;
}

