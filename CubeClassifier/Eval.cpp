//
//  Eval.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include "Eval.h"
#include <iostream>

// very simple, wish there was some nicer way to do this but it goes quickly at least?
int Eval::SetMaxMin(float* max, float* min){
    
    int ls=interface->GetVarListSize();
    
    for (int i=0; i<ls; i++) {
        
        max[i]=interface->GetMax(i);
        min[i]=interface->GetMin(i);
        
    }
        
    return 0;
    
}


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

TTree* Eval::SelectTree(int stype){
    
    TTree* ctree;
    bool fflag=false;
    
    // first find the tree
    std::vector<int> typelist=interface->GetTypeList();
    
    std::vector<std::string> namelist=interface->GetNameList();
    std::vector<std::string> treelist=interface->GetTreeList();
    std::vector<int>::iterator yt=typelist.begin();
    std::vector<std::string>::iterator tt=treelist.begin();
    for (std::vector<std::string>::iterator it=namelist.begin(); it!=namelist.end(); ++it,++tt,++yt){
        
        if (fflag) continue;
        
        std::string filename=*it;
        TFile* tfile= new TFile(filename.c_str());
        std::string treename=*tt;
        TTree* ttree=(TTree*)gDirectory->Get(treename.c_str());
        int typet = *yt;
        
        // -1 will be classify type
        if (typet==stype){
            
            ctree=ttree;
            fflag=true;
            
        }
        
    }    
    
    return ctree;
    
    
}


