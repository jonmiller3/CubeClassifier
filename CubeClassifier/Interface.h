//
//  Interface.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#ifndef CubeClassifier_Interface_h
#define CubeClassifier_Interface_h

#include <iostream>
#include <vector>

struct filelist_struct {
    
    std::string name;
    std::string tree;
    int type;
    
};

struct varlist_struct {
    std::string varname;
    double max;
    double min;
    
};


class Interface {
  
    int mode;
    std::string filelist;
    std::string varlist;
    std::string setlist;
   
    // files
    std::vector<std::string> namelist;
    std::vector<std::string> treelist;
    std::vector<int> typelist;
    
    // variables
    std::vector<std::string> varnamelist;
    std::vector<double> varmaxlist;
    std::vector<double> varminlist;

    // settings
    double prunestat;
    double prunesyst;
    int cubelevel;
    int cubesetting;
    int includeweighting;
    
    // new way
    std::vector<filelist_struct> filelistvec;
    std::vector<varlist_struct> varlistvec;
    
private:
    void ReadFileList();
    void ReadSettingsList();
    void ReadVariableList();
    
public:
    Interface(int,const char*[]);
    Interface(){
        mode=0;
        prunestat=0;
        prunesyst=0;
        cubelevel=1;
        cubesetting=1;
        includeweighting=0;
    }
    
    int GetMode(){return mode;}
    
    double GetPruneStat(){return prunestat;}
    double GetPruneSyst(){return prunesyst;}
    int GetCubeLevel(){return cubelevel;}
    int GetCubeSetting(){return cubesetting;}
    int GetWeightSetting(){return includeweighting;}
    
    // these are sort of the old way of doing things
    std::vector<std::string> GetTreeList(){return treelist;}
    std::vector<std::string> GetNameList(){return namelist;}
    std::vector<int> GetTypeList(){return typelist;}
    
    std::vector<std::string> GetVarNameList(){return varnamelist;}
    std::vector<double> GetMaxList(){return varmaxlist;}
    std::vector<double> GetMinList(){return varminlist;}
    
    // new way
    std::string GetTreeName(int elem){return filelistvec[elem].tree;}
    std::string GetFileName(int elem){return filelistvec[elem].name;}
    int GetType(int elem){return filelistvec[elem].type;}
    
    std::string GetVarName(int elem){return varlistvec[elem].varname;}
    double GetMax(int elem){return varlistvec[elem].max;}
    double GetMin(int elem){return varlistvec[elem].min;}
    
    
};


#endif

// this is sort of the minimal amount of stuff I can do