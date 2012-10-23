//
//  Data.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "Data.h"

Data::Data(double* input, double* max_range, double* min_range){
    
    // this might be an improper way to use it
    numdim = sizeof(max_range);   
    values = new double[numdim];
    
    for (int i=0; i<numdim; i++) {
        double trange = max_range[i]-min_range[i];
        values[i] = (input[i]-min_range[i])/trange;
    }
    
    return;
    
}

double* Data::getValues(){
    
    return values;
    
}
