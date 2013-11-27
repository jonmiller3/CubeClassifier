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

    CreateTestTree* signal = new CreateTestTree("DefaultSignal");
    signal->AddVariables();
    signal->Loop(100000, 150);
    signal->WriteROOTFile();
    
    std::cout<<" done with Signal! "<<std::endl;
    
    CreateTestTree* background = new CreateTestTree("DefaultBackground");
    background->AddVariables();
    background->Loop(100000, 100);
    background->WriteROOTFile(); 
    
    std::cout<<" done with Background! "<<std::endl;
    
    CreateTestTree* background2 = new CreateTestTree("DefaultBackground2");
    background2->AddVariables();
    background2->Loop(100000, 200);
    background2->WriteROOTFile();
    
    std::cout<<" done with Background! "<<std::endl;
    
    CreateTestTree* test = new CreateTestTree("DefaultTest");
    test->AddVariables();
    test->Loop(100, 150);
    test->Loop(100000, 100);
    test->Loop(100000, 200);
    test->WriteROOTFile();
    
    std::cout<<" done with Test! "<<std::endl;
    
    return 0;
    
}

