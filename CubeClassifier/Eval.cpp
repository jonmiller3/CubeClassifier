//
//  Eval.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include "Eval.h"
#include <iostream>

/*
// very simple, wish there was some nicer way to do this but it goes quickly at least?
int Eval::SetMaxMin(float* max, float* min){
    
    int ls=interface->GetVarListSize();
    
    for (int i=0; i<ls; i++) {
        
        max[i]=interface->GetMax(i);
        min[i]=interface->GetMin(i);
        
    }
        
    return 0;
    
}
 */


Eval::Eval(Interface* inputinterface){
    
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
    

    currentIO=0;
    outIO=0;
    
    
    enumber=0;
    
    var = new float[ndim];

    return;
}

//TTree* Eval::SelectTree(int stype){
//    
//    TTree* ctree;
//    bool fflag=false;
//    
//    // first find the tree
//    std::vector<int> typelist=interface->GetTypeList();
//    
//    std::vector<std::string> namelist=interface->GetNameList();
//    std::vector<std::string> treelist=interface->GetTreeList();
//    std::vector<int>::iterator yt=typelist.begin();
//    std::vector<std::string>::iterator tt=treelist.begin();
//    for (std::vector<std::string>::iterator it=namelist.begin(); it!=namelist.end(); ++it,++tt,++yt){
//        
//        if (fflag) continue;
//        
//        std::string filename=*it;
//        TFile* tfile= new TFile(filename.c_str());
//        std::string treename=*tt;
//        TTree* ttree=(TTree*)gDirectory->Get(treename.c_str());
//        int typet = *yt;
//        
//        // -1 will be classify type
//        if (typet==stype){
//            
//            ctree=ttree;
//            fflag=true;
//            
//        }
//        
//    }    
//    
//    return ctree;
//    
//    
//}


// this loads from the interface
int Eval::LoadCubeMap(){

    // I am assuming that -1 is the type of Classify Control Root Files
    int conelem = interface->GetNextElem(-1, -1);
    
    IO<TTree, TFile>* controlIO = new IO<TTree, TFile>(interface->GetFileName(conelem),
                                                       interface->GetTreeName(conelem),conelem,
                                                       interface->GetType(conelem),0);
    
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        controlIO->SetTreeVars(varname.c_str(), &(var[i]));
        //ctree->SetBranchAddress(varname.c_str(), &(var[i]));
        
    }    
    
    // crazy hacks
    float numsig;
    float numdata;
    float nummc;
    float ratios;
    float ratiom;
    float cubedepth;
    
    controlIO->SetTreeVar("numsig", &numsig);
    controlIO->SetTreeVar("nummc", &nummc);
    controlIO->SetTreeVar("numdata", &numdata);
    
    controlIO->SetTreeVar("ratios", &ratios);
    controlIO->SetTreeVar("ratiom", &ratiom);
    controlIO->SetTreeVar("cubedepth", &cubedepth);
    
    
    float_q nullfloat5= {{0,0,0,0,0}};
    int tinfo=1;
    i=0;
    while (tinfo>0) {
        tinfo = controlIO->GetEntry(i);
        if (tinfo<1) continue; // not sure if I need this
        
        // let's put in my cuts
        if ((interface->GetPruneSyst()!=0)&&(interface->GetPruneStat()<abs(0.5-ratiom))) continue;
        
        // this should probably be that both need to be above...
        if (interface->GetPruneStat()>numdata) continue;
        
        std::vector<int> varj;
        
        (cubedepth)+=0.01; // not sure if this is necessary..
        varj.push_back((int)(cubedepth));
        
        for (int j=0; j<ndim; j++) {
            var[j]+=0.01; // not sure if this is necessary..
            varj.push_back((int)var[j]);
        }
        
        float_q cmval;
        cmval.x[0]=ratios;
        cmval.x[1]=ratiom;
        
        cmval.x[2]=numsig;
        cmval.x[3]=numdata;
        cmval.x[4]=nummc;
        
        
        // maybe I should do a find just in case someone has two of the same thing
        // but that shouldn't be possible (and would make it slower)
        
        
        cubemap.insert ( std::pair<std::vector<int>,float_q>(varj,cmval) );
        
        // I coudl also do .first and .second
        
        
        i++;
    }
    
    return 0;
}