// this loads from the interface
int Eval::LoadCubeMap(){

    // I am assuming that -1 is the type of Classify Control Root Files
    int conelem = interface->GetNextElem(-1, -1);
    
    IO<TTree, TFile>* controlIO = new IO<TTree, TFile>(interface->GetTreeName(conelem),
                                                       interface->GetFileName(conelem),conelem,
                                                       interface->GetType(conelem));
    
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        controlIO->SetTreeVar(varname.c_str(), <float>(var[i]));
        //ctree->SetBranchAddress(varname.c_str(), &(var[i]));
        
    }    
    
    float numsig;
    float numdata;
    float nummc;
    float ratios;
    float ratiom;
    float cubedepth;
    
    controlIO->SetTreeVar("numsig", <float>numsig);
    controlIO->SetTreeVar("nummc", <float>nummc);
    controlIO->SetTreeVar("numdata", <float>numdata);
    
    controlIO->SetTreeVar("ratios", <float>ratios);
    controlIO->SetTreeVar("ratiom", <float>ratiom);
    controlIO->SetTreeVar("cubedepth", <float>cubedepth);
    
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
        
        cubedepth+=0.01; // not sure if this is necessary..
        varj.push_back((int)cubedepth);
        
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
int Eval::GetNewTree(int ctype){
    
    // I don't think this will work because
    // I destroy currentIO to know that I should
    // create a new tree
    int celem=0;
    if (currentIO=0){
        celem = -1;
    } else {
        int celem = currentIO->GetElement();
    }
    
    
    int newelem = interface->GetNextElem(ctype, celem);
    
    currentIO = new IO<TTree, TFile>(interface->GetTreeName(newelem),
                                     interface->GetFileName(newelem),newelem,
                                     interface->GetType(newelem));
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        currentIO->SetTreeVar(varname.c_str(), <float>(var[i]));

        //currentttree->SetBranchAddress(varname.c_str(), &(var[i]));
        
    }    
    
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
    
    beginelem=currentelem;
    beginenum=enumber;
    
    if (currenttfile==0) GetNewTree(4);
    
    weight=1.0/((float)currentttree->GetEntries());
    
    // let's assume it is root for now
    // this is maybe not necessary?
    
    
    data = new float[nevents*2];    
    long cnum=0;
    
    while (cnum<nevents) {
        
        
        int tinfo = currentttree->GetEntry(cnum+enumber);
        
        if (tinfo==0) currentIO=0;
        
        if (currentIO==0){
            enumber=-cnum;
            GetNewTree(4);
            
            weight=1.0/((float)currentIO->GetEntries());
            
            tinfo = currentIO->GetEntry(cnum+enumber);
        }
        if (tinfo==0) currentIO=0;
        
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
        Printf(" input order %s",treename.c_str());
        TTree* ttree=(TTree*)gDirectory->Get(treename.c_str());
        int typet = *yt;
        
        if (typet!=4) continue;
        
        
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
    if (outIO==0) CreateNewTree(4,beginelem);
    // this whole bit is still kludgy
    
    // particular way of doing it
    //outfile = new TFile("/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/results.root","RECREATE");
    //outtree = new TTree("rtree", "Result of Evalulation");
    //outtree->AddFriend("treeDefaultTest","/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/DefaultTest.root");
    
    // this is just to test!
    
    // this is a bit dangerous
    float ratios[mdim];
    float ratiom[mdim];
    float numsig[mdim];
    float numdata[mdim];
    float nummc[mdim];
    int   kdim[mdim];
    int   elemnum;
    
    TBranch *mdimb   = outtree->Branch("mdim",&mdim,"mdim/I");
    
    TBranch *numsigb = outtree->Branch("numsig",numsig,"numsig[mdim]/F");
    TBranch *numdatab = outtree->Branch("numdata",numdata,"numdata[mdim]/F");
    TBranch *nummcb = outtree->Branch("nummc",nummc,"nummc[mdim]/F");
    
    TBranch *ratiosb = outtree->Branch("ratios",ratios,"ratios[mdim]/F");
    TBranch *ratiosm = outtree->Branch("ratiom",ratiom,"ratiom[mdim]/F");
    
    TBranch *dimb = outtree->Branch("kdim", kdim, "Kdim[mdim]/I");
    
    TBranch *elemb   = outtree->Branch("event",&elemnum,"event/I");
    
    // I need some other approach to figure out when to 'stop'
    // I have begin elem?
    long tent = currentttree->GetEntries();
    
    Printf("this is the number of entries %d",tent);
    Printf("this is the number of events %d",nevents);
    Printf("this is the name %s",currentttree->GetName());
    
    long cnum=0;
    
    
    while (cnum<nevents) {
        
        
        if (tent==cnum+beginenum){
            Printf("this is the number of entries %d",tent);
            Printf("this is the number of events %d",cnum);
            
            outfile->Write();

            beginenum-=cnum;
            beginelem++;
            // dangerous
            //currentttree->Write("",TObject::kOverwrite);
            
            CreateNewTree(4,beginelem);

            mdimb   = outtree->Branch("mdim",&mdim,"mdim/I");
            
            numsigb = outtree->Branch("numsig",numsig,"numsig[mdim]/F");
            numdatab = outtree->Branch("numdata",numdata,"numdata[mdim]/F");
            nummcb = outtree->Branch("nummc",nummc,"nummc[mdim]/F");
            
            ratiosb = outtree->Branch("ratios",ratios,"ratios[mdim]/F");
            ratiosm = outtree->Branch("ratiom",ratiom,"ratiom[mdim]/F");
            

            dimb = outtree->Branch("kdim", kdim, "Kdim[mdim]/I");
    
            elemb   = outtree->Branch("event",&elemnum,"event/I");
            
            // I need to find another approach to get this..
            tent = currentttree->GetEntries();

            Printf("this is the name %s",currentttree->GetName());
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
        outtree->Fill();
        
        cnum++;
    }
    

    // dangerous
    //outtree->Write("",TObject::kOverwrite);
    outfile->Write();
    
    
    beginenum+=cnum;
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");

    
    return 0;
    
}

// bad name, this should be
// GetTree or something
int Eval::CreateNewTree(int ctype, int celem){
    
    if (interface->GetType(celem)!=ctype){
        celem++;
        CreateNewTree(ctype, celem);
    }

    outfile = new TFile((interface->GetOutFileName(celem)).c_str(),"RECREATE");
    outtree = new TTree("rtree","Result of Evalulation");
    outtree->AddFriend((interface->GetTreeName(celem)).c_str(),(interface->GetFileName(celem)).c_str());
    
    // I should have a better way of setting the current tree
    GetNewTree(4, celem);
    
    return 0;
    
    
    int newelem = interface->GetNextElem(ctype, celem);
    
    outIO = new IO<TTree, TFile>("rtree",
                                 interface->GetOutFileName(newelem),newelem,
                                 interface->GetType(newelem));
    
    outIO->AddFriend(interface->GetTreeName(newelem),interface->GetFileName(newelem));
    
    // OK, I need to set up this
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        currentIO->SetTreeVar(varname.c_str(), <float>(var[i]));
        
        //currentttree->SetBranchAddress(varname.c_str(), &(var[i]));
        
    }
    
    return 0;

    
    
    
}