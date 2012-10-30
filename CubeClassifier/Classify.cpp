//
//  Classify.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/25/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>

#include "Classify.h"

void Classify::CreateCubeMap(int* cubeset_out){
    
    float_triple nullfloat3 = {{0,0,0}};
    //int *varj = (int*)malloc(sizeof(int)*(ndim+1));
    //std::string varjstr;
    
    for (int i=0; i<edim; i++) {
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
            
            std::cout<<" here it is (4) "<<varj[0]<<varj[1]<<varj[2]<<varj[3]<<varj[4]<<std::endl;
            std::cout<<" here it is (3) "<<((*cubeit).second).x[0]
            <<" "<<((*cubeit).second).x[1]<<" "<<((*cubeit).second).x[2]<<std::endl;
            std::cout<<" here it is (2) "<<data[i*2+0]<<" "<<data[i*2+1]<<std::endl;
            
            
            /*
             
             */
        }
    }  
  
    
    return;
}

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


Classify::Classify(Interface*){
    
    // right now this is a big function
    
    
    // it starts up the queue
    
    
    
}


