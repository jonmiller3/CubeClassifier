//
//  Eval.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include "Eval.h"
#include <iostream>


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
    float numc[NUMCLASS];

    float ratio[NUMCLASS-1];
    
    //float ratios;
    //float ratiom;
    float cubedepth;
    
    for (int j=0; j<NUMCLASS; j++) {
        controlIO->SetTreeVar(Form("num%d",j), &numc[j]);
        if (j==0) continue;
        controlIO->SetTreeVar(Form("ratio%d",j), &ratio[j-1]);
    }

    
    
    //controlIO->SetTreeVar("nummc", &nummc);
    //controlIO->SetTreeVar("numdata", &numdata);
    
    //controlIO->SetTreeVar("ratios", &ratios);
    //controlIO->SetTreeVar("ratiom", &ratiom);
    controlIO->SetTreeVar("cubedepth", &cubedepth);
    
    
    //float_q nullfloat5= {{0,0,0,0,0}};
    int tinfo=1;
    i=0;
    while (tinfo>0) {
        tinfo = controlIO->GetEntry(i);
        if (tinfo<1) continue; // not sure if I need this
        
        // let's put in my cuts
        if (NUMCLASS>2) {
            if ((interface->GetPruneSyst()!=0)&&(interface->GetPruneStat()<abs(0.5-ratio[1]))) continue;
        } else {
            if ((interface->GetPruneSyst()!=0)) continue;

        }
        // this should probably be that both need to be above...
        if (interface->GetPruneStat()>numc[0]) continue;
        
        std::vector<int> varj;
        
        (cubedepth)+=0.01; // not sure if this is necessary..
        varj.push_back((int)(cubedepth));
        
        for (int j=0; j<ndim; j++) {
            var[j]+=0.01; // not sure if this is necessary..
            varj.push_back((int)var[j]);
        }
        
        float_q cmval;
        
        for (int j=0; j<NUMCLASS-1; j++) {
            cmval.x[j]=ratio[j];
            //std::cout<<" this is the associated ratio "<<cmval.x[j]<<std::endl;
        }
        
        //cmval.x[0]=ratios;
        //cmval.x[1]=ratiom;
        
        for (int j=0; j<NUMCLASS; j++) {
            cmval.x[j+NUMCLASS-1]=numc[j];
            //std::cout<<" this is the associated numbers "<<cmval.x[j+NUMCLASS-1]<<std::endl;
        }
        //cmval.x[2]=numsig;
        //cmval.x[3]=numdata;
        //cmval.x[4]=nummc;
        
        
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
    
    
    data = new float_d[nevents];
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
        
        data[cnum].x[1]=weight;
        data[cnum].x[0]=(float)currentIO->GetType();
                
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
        
        // path is relative to home
        char* pPath;
        pPath = getenv ("HOME");
        std::string basename = Form("%s/",pPath);
        
        
        std::string filename=*it;
        
        filename = basename + filename;

        
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

    
    //float ratios[mdim];
    //float ratiom[mdim];
    //float numsig[mdim];
    //float numdata[mdim];
    //float nummc[mdim];
    int   kdim[mdim];
    int   elemnum;
    
    outIO->SetOutTreeVar("mdim", &mdim);
    outIO->SetOutTreeVar("event", &elemnum);
    
    float_qc cvars[mdim];
    float_qr rvars[mdim];
    
    
    for (int l=0; l<mdim; l++) {
        
        for (int m=0; m<NUMCLASS; m++) {
            outIO->SetOutTreeVar(Form("cvars%d_%d",l,m), &cvars[l].x[m]);
            if (m==NUMCLASS-1) continue;
            outIO->SetOutTreeVar(Form("rvars%d_%d",l,m), &rvars[l].x[m]);
        }
    }

    

    
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
            
            for (int l=0; l<mdim; l++) {
                
                for (int m=0; m<NUMCLASS; m++) {
                    outIO->SetOutTreeVar(Form("cvars%d_%d",l,m), &cvars[l].x[m]);
                    if (m==NUMCLASS-1) continue;
                    outIO->SetOutTreeVar(Form("rvars%d_%d",l,m), &rvars[l].x[m]);
                }
            }
            
            

    
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
                
                
                for (int l=0; l<NUMCLASS; l++) {
                    
                    cvars[k].x[l]=-1;
                    if (l==NUMCLASS-1) continue;
                    rvars[k].x[l]=-1;
                    
                }
                //ratios[k]=-1;
                //ratiom[k]=-1;
                
                
                //nummc[k]=-1;
                //numdata[k]=-1;
                //numsig[k]=-1;
            
            
            } else {
                cubeit=cubemap.find(varj);

                for (int l=0; l<2*NUMCLASS-1; l++) {
                    
                    //std::cout<<" what is this "<<((*cubeit).second).x[l]<<" and "<<l<<std::endl;
                    // there is probably a much faster way to do this
                    if (l>NUMCLASS-2) {
                        cvars[k].x[l-NUMCLASS+1]=((*cubeit).second).x[l];
                        //std::cout<<" doing cvars "<<std::endl;
                    } else {
                        rvars[k].x[l]=((*cubeit).second).x[l];
                        //std::cout<<" doing rvars "<<std::endl;
                    }
                }
                
                
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
    
    
    
    // I get the celem and use it
    
    outIO = new IO<TTree, TFile>(interface->GetOutFileName(celem),
                                 "rtree",celem,
                                 interface->GetType(celem),1);
    
    outIO->AddFriend(interface->GetTreeName(celem),interface->GetFileName(celem));
    
    return 0;
    
}