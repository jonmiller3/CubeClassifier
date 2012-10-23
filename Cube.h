//
//  Cube.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_Cube_h
#define CubeClassifier_Cube_h

class Cube {    
    double fraction;
    double staterr;
    double systerr;
    
    // A is data, B is background MC, C is signal MC
    int countsA;
    int countsB;
    int countsC;
    
    double* identity;
    int setting;
    
public:
    Cube(double*, int);
    int* getCounts(void);
    double* getResult(void);
};


#endif

// this will store the counts it gets when the proper return value is received to 
// tell it to store a count of the appropriate type

// this will store the fraction and errors when this value is received

// this will get counts

// this will get fraction, staterr, systerr

// if I make all my cubes I Will run into memory problems, so
// I just make the ones that I put events in, there should be no way to ever 
// make more than 2million or so that way

// I think it needs some sort of multimap between the container and the elements....
// http://www.cplusplus.com/reference/stl/multimap/