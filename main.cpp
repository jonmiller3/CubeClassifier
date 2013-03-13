//
//  main.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>
#include <OpenCL/OpenCL.h>

// these are the old ones
//#include "fillcube.cl.h"
//#include "fillcube2.cl.h"
//#include "fillcube3.cl.h"
//#include "normdata.cl.h"
//#include "calccuberesult.cl.h"

// new one but not necessary now
//#include "Interface.h"

#include "BaseClassifier.h"

//#include "oclUtils.h"

// not using right now
//#include "boost/lexical_cast.hpp"
//#include "boost/tuple/tuple.hpp"
//#include "boost/tuple/tuple_comparison.hpp"
//#include "boost/tuple/tuple_io.hpp"


#include <map>
#include <vector>
#include <string>

// this is just to test
int main (int argc, const char * argv[])
{

    // just create the class
    BaseClassifier* bs = new BaseClassifier(4,4);

    bs->StartQueue();
    
    printf("I have started the queue");
    
    bs->ProcessQueue();
    
    printf("I have processed the queue");
    
    delete bs;
    

    return 0;

}


