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
        
        std::string pPath = getenv ("HOME");
        std::string basename = pPath+"/Dropbox/CubeClassifier";

        
        filelist=basename+"/DefaultFileList.txt";
        varlist=basename+"/DefaultVarList.txt";
        setlist=basename+"/DefaultSettings.txt";

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
        
        // old way
        namelist.push_back(ssname);
        treelist.push_back(sstree);
        typelist.push_back(sstype);
        
        // new way
        filelist_struct fl_struct;
        fl_struct.type=sstype;
        fl_struct.name=ssname;
        fl_struct.tree=sstree;
        
        // I am not sure whether I want to put root in the interface
        // maybe it would be nice to handle all root calls here
        // so you just program new interface to handle other types of
        // files/modes/etc
        fl_struct.entries=0;
        
        filelistvec.push_back(fl_struct);
        
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
        
        // old way
        varnamelist.push_back(ssname);
        varmaxlist.push_back(ssmax);
        varminlist.push_back(ssmin);
        
        // new way
        varlist_struct vl_struct;
        vl_struct.max=ssmax;
        vl_struct.min=ssmin;
        vl_struct.varname=ssname;
                
        varlistvec.push_back(vl_struct);
        
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
