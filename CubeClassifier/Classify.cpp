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
    
    // assuming ROOT
    // that will have to be something that I set/don't set later
    
    // latter I want to be able to prune/etc here too?
    
    TFile* outputfile = new TFile("/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/classify_output.root","RECREATE");
    
    TTree* outputtree = new TTree("classtree","Classify Tree");
    
    float cvar[ndim+1];
    float ratios;
    float ratiom;
    float numsig;
    float numdata;
    float nummc;
    
    outputtree->Branch("cubedepth", &cvar[0],"cubedepth/F");
    
    int i=1;
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    
    for (std::vector<std::string>::iterator it = varnamelist.begin(); it!=varnamelist.end(); ++it,++i ) {
        
        std::string varname = *it; 
        outputtree->Branch(varname.c_str(), &cvar[i], Form("%s/F",varname.c_str()));
        
    }
    
    outputtree->Branch("ratios", &ratios, "ratios/F");
    outputtree->Branch("ratiom", &ratiom, "ratiom/F");
    
    outputtree->Branch("numsig", &numsig, "numsig/F");
    outputtree->Branch("numdata", &numdata, "numdata/F");
    outputtree->Branch("nummc", &nummc, "nummc/F");
    

    // I need to set up the tree... 
    
    for (cubeit=cubemap.begin(); cubeit!=cubemap.end(); ++cubeit) {
        
        nummc=0;
        numdata=0;
        numsig=0;
        
        std::vector<int> classification = ((*cubeit).first);
        float_triple cinformation = ((*cubeit).second);
        
        //std::cout<<" cinfomration "<<cinformation.x[0]<<std::endl;
        
        numsig=cinformation.x[0];
        numdata=cinformation.x[1];
        nummc=cinformation.x[2];
        
        i=0;
        for (std::vector<int>::iterator it = classification.begin(); it!=classification.end(); ++it,++i){
            
            int cval=*it;
            cvar[i]=(float)cval;
            //std::cout<<"cvar "<<cvar[i]<<" the iter "<<*it<<" "<<i<<" "<<cval<<std::endl;
        }
        
        //std::cout<<" val "<<cvar[0]<<" "<<cvar[1]<<" "<<cvar[2]<<" "<<cvar[3]<<" "<<cvar[4]<<std::endl;
        
        ratiom=nummc/(nummc+numdata);
        
        ratios=numsig/(numdata+numsig);
        
        // alternate definition
        //ratios = -expf(numsig)+expf(numdata+numsig);
        
        if (interface->GetPruneStat()>numdata) continue;
        
        if ((interface->GetPruneSyst()!=0)&&(interface->GetPruneSyst()<abs(0.5-ratiom))) continue;
        
        //std::cout<<" ratiotest "<<numsig<<" "<<numdata<<" "<<ratios<<std::endl;
        
        outputtree->Fill();
        
    }
    outputtree->Write();
    outputfile->Write();
    outputfile->Close();
    
    return 0;
    
}


int Classify::ProcessOutput(int* output_data, long nevents){
    
    this->CreateCubeMap(output_data,nevents);    
    
    return 0;
    
}

int Classify::CreateCubeMap(int* cubeset_out,long nevents){
    
    float_triple nullfloat3 = {{0,0,0}};
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
                cubemap.insert(cubeit, std::pair< std::vector<int>,float_triple>(varj,nullfloat3) );
            }
            
            
            cubeit=cubemap.find(varj);
            ((*cubeit).second).x[(int)data[i*2+0]]+=data[i*2+1]; // + (*cubeit).second[(int)data[i][0]];
            if (i%60000==1){
                std::cout<<" here it is (4) "<<varj[0]<<varj[1]<<varj[2]<<varj[3]<<varj[4]<<std::endl;
                std::cout<<" here it is (3) "<<((*cubeit).second).x[0]
                <<" "<<((*cubeit).second).x[1]<<" "<<((*cubeit).second).x[2]<<std::endl;
                std::cout<<" here it is (2) "<<data[i*2+0]<<" "<<data[i*2+1]<<std::endl;
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
    ndim=(interface->GetVarListSize());
    
    /*
    currenttfile=0;
    currentttree=0;
    currenttype=-1;
    currentelem=0;
    */
     
    currentIO=0;
    outIO=0;
    
    
    enumber=0;
    
    var = new float[ndim];
    
    
    //e0=0;
    //e1=0;
    //e2=0;
    
    //maxelem=0;

    //CreateNewTree(4);

    return;
    
    
}

