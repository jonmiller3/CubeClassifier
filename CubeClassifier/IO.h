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


#include "TTree.h"
#include "TFile.h"


template<typename T> struct type_name
{
    static const char* name() { return 0;}
};

template<> struct type_name<int> { static const char* name() {return "I";} };
template<> struct type_name<float> { static const char* name() {return "F";} };


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
    
    float check;
    
public:
  
    // I am giving all the funcationality
    // I should be able to do the same thing with
    // some other 'general' IO hopefully
    
    // do I need two constructors, one which 'recreates'?
    IO(std::string filename, std::string treename, int elem, int typ, int iread){
        if (iread==0){
            file= new TFile(filename.c_str());
        } else {
            file= new TFile(filename.c_str(),"RECREATE");
        }
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
    
    template <class T>
    int SetTreeVars(std::string name,T* variable){
        tree->SetBranchAddress(name.c_str(),variable);
        return 0;
    }

    template <class T>
    int SetOutTreeVar(std::string name,T variable){
        tree->Branch(name.c_str(),&variable,Form("%s/%s",name.c_str(),type_name<typeof(variable)>::name()));
        return 0;
    }
    
    template <class T>
    int SetOutTreeVars(std::string name,T* variable,std::string name2){
        tree->Branch(name.c_str(),&variable,Form("%s[%s]/%s",name.c_str(),
                                                 name2.c_str(),type_name<typeof(variable[0])>::name()));
        return 0;
    }
    
    int GetEntries(){return tree->GetEntries();}
    
    int GetElement(){return element;}
    
    int GetType(){return type;}
    
    int Write(){
        tree->Write();
        return 0;
    }
    int Fill(){
        tree->Fill();
        return 0;
    }
    
    int AddFriend(std::string tname, std::string fname){
        
        tree->AddFriend(tname.c_str(),fname.c_str());
        return 0;
        
    }
};



#endif /* defined(__CubeClassifier__IO__) */
