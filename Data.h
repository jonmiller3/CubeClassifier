//
//  Data.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_Data_h
#define CubeClassifier_Data_h

class Data {
    double* values;
    int numdim;
    
public:
    Data(double*,double*,double*);
    double* getValues();
};

#endif

// I need to give thought to the weight (from simulation)
// and the scaling (normalize number of events)