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

#include "Interface.h"


#define NUMCLASS 2

struct float_q {
    float x[2*NUMCLASS-1];
};

struct float_qc {
    float x[NUMCLASS];
};

struct float_qr {
    float x[NUMCLASS-1];
};


struct float_triple {
    float x[3];
};

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
    
    long GetEntries(){return 0;}
    
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
        
        // I am going to make it relative to $HOME
        // this can be fixed later
        
        char* pPath;
        pPath = getenv ("HOME");
        std::string basename = Form("%s/",pPath);
        filename = basename + filename;
        
        if (iread==0){
            file= new TFile(filename.c_str());
            tree=(TTree*)gDirectory->Get(treename.c_str());
            tree->Show(5);
        } else {
            file= new TFile(filename.c_str(),"RECREATE");
            tree=new TTree(treename.c_str(),treename.c_str());
        }
        printf("constructor: name is %s with tree %s\n",filename.c_str(),treename.c_str());
        element=elem;
        type=typ;
        return;
    }

    ~IO(){file->Close();}
    
    int GetEntry(long i){
        int entry = tree->GetEntry(i);
        //tree->Show();
        return entry;
    }

    template <class T>
    int SetTreeVar(std::string name,T variable){
        tree->SetBranchAddress(name.c_str(),variable);
        printf("set up var %s \n",name.c_str());
        tree->GetEntry(5);
        printf("test %f \n",variable);
        return 0;
    }

    template <class T>
    int SetTreeVarTest(std::string name,T variable){
        return 0;
    }
    
    template <class T>
    int SetTreeVarCheck(std::string name, T variable){
        tree->SetBranchAddress(name.c_str(),variable);
        check=variable;
        //printf("set up var %s \n",name.c_str());
        return 0;
    }
    
    template <class T>
    int SetTreeVars(std::string name,T variable){
        tree->SetBranchAddress(name.c_str(),variable);
        return 0;
    }

    template <class T>
    int SetOutTreeVar(std::string name,T* variable){
        std::string bname = Form("%s/%s",name.c_str(),type_name<typeof(*variable)>::name());
        tree->Branch(name.c_str(),variable,bname.c_str());
        printf("set up var %s with branch %s \n",name.c_str(),bname.c_str());
        return 0;
    }
    
    template <class T>
    int SetOutTreeVars(std::string name,T variable,std::string name2){
        std::string bname = Form("%s[%s]/%s",name.c_str(),name2.c_str(),type_name<typeof(*variable)>::name());
        tree->Branch(name.c_str(),variable,bname.c_str());
        printf("set up var %s with branch %s \n",name.c_str(),bname.c_str());
        return 0;
    }
    
    long GetEntries(){return tree->GetEntries();}
    
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
        
        
        char* pPath;
        pPath = getenv ("HOME");
        std::string basename = Form("%s/",pPath);
        fname = basename + fname;
        
        printf("addfriend: name is %s with tree %s\n",fname.c_str(),tname.c_str());
        tree->AddFriend(tname.c_str(),fname.c_str());
        return 0;
        
    }
    
};



#endif /* defined(__CubeClassifier__IO__) */
