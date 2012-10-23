//
//  Cube.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "Cube.h"

int* Cube::getCounts(){
    
    int val[] = {countsA,countsB,countsC};
    
    return val;
}

double* Cube::getResult(){
    
    double val[] = {fraction,staterr,systerr};
    
    return val;
}


Cube::Cube(double* in, int set){
    
    identity=in;
    setting=set;
    
    return;
    
}
