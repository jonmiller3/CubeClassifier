//
//  Classify.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "Classify.h"
#include "TFile.h"
#include "TTree.h"

int Classify::ProcessOutput(int* output_data, long nevents){
    
    this->CreateCubeMap(output_data,nevents);    
    
    return 0;
    
}

int Classify::CreateCubeMap(int* cubeset_out,long nevents){
    
    float_triple nullfloat3 = {{0,0,0}};
    //int *varj = (int*)malloc(sizeof(int)*(ndim+1));
    //std::string varjstr;
    
    for (int i=0; i<nevents; i++) {
        for (int k=0; k<mdim; k++) {
            std::vector<int> varj;
            varj.push_back(k);
            for (int j=0; j<ndim; j++) {
                varj.push_back(cubeset_out[i*ndim*mdim+k*ndim+j]);
                
                /*
                 std::cout<<datanorm_out[i*ndim+j]<<" this is "<<i<<" "<<j<<" "<<
                 " "<<k<<" result "<<cubeset_out[i*ndim*mdim+j+ndim*k]<<std::endl;
                 */
                 
            }
            //std::string varjstr;
            //int* varj = (int*)malloc(sizeof(int)*(ndim+1));
            cubeit=cubemap.begin();
            //varj[0] = k;
            //memcpy(&(varj[1]),&(cubeset_out[i*ndim*mdim+k*ndim]),sizeof(int)*ndim);
            // memmove might be better
            //memcpy(&varjstr, varj, sizeof(int)*(ndim+1));
            //std::string varjstr(varj,ndim+1);
            //varjstr[ndim+2] = '\0';
            //std::cout<<" string is "<<varjstr.c_str()<<"\n";
            
            // I don't know about this
            if (cubemap.find(varj)==cubemap.end()){
                //std::cout<<" adding in a new one "<<std::endl;
                cubemap.insert(cubeit, std::pair< std::vector<int>,float_triple>(varj,nullfloat3) );
            }
            
            
            
            /*
             float cuberes[3]={0};
             cuberes[(int)data[i][0]] = data[i][1];
             cubemap[varj]=cuberes;
             */
            cubeit=cubemap.find(varj);
            ((*cubeit).second).x[(int)data[i*2+0]]+=data[i*2+1]; // + (*cubeit).second[(int)data[i][0]];
            if (i%1000==0){
                std::cout<<" here it is (4) "<<varj[0]<<varj[1]<<varj[2]<<varj[3]<<varj[4]<<std::endl;
                std::cout<<" here it is (3) "<<((*cubeit).second).x[0]
                <<" "<<((*cubeit).second).x[1]<<" "<<((*cubeit).second).x[2]<<std::endl;
                std::cout<<" here it is (2) "<<data[i*2+0]<<" "<<data[i*2+1]<<std::endl;
            }
            
            
            /*
             
             */
        }
    }  
  
    
    return 0;
}

/*
// I might need float*
void Classify::CalcCubeResult(int qnum, int dim1_size, float_triple* result_out){
    
    dispatch_sync(queue[qnum], ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(calccuberesult_kernel,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        
        int work_size = dim1_size/wgs;
        cl_ndrange range = {
            1,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {dim1_size, 0, 0},    // The global range -- this is how many items
            // IN TOTAL in each dimension you want to
            // process.
            
            {work_size, 0, 0} // The local size of each workgroup.  This
            // determines the number of workitems per
            // workgroup.  It indirectly affects the
            // number of workgroups, since the global
            // size / local size yields the number of
            // workgroups.  So in our test case, we will
            // have NUM_VALUE / wgs workgroups.
        };
        
        
        
        
        calccuberesult_kernel(&range, (cl_float3*)mem_in, (cl_float3*)mem_out);
        
        gcl_memcpy(result_out, mem_out, sizeof(cl_float3) * dim1_size);
        
    });  
    
    gcl_free(mem_in);
    gcl_free(mem_out);
    
    
    return;
    
}
 */


