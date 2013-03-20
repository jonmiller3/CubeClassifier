//
//  main.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#ifdef __APPLE__
    #include <OpenCL/OpenCL.h>
#else
    #include <stdio.h>
    #include <string.h>
    #include "CL/cl.h"
    #define FALSE false
    #define TRUE true
#endif

#include "Interface.h"
#include "BaseClassifier.h"
#include "Classify.h"
#include "Eval.h"

#include <map>
#include <vector>
#include <string>

// this is just to test
int main (int argc, const char * argv[])
{
    
    if (argc<2) {
     
        printf("I don't have any arguments! %d",argc);
        
        Interface* iface = new Interface(argc,argv);

        // just create the class
        Classify* bs = new Classify(iface);

        bs->StartQueue();
    
        printf("I have started the queue");
    
        bs->ProcessQueue();
    
        printf("I have processed the queue");
    
        bs->WriteOutput();
        
        delete bs;
        
        delete iface;
        
    }

    return 0;

}


