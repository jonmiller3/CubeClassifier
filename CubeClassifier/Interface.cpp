//
//  Interface.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>
#include "Interface.h"
#include <string>
#include <fstream>
#include <sstream>

#include <vector>

Interface::Interface(int argc, const char* argv[]){
        
    const char* modestr = argv[1];
    if (strcmp(modestr,"Eval")){
        mode = 1;
    } else {
        mode = 0;
    }
    filelist = argv[2];
    varlist = argv[3];
    setlist = argv[4];
    
    ReadFileList();
    ReadSettingsList();
    ReadVariableList();
    
}

void Interface::ReadFileList(){
    
    std::ifstream myfile;
    myfile.open(filelist.c_str());


    
    while (!myfile.eof()){
        
        int sstype;
        std::string ssname;
        
        std::string line;
        
        std::stringstream ss ( std::stringstream::in | std::stringstream::out );

        getline(myfile, line);
        
        ss<<line;
        
        ss>>ssname;
        ss>>sstype;
        
        namelist.push_back(ssname);
        typelist.push_back(sstype);
        
    }
    myfile.close();
    
    return;
    
}

void Interface::ReadVariableList(){
    
    std::ifstream myfile;
    myfile.open(varlist.c_str());
    
    while (!myfile.eof()){
        
        double ssmax;
        double ssmin;
        std::string ssname;
        
        std::string line;
        
        std::stringstream ss ( std::stringstream::in | std::stringstream::out );
        
        getline(myfile, line);
        
        ss<<line;
        
        ss>>ssname;
        ss>>ssmax;
        ss>>ssmin;
        
        varnamelist.push_back(ssname);
        varmaxlist.push_back(ssmax);
        varminlist.push_back(ssmin);
        
    }
    
    myfile.close();
    return;
    
}

void Interface::ReadSettingsList(){
    
    std::ifstream myfile;
    myfile.open(setlist.c_str());
    
    std::string line;
    std::stringstream ss ( std::stringstream::in | std::stringstream::out );
        
    getline(myfile, line);        
    ss<<line;
    
    ss>>prunestat;
    ss>>prunesyst;
        
    getline(myfile, line);        
    ss<<line;   
    
    ss>>cubesetting;
    ss>>cubelevel;
    
    getline(myfile, line);        
    ss<<line;   
    
    ss>>includeweighting;
    
    myfile.close();
    
}