Classify::Classify(Interface* inputinterface){
    
    // right now this is a big function
    
    interface=inputinterface;
    
    // is mdim just cubelevel?
    mdim=interface->GetCubeLevel();
    
    // obviously
    cubelevel=interface->GetCubeLevel();
    cubesetting=interface->GetCubeSetting();
    
    // not sure about this, this forces the whole set of names to be used...
    // but maybe that is right? if you are not interested just don't include them
    ndim=(interface->GetVarNameList()).size();
    
    
    currenttfile=0;
    currentttree=0;
    currenttype=-1;
    currentelem=0;
    
    
    //e0=0;
    //e1=0;
    //e2=0;
    
    //maxelem=0;

    std::string filename=interface->GetNameList()[currentelem];
    //maxelem=interface->GetNameList().size();
    currenttfile= new TFile(filename.c_str());
    std::string treename=interface->GetTreeList()[currentelem];
    currentttree=(TTree*)gDirectory->Get(treename.c_str());
    currenttype=interface->GetTypeList()[currentelem];

    
    
}

// this is a special 
// input is number of events
// output is the data_in that is used in one set of processing
// I need to keep track of which file is open
// which files are done
// where I am in the file
int Classify::InputData(long nevents, float* data_in){
     

    
    if (currenttfile==0){
        std::string filename=interface->GetNameList()[currentelem];
        currenttfile= new TFile(filename.c_str());
        currentttree=(TTree*)currenttfile->TDirectory::Get((interface->GetTreeList()[currentelem]).c_str());
        currenttype=interface->GetTypeList()[currentelem];
    }
    
    // let's assume it is root for now
    // this is maybe not necessary?

    size_t varsize=interface->GetVarNameList().size();
    float* var = new float[varsize];
    std::vector<std::string> varnamelist=interface->GetVarNameList();
    int i=0;
    for (std::vector<std::string>::iterator it=varnamelist.begin(); it!=varnamelist.end(); ++it,i++) {

        std::string varname = *it; 
        currentttree->SetBranchAddress(varname.c_str(), &(var[i]));
        
    }

    data = new float[nevents*2];    
    long cnum=0;
    
    // for now this is just using default, later I will need to set it here
    float weight=1;
    
    while (cnum<nevents) {
    
        currentttree->GetEntry(cnum+enumber);
        
        for (i=0; i<varsize; i++) {
            data_in[cnum*varsize+i]=var[i];
        }
        
        data[2*cnum+1]=weight;
        data[2*cnum+0]=(float)currenttype;
        
        //if (currenttype==0) {data0[e0]=var[i];e0++;}
        //if (currenttype==1) {data1[e1]=var[i];e1++;}
        //if (currenttype==2) {data2[e2]=var[i];e2++;}
        
        cnum++;
    }
    
    enumber+=nevents;
    if (nevents!=cnum) printf(" oh no, cnum != nevents! \n");
    
    if (enumber==EventsToProcess()) {
        
        currenttfile=0;
        currentelem++;
    }
    
    return 0;
    
}



// obvious, but I need to remember that this should be initalized in the constructor
long Classify::EventsToProcess(){    
    
    long val=0;
    
    long v1=0;
    long v2=0;
    long v0=0;
    
    std::vector<std::string> namelist=interface->GetNameList();
    std::vector<std::string> treelist=interface->GetTreeList();
    std::vector<int> typelist=interface->GetTypeList();
    std::vector<std::string>::iterator tt=treelist.begin();
    std::vector<int>::iterator yt=typelist.begin();
    for (std::vector<std::string>::iterator it=namelist.begin(); it!=namelist.end(); ++it,++tt,++yt){
        
        std::string filename=*it;
        TFile* tfile= new TFile(filename.c_str());
        std::string treename=*tt;
        TTree* ttree=(TTree*)gDirectory->Get(treename.c_str());
        int typet = *yt;
        
        if (typet==0) v0++;
        if (typet==1) v1++;
        if (typet==2) v2++;
        
        
        val += ttree->GetEntries();

    }
        
    //data0 = new float[v0];
    //data1 = new float[v1];
    //data2 = new float[v2];
    
    //val=20;
   // currentttree->Print();
    
    edim=val;
    return val;
}

// very simple, wish there was some nicer way to do this but it goes quickly at least?
int Classify::SetMaxMin(float* max, float* min){
    
    int i=0;
    
    std::vector<double> minlist = interface->GetMinList();
    std::vector<double> maxlist = interface->GetMaxList();
    
    std::vector<double>::iterator minit=minlist.begin();
    for (std::vector<double>::iterator maxit=maxlist.begin(); maxit!=maxlist.end(); ++maxit,++minit,i++) {
        
        max[i]=*maxit;
        min[i]=*minit;
 
    }
    
    return 0;
    
}