// bad name
int Eval::GetNewTree(int newelem){
    

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

// this is a special 
// input is number of events
// output is the data_in that is used in one set of processing
// I need to keep track of which file is open
// which files are done
// where I am in the file
int Eval::InputData(long nevents, float* data_in){
    
    // for now this is just using default, later I will need to set it here
    float weight=1;
    int currentelem=0;
    
    beginenum=enumber;
    
    if (currentIO==0) {
        beginelem = interface->GetNextElem(-4, -1);
        currentelem=beginelem;
    } else {
        beginelem=currentIO->GetElement();
        currentelem = interface->GetNextElem(-4, beginelem);
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
            currentelem = interface->GetNextElem(-4, currentelem);
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


// obvious, but I need to remember that this should be initalized in the constructor
// this is the old way, but it shouldn't be calling any problems so I can leave it
long Eval::EventsToProcess(){    
    
    long val=0;
    
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
        
        if (typet!=-4) continue;

        val += ttree->GetEntries();
        
    }

    edim=val;
    return val;
}


// I need to put the new vqlues directly into the tree
// friend tree? or I can just make a new 'tree'
// I should have spectator variables and index variables
// for now just output it all
int Eval::ProcessOutput(int* output_data, long nevents){    
    
    // general way of doing it?
    // not sure if I should have outIO
    if (outIO==0) CreateNewTree(beginelem);
    // otherwise outIO exists and we continue to write to it
    // I can probably get this from outIO?
    int currentelem=beginelem;

    
    float ratios[mdim];
    float ratiom[mdim];
    float numsig[mdim];
    float numdata[mdim];
    float nummc[mdim];
    int   kdim[mdim];
    int   elemnum;
    
    outIO->SetOutTreeVar("mdim", &mdim);
    outIO->SetOutTreeVar("event", &elemnum);
    outIO->SetOutTreeVars("numsig", numsig, "mdim");
    outIO->SetOutTreeVars("numdata", numdata, "mdim");
    outIO->SetOutTreeVars("nummc", nummc, "mdim");
    
    outIO->SetOutTreeVars("ratios", ratios, "mdim");
    outIO->SetOutTreeVars("ratiom", ratiom, "mdim");
    
    outIO->SetOutTreeVars("kdim", kdim, "mdim");
    
    long tent = interface->GetTreeEntries(outIO->GetElement());
    
    Printf("this is the number of entries %d\n",tent);
    Printf("this is the number of events %d\n",nevents);
    
    long cnum=0;
    
    
    while (cnum<nevents) {
        
        
        if (tent==cnum+beginenum){
            Printf("this is the number of entries %d\n",tent);
            Printf("this is the number of events %d\n",cnum);
            
            outIO->Write();

            beginenum-=cnum;
            currentelem=interface->GetNextElem(-4, outIO->GetElement());
            
            CreateNewTree(currentelem);
            
            outIO->SetOutTreeVar("mdim", &mdim);
            outIO->SetOutTreeVar("event", &elemnum);
            outIO->SetOutTreeVars("numsig", numsig, "mdim");
            outIO->SetOutTreeVars("numdata", numdata, "mdim");
            outIO->SetOutTreeVars("nummc", nummc, "mdim");
    
            outIO->SetOutTreeVars("ratios", ratios, "mdim");
            outIO->SetOutTreeVars("ratiom", ratiom, "mdim");
    
            outIO->SetOutTreeVars("kdim", kdim, "mdim");
            
            // I need to find another approach to get this..
            tent = interface->GetTreeEntries(outIO->GetElement());

            Printf("this is the number of entries %d",tent);
            
        }
        
        
        for (int k=0; k<mdim; k++) {
            std::vector<int> varj;
            kdim[k]=k;
            varj.push_back(k);
            for (int j=0; j<ndim; j++) {
                varj.push_back(output_data[cnum*ndim*mdim+k*ndim+j]);
        
            }
            
            if (cubemap.find(varj)==cubemap.end()){

                // how to say the values are uninteresting?
                
                
                ratios[k]=-1;
                ratiom[k]=-1;
                
                
                nummc[k]=-1;
                numdata[k]=-1;
                numsig[k]=-1;
            
            
            } else {
                cubeit=cubemap.find(varj);

            
                ratios[k]=((*cubeit).second).x[0];
                ratiom[k]=((*cubeit).second).x[1];

                numsig[k]=((*cubeit).second).x[2];
                numdata[k]=((*cubeit).second).x[3];
                nummc[k]=((*cubeit).second).x[4];

            }
            
        }
        elemnum=cnum;
        
        // this should work, hides some of the details
        outIO->Fill();
        
        cnum++;
    }
    

    // dangerous
    //outtree->Write("",TObject::kOverwrite);
    outIO->Write();
    
    
    beginenum+=cnum;
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");
    beginelem=currentelem;
    
    return 0;
    
}

// bad name, this should be
// GetTree or something
int Eval::CreateNewTree(int celem){
    
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