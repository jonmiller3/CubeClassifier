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
        
    if (argc>1){
    
        const char* modestr = argv[1];
        if (strcmp(modestr,"Eval")){
            mode = 1;
        } else {
            mode = 0;
        }
        filelist = argv[2];
        varlist = argv[3];
        setlist = argv[4];
    } else {
        
        mode=0;
        filelist="/Users/jonathanmiller/Desktop/CubeClassifier/DefaultFileList.txt";
        varlist="/Users/jonathanmiller/Desktop/CubeClassifier/DefaultVarList.txt";
        setlist="/Users/jonathanmiller/Desktop/CubeClassifier/DefaultSettings.txt";

    }

        
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
        std::string sstree;
        
        std::string line;
        
        std::stringstream ss ( std::stringstream::in | std::stringstream::out );

        getline(myfile, line);
        
        ss<<line;
        
        ss>>ssname;
        ss>>sstype;
        ss>>sstree;
        
        namelist.push_back(ssname);
        treelist.push_back(sstree);
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
        ss>>ssmin;
        ss>>ssmax;
        
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
    
    {
        std::string line;
        std::stringstream ss ( std::stringstream::in | std::stringstream::out );
        
        getline(myfile, line);        
        ss.str(line);
    
        ss>>prunestat;
        ss>>prunesyst;
        
    }
    {

        std::string line;
        std::stringstream ss ( std::stringstream::in | std::stringstream::out );
    
        getline(myfile, line);        
        ss.str(line);   
    
        ss>>cubesetting;
        ss>>cubelevel;
    
    }
    {

        std::string line;
        std::stringstream ss ( std::stringstream::in | std::stringstream::out );
    
        getline(myfile, line);        
        ss.str(line);   
    
        ss>>includeweighting;
    
            
    }
    myfile.close();
    
}
