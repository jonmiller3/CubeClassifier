//
//  main.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//  Copyright (c) 2013 Universidad Tecnico Frederico Santa Maria. All rights reserved.
//

#include <iostream>

//#define USECPU

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
#include "PreProcess.hpp"

#include <map>
#include <vector>
#include <string>

// this is just to test
int main (int argc, const char * argv[])
{
    
    if (argc<2) {
     
        printf("I don't have any arguments! %d",argc);
        
        Interface* iface = new Interface(argc,argv);

        
        PreProcess* pp = new PreProcess(iface);
        pp->Process(1000);
        pp->WriteOutput();
        
        // just create the class
        Classify* bs = new Classify(iface);

        bs->StartQueue();
    
        printf("I have started the queue");
    
        bs->ProcessQueue();
    
        printf("I have processed the queue");
    
        bs->WriteOutput();
        
        delete bs;
        
        // this is just for the classify test
        //delete iface;
        
        printf("now begin test of eval");
        
        Eval* ebs = new Eval(iface);
        
        ebs->Process();
        
        printf("I have processed the eval queue");
        
        
    }

    return 0;

}