// bad name, this should be
// GetTree or something
int Classify::CreateNewTree(int celem){
    
    //    if (interface->GetType(celem)!=ctype){
    //        celem++;
    //        CreateNewTree(ctype, celem);
    //    }
    //
    //    outfile = new TFile((interface->GetOutFileName(celem)).c_str(),"RECREATE");
    //    outtree = new TTree("rtree","Result of Evalulation");
    //    outtree->AddFriend((interface->GetTreeName(celem)).c_str(),(interface->GetFileName(celem)).c_str());
    //
    //    // I should have a better way of setting the current tree
    //    GetNewTree(4, celem);
    //
    //    return 0;
    
    
    // I get the celem and use it
    
    outIO = new IO<TTree, TFile>(interface->GetOutFileName(celem),
                                 "rtree",celem,
                                 interface->GetType(celem),1);
    
    outIO->AddFriend(interface->GetTreeName(celem),interface->GetFileName(celem));
    
    return 0;
    
}


/*
int Classify::CreateNewTree(int ctype){
    
    
    
    std::string filename=interface->GetNameList()[currentelem];
    currenttfile= new TFile(filename.c_str());
    std::string treename=interface->GetTreeList()[currentelem];
    currentttree=(TTree*)gDirectory->Get(treename.c_str());
    currenttype=interface->GetTypeList()[currentelem];  
    
    // this isn't so neat as I first imagined
    // this is broken, I need to rethink
    // basically it keeps going and goes to trees/files that are not applicable
    if (currenttype==ctype&&currentelem<((interface->GetNameList()).size()-1)) {
        currentelem++;
        CreateNewTree(ctype);
    }
    if (currenttype==-1&&currentelem<((interface->GetNameList()).size()-1)) {
        currentelem++;
        CreateNewTree(ctype);
    }
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {

        std::string varname = *it; 
        currentttree->SetBranchAddress(varname.c_str(), &(var[i]));
        
    }    
        
    return 0;
}
 */

// bad name
int Classify::GetNewTree(int newelem){
    
    
    //    int celem=0;
    //    if (currentIO==0){
    //        celem = -1;
    //    } else {
    //        int celem = currentIO->GetElement();
    //    }
    //
    //
    //    int newelem = interface->GetNextElem(ctype, celem);
    
    currentIO = new IO<TTree, TFile>(interface->GetFileName(newelem),
                                     interface->GetTreeName(newelem),newelem,
                                     interface->GetType(newelem),0);
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        currentIO->SetTreeVars(varname.c_str(), &(var[i]));
        
        //currentttree->SetBranchAddress(varname.c_str(), &(var[i]));
        
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
    
    
    data = new float[nevents*2];
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
        
        for (int i=0; i<ndim; i++) {
            data_in[cnum*ndim+i]=var[i];
        }
        
        data[2*cnum+1]=weight;
        data[2*cnum+0]=(float)currentIO->GetType();
        
        cnum++;
    }
    
    test_float = data_in;
    
    enumber+=cnum;
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");
    
    return 0;
    
}

/*
// this is a special
// input is number of events
// output is the data_in that is used in one set of processing
// I need to keep track of which file is open
// which files are done
// where I am in the file
int Classify::InputData(long nevents, float* data_in){
     
    // for now this is just using default, later I will need to set it here
    float weight=1;
    
    if (currenttfile==0) CreateNewTree(4);
        
    weight=1.0/((float)currentttree->GetEntries());
    
    // let's assume it is root for now
    // this is maybe not necessary?


    data = new float[nevents*2];    
    long cnum=0;
    
    while (cnum<nevents) {
    
        
        int tinfo = currentttree->GetEntry(cnum+enumber);
        
        if (tinfo==0) currenttfile=0;
            
        if (currenttfile==0){
            enumber=-cnum;
            currentelem++;
            CreateNewTree(4);
            
            weight=1.0/((float)currentttree->GetEntries());

            tinfo = currentttree->GetEntry(cnum+enumber);
        }
        if (tinfo==0) currenttfile=0;
        
        for (int i=0; i<ndim; i++) {
            data_in[cnum*ndim+i]=var[i];
        }
        
        data[2*cnum+1]=weight;
        data[2*cnum+0]=(float)currenttype;
        
        //if (currenttype==0) {data0[e0]=var[i];e0++;}
        //if (currenttype==1) {data1[e1]=var[i];e1++;}
        //if (currenttype==2) {data2[e2]=var[i];e2++;}
        
        cnum++;
    }
    
    test_float = data_in;
    
    enumber+=cnum;
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");

    return 0;
    
}
*/


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
        
        std::string filename=*it;
        TFile* tfile= new TFile(filename.c_str());
        std::string treename=*tt;
        TTree* ttree=(TTree*)gDirectory->Get(treename.c_str());
        int typet = *yt;
        
        if (typet<0||typet>3) continue;
        
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

// very simple, wish there was some nicer way to do this but it goes quickly at least?
/*
int Classify::SetMaxMin(float* max, float* min){
    
    int i=0;
    
    std::vector<double> minlist = interface->GetMinList();
    std::vector<double> maxlist = interface->GetMaxList();
    
    std::vector<double>::iterator minit=minlist.begin();
    for (std::vector<double>::iterator maxit=maxlist.begin(); maxit!=maxlist.end(); ++maxit,++minit,i++) {
        
        max[i]=*maxit;
        min[i]=*minit;
 
    }
    
    return 0;
    
}
 */
