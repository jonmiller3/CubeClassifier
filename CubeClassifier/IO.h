//
//  IO.h
//  CubeClassifier
//
//  Created by Jonathan Miller on 8/19/13.
//  Copyright (c) 2013 UTFSM. All rights reserved.
//

#ifndef __CubeClassifier__IO__
#define __CubeClassifier__IO__

#include <iostream>

#include <string>
#include <stdio>


#include "TTree.h"
#include "TFile.h"

// I haven't throught through all the template
// yet, because I don't know how the other
// container types might work
template <class T, class F>
class IO {
    
    T* tree;
    F* file;
    int element;
    int type;
    
public:
    
    IO(std::string filename, std::string treename,int elem, int typ){
        element=elem;
        type=typ;
        return;
    }
    
    ~IO(){;}
    
    int GetEntry(long i){return tree->GetEntry(i);}
    
    template <class C>
    int SetTreeVar(std::string name,C variable){
        return 0;
    }
    
    int GetEntries(){return 0;}
    
    int GetElement(){return element;}
    
    int GetType(){return type;}
    
    int AddFriend(std::string tname, std::string fname){
        
        return 0;
        
    }
};



template <>
class IO <TTree, TFile> {
    
    TTree* tree;
    TFile* file;
    
    int element;
    int type;
    
public:
  
    // I am giving all the funcationality
    // I should be able to do the same thing with
    // some other 'general' IO hopefully
    
    // do I need two constructors, one which 'recreates'?
    IO(std::string filename, std::string treename, int elem, int typ){
        file= new TFile(filename.c_str());
        tree=(TTree*)gDirectory->Get(treename.c_str());
        element=elem;
        type=typ;
        return;
    }
    
    ~IO(){;}
    
    int GetEntry(long i){return tree->GetEntry(i);}

    template <class T>
    int SetTreeVar(std::string name,T variable){
        tree->SetBranchAddress(name.c_str(),&variable);
        return 0;
    }
    int GetEntries(){return tree->GetEntries();}
    
    int GetElement(){return element;}
    
    int GetType(){return type;}
    
    int AddFriend(std::string tname, std::string fname){
        
        tree->AddFriend(tname,fname);
        return 0;
        
    }
};



#endif /* defined(__CubeClassifier__IO__) */
