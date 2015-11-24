//
//  Base.hpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 11/23/15.
//  Copyright © 2015 UTFSM. All rights reserved.
//

#ifndef Base_hpp
#define Base_hpp

#include <stdio.h>

// this is part of a refractoring
// which is incomplete

#include "IO.h"
#include "Interface.h"


#include <vector>
#include <map>

class Base {
protected:
    float* var;
    int nvars;
    int ndim;
    int cubelevel;
    
    IO<TTree, TFile>* currentIO;
    Interface* interface;
    
    
public:
    Base(Interface*);
    long InputData(long);
    int GetNewTree(int);
    //int SetMaxMin();
    virtual int SetData(int,int,int);
    
};



#endif /* Base_hpp */
