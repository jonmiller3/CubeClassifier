//
//  PreProcess.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 11/10/15.
//  Copyright © 2015 UTFSM. All rights reserved.
//

#include "PreProcess.hpp"

PreProcess::PreProcess(Interface* inputinterface):Base(inputinterface){


    max = new float[nvars];
    min = new float[nvars];
    
    SetMaxMin();
     
    
    return;
    
}

/*
long PreProcess::InputData(long nevents){
    
    // for now this is just using default, later I will need to set it here
    float weight=1;
    int currentelem = interface->GetNextElem(0, -1);
    
    currentIO = new IO<TTree, TFile>(interface->GetFileName(currentelem),
                                     interface->GetTreeName(currentelem),currentelem,
                                     interface->GetType(currentelem),0);
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        currentIO->SetTreeVars(varname.c_str(), &(var[i]));
        
    }
    
    interface->SetTreeEntries(currentelem, currentIO->GetEntries());
    
    
    weight=1.0/((float)currentIO->GetEntries());
    
    // let's assume it is root for now
    // this is maybe not necessary?
    
    
    long cnum=0;
    
    while (cnum<nevents) {
        
        
        int tinfo = currentIO->GetEntry(cnum);
        
        if (tinfo==0){
            currentelem = interface->GetNextElem(0, currentelem);
            GetNewTree(currentelem);
            
            weight=1.0/((float)currentIO->GetEntries());
            
            tinfo = currentIO->GetEntry(cnum);
        }
        
        int j=0;
        for (int i=0; i<nvars; i++) {
            int par=interface->GetVarParameter(i);
            if (par>0) continue;
            if (data_in.find(j)==data_in.end()){
                std::vector<float> vvar;
                vvar.push_back(var[i]);
                data_in[j] = vvar;
            } else {
                data_in[j].push_back(var[i]);
            }
            j++;
        }
        
        
        cnum++;
    }
    
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");
    
    return 0;
    
}
 */

int PreProcess::SetData(int par,int i,int j){
    
    if (data_in.find(j)==data_in.end()){
        std::vector<float> vvar;
        vvar.push_back(var[i]);
        data_in[j] = vvar;
    } else {
        data_in[j].push_back(var[i]);
    }
    
    return 0;
}

/*
int PreProcess::GetNewTree(int newelem){
    
    
    
    currentIO = new IO<TTree, TFile>(interface->GetFileName(newelem),
                                     interface->GetTreeName(newelem),newelem,
                                     interface->GetType(newelem),0);
    
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {
        
        std::string varname = *it;
        currentIO->SetTreeVars(varname.c_str(), &(var[i]));
        
    }
    
    interface->SetTreeEntries(newelem, currentIO->GetEntries());
    
    return 0;
}
 */

int PreProcess::Process(int nevents=10000){
    
    
    InputData(nevents);
    
    std::map<int, std::vector<float> >::iterator it = data_in.begin();
    
    while (it != data_in.end()) {
        
        std::sort((it->second).begin(),(it->second).end());
        it++;

    }
    
    it = data_in.begin();
    int it_count=0;

    while (it != data_in.end()) {
     
        // here I need a function to get the difference
        
        // first the nearest neighbor
        
        float diff = GetDifference(it->second,1);
        int div = (max[it_count]-min[it_count])/diff;
        
        //std::vector<int> vdiff;
        //vdiff.push_back(div);
        diff_set[0].push_back(div);
        
        // next the
        for (int i=1; i<cubelevel; i++) {
            
            //can have problems if nevents/std::pow(2.0,(double)i)<2
            diff = GetDifference(it->second,nevents/std::pow(2.0,(double)i)-1);
            div = (max[it_count]-min[it_count])/diff;
            diff_set[i].push_back(div);
            
            
        }
        
        
        it++;
        it_count++;
        
    }
    
    
    
    
    return 0;
    
}


float PreProcess::GetDifference(std::vector<float> values ,float skip ){
    
    float res=1e60;
    
    std::vector<float>::iterator it=values.begin();
    
    it+=(skip);
    std::cout<<" skip here "<<skip<<" and "<<values.size()<<std::endl;
    int counter=0;
    
    while (it != values.end()-skip) {

        
        float diff = std::abs((*it) - (*(it-skip)));
        
        if (diff<res&&diff>0) res=diff;
        
        diff = std::abs((*it) - (*(it+skip)));
        
        if (diff<res&&diff>0) res=diff;
    
        it++;
        counter++;
    
    }
    
    std::cout<<" difference for "<<skip<<" "<<res<<std::endl;
    
    return res;
    
}

int PreProcess::WriteOutput(std::string filename){
    
    
    std::string output = CreateOpenCLBuffer();
    
    char* pPath;
    pPath = getenv ("HOME");
    std::string basename = Form("%s/Dropbox/CubeClassifier/",pPath);
    filename = basename + filename;
    
    std::cout<<" test "<<sizeof(output.c_str())<<" "<<output.c_str()<<std::endl;
    
    std::ofstream outfile;
    outfile.open(filename.c_str(),std::ios::out);
    outfile.write (output.c_str(),output.size());
    
    
    
    
    return 0;
    
}

std::string PreProcess::CreateOpenCLBuffer(){
    
    std::stringstream bstream;
    
    bstream<<"\n";
 
    
    bstream<<"\n";
    bstream<<"\n";
    bstream<<"//#include <OpenCL/OpenCL.h> \n ";
    bstream<<"\n";
    
    bstream<<"kernel void fillpreprocesscubefull(global const float* input, constant float* max, constant float* min, global int* output)";
    bstream<<"{\n";

    
    // here we make the array (look up?) 
    std::map<int, std::vector<int> >::iterator it_set = diff_set.begin();

    it_set++;
    bstream<<" int model[";
    bstream<<(it_set->second).size()*diff_set.size();
    bstream<<"] = { ";
    it_set--;
    
    while (it_set != diff_set.end()) {

    
        std::stringstream ss;
        
        for(size_t i = 0; i < (it_set->second).size(); ++i)
        {
            if(i != 0||it_set!=diff_set.begin())
                ss << ",";
            ss << (it_set->second)[i];
        }
        
        std::string s = ss.str();
        
        const char* cs = s.c_str();
        
        bstream<<cs;
        
        it_set++;
    
    }
    
    bstream<<" };";
    
    bstream<<"\n";
    bstream<<"\n";
    bstream<<"int i = get_global_id(0); int j = get_global_id(1); int k = get_global_id(2); int p = get_global_size(0); int r = get_global_size(1); int q = get_global_size(2);\n";
    bstream<<"\n";
    bstream<<"float trange = max[j]-min[j]; float difference = input[i*r+j]-min[j]; \n";
    bstream<<"\n";
    // this is the part where I need to have it look up in the array with j/k
    bstream<<"int mult = model[q*j+k];";
    bstream<<"\n";
    bstream<<"output[i*r*q+r*k+j]=(int)(difference/trange*mult);\n";
    bstream<<"\n";
    bstream<<"}";
    
    
    std::string s=bstream.str();
    std::cout<<" test string "<<s.c_str()<<std::endl;
    //char* res=s.c_str();
    
    return s;
    
}


int PreProcess::SetMaxMin(){
    
    int j=0;
    for (int i=0; i<ndim; i++) {
        int par=interface->GetVarParameter(i);
        if (par>0) continue;
        max[j]=interface->GetMax(i);
        min[j]=interface->GetMin(i);
        j++;
    }
    
    return 0;
    
}
