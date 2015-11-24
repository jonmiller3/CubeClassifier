//
//  Base.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 11/23/15.
//  Copyright © 2015 UTFSM. All rights reserved.
//

#include "Base.hpp"

Base::Base(Interface* inputinterface){
    
    
    interface=inputinterface;
    
    ndim=(interface->GetVarNumber());
    nvars=(interface->GetVarListSize());
    
    cubelevel=interface->GetCubeLevel();
    
    currentIO=0;
    
    var = new float[nvars];
    
    return;
    
}

int Base::GetNewTree(int newelem){
    
    currentIO = new IO<TTree, TFile>(interface->GetFileName(newelem),
                                     interface->GetTreeName(newelem),newelem,
                                     interface->GetType(newelem),0);
    for (int i=0; i<nvars; i++) {
        std::string varname=interface->GetVarName(i);
        currentIO->SetTreeVars(varname.c_str(), &(var[i]));
    }
    
    interface->SetTreeEntries(newelem, currentIO->GetEntries());
    
    return 0;
}


long Base::InputData(long nevents){
    
    // for now this is just using default, later I will need to set it here
    float weight=1;
    int currentelem = interface->GetNextElem(0, -1);
    
    GetNewTree(currentelem);
    
    weight=1.0/((float)currentIO->GetEntries());
    
    long cnum=0;
    
    while (cnum<nevents) {
        
        int tinfo = currentIO->GetEntry(cnum);
        
        if (tinfo==0){
            currentelem = interface->GetNextElem(0, currentelem);
            GetNewTree(currentelem);
            
            weight=1.0/((float)currentIO->GetEntries());
            
            tinfo = currentIO->GetEntry(cnum);
        }
        
        int j=0;
        for (int i=0; i<nvars; i++) {
            int par=interface->GetVarParameter(i);
            
            SetData(par,i,j);
            
            j++;
        }
        
        
        cnum++;
    }
    
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");
    
    return 0;
    
}

// is a dummy
int Base::SetData(int par, int i, int j){
    
    return 0;
}

