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
    ndim=(interface->GetVarNameList()).size();
    
    var = new float[ndim];
    

    return;
}