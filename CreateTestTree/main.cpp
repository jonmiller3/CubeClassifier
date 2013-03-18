//
//  main.cpp
//  CreateTestTree
//
//  Created by Jonathan Miller on 3/14/13.
//  Copyright (c) 2013 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "CreateTestTree.h"

int main (int argc, const char * argv[])
{

    CreateTestTree* signal = new CreateTestTree("DefaultSignal",5);
    signal->AddVariables(5);
    signal->Loop(10000, true);
    signal->WriteROOTFile();
    
    CreateTestTree* background = new CreateTestTree("DefaultBackground",5);
    background->AddVariables(5);
    background->Loop(100000, false);
    background->WriteROOTFile(); 
    
    return 0;
    
}

