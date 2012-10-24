//
//  main.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/22/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>
#include <OpenCL/OpenCL.h>
#include "fillcube.cl.h"
#include "normdata.cl.h"
#include "calccuberesult.cl.h"

//#include "boost/lexical_cast.hpp"
//#include "boost/tuple/tuple.hpp"
//#include "boost/tuple/tuple_comparison.hpp"
//#include "boost/tuple/tuple_io.hpp"


#include <map>
#include <vector>
#include <string>

struct float_triple {
    float x[3];
};

int main (int argc, const char * argv[])
{
    
    // set up to use the gpu
    dispatch_queue_t queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_GPU,
                                                       NULL);

    // set up to use the cpu
    if (queue == NULL) {
        queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_CPU, NULL);
    }    
    
    // I can name it?
    char name[128];
    cl_device_id gpu = gcl_get_device_id_with_dispatch_queue(queue);
    clGetDeviceInfo(gpu, CL_DEVICE_NAME, 128, name, NULL);
    fprintf(stdout, "Created a dispatch queue using the %s\n", name);
    
    // I probably should get other peices of information like number of devices
    // and how much the device can process.... 
    
    
    
    // now the data, each bit of data has N values (N = number of variabes = ndim)
    int eventsA=10;
    int eventsC=10;
    int ndim=4;
    // here we do M^N
    int mdim=3;
    int numsquares=mdim^ndim;
    
    double data[eventsA+eventsC][2];
    
    float* datanorm_in = (float*)malloc(sizeof(cl_float)*(eventsA+eventsC)*ndim);
    
    for (int i=0; i<eventsA; i++) {
        for (int j=0; j<ndim; j++) {
            datanorm_in[i*ndim+j]=(cl_float)2;
            //std::cout<<i<<" "<<j<<" "<<datanorm_in[i*ndim+j]<<std::endl;
        }
        data[i][0]=0;
        data[i][1]=1;
    }
    
    for (int i=eventsA; i<eventsC+eventsA; i++) {
        for (int j=0; j<ndim; j++) {
            if ((i+j)%3==0) {
                datanorm_in[i*ndim+j]=(cl_float)1;
            } else {
                datanorm_in[i*ndim+j]=(cl_float)0;
            }
            //std::cout<<i<<" "<<j<<" "<<datanorm_in[i*ndim+j]<<std::endl;
        }
        data[i][0]=1;
        data[i][1]=1;
    }
    float* max_in = (float*)malloc(sizeof(cl_float)*ndim);
    float* min_in = (float*)malloc(sizeof(cl_float)*ndim);
    
    for (int j=0; j<ndim; j++) {
        max_in[j]=4;
        min_in[j]=0;
    }
    
    
    // so my idea actually is that I have 2 dimensions for the data (A+B+C) X N

    float* datanorm_out = (float*)malloc(sizeof(cl_float)*(eventsA+eventsC)*ndim);
    /*
    for (int i=0; i<(eventsC+eventsA); i++) {
        for (int j=0; j<4; j++) {
            std::cout<<datanorm_in[i,j]<<" this is "<<i<<" "<<j<<" "<<datanorm_out[i,j]<<std::endl;
        }
    }
     */
    
    // deal with memory going into the cl device
    void* mem_in  = gcl_malloc(sizeof(cl_float) * (eventsA+eventsC) * ndim, datanorm_in,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    
    void* memmax_in  = gcl_malloc(sizeof(cl_float) * ndim, max_in,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    
    void* memmin_in  = gcl_malloc(sizeof(cl_float) * ndim, min_in,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    
    
    void* mem_out = gcl_malloc(sizeof(cl_float) * (eventsA+eventsC) * ndim, NULL,
                               CL_MEM_WRITE_ONLY);
    
    
    dispatch_sync(queue, ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(normdata_kernel,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        int workgroup_size = (eventsA+eventsC)*ndim/wgs;
        cl_ndrange range = {
            2,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {eventsA+eventsC, ndim, 0},    // The global range -- this is how many items
            // IN TOTAL in each dimension you want to
            // process.
            
            {workgroup_size, 0, 0} // The local size of each workgroup.  This
            // determines the number of workitems per
            // workgroup.  It indirectly affects the
            // number of workgroups, since the global
            // size / local size yields the number of
            // workgroups.  So in our test case, we will
            // have NUM_VALUE / wgs workgroups.
        };
        
        normdata_kernel(&range,(cl_float*)mem_in, (cl_float*)memmax_in, (cl_float*)memmin_in, (cl_float*)mem_out);
    
        gcl_memcpy(datanorm_out, mem_out, sizeof(cl_float) * (eventsA+eventsC) * ndim);
        
    }); 
        

    gcl_free(mem_in);
    gcl_free(memmax_in);
    gcl_free(memmin_in);
    gcl_free(mem_out);
    
    // so my idea actually is that I have 2 dimensions for the data (A+B+C) X N 
    // and out would be 3 dimensions, W=(A+B+C) X N X M
    
    // this would be another call to the gpu and fillcube
    
    
    /*
    for (int i=0; i<(eventsC+eventsA); i++) {
        for (int j=0; j<ndim; j++) {
            std::cout<<datanorm_in[i*ndim+j]<<" this is "<<i<<" "<<j<<" "<<datanorm_out[i*ndim+j]<<std::endl;
        }
    }
     */
    free(datanorm_in);
     
    mem_in  = gcl_malloc(sizeof(cl_float) * (eventsA+eventsC) * ndim, datanorm_out,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // so my idea actually is that I have 2 dimensions for the data (A+B+C) X N X M
    
    int* cubeset_out = (int*)malloc(sizeof(cl_int) * (eventsA+eventsC) * ndim * mdim);
    mem_out = gcl_malloc(sizeof(cl_int) * (eventsA+eventsC) * ndim * mdim, NULL,
                                 CL_MEM_WRITE_ONLY);

    dispatch_sync(queue, ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(fillcube_kernel,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
   
        int dim1_size = (eventsA+eventsC)/wgs;
        int dim2_size = mdim;
        int dim3_size = ndim;
        cl_ndrange range = {
            3,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {eventsA+eventsC, ndim, mdim},    // The global range -- this is how many items
            // IN TOTAL in each dimension you want to
            // process.
            
            {dim1_size*dim2_size*dim3_size, 0, 0} // The local size of each workgroup.  This
            // determines the number of workitems per
            // workgroup.  It indirectly affects the
            // number of workgroups, since the global
            // size / local size yields the number of
            // workgroups.  So in our test case, we will
            // have NUM_VALUE / wgs workgroups.
        };
        

        
        
        fillcube_kernel(&range, (cl_float*)mem_in, (cl_int*)mem_out);
        
        gcl_memcpy(cubeset_out, mem_out, sizeof(cl_int) * (eventsA+eventsC) * ndim * mdim);
        
    });       

    std::map< std::vector<int>,float_triple> cubemap;
    std::map< std::vector<int>,float_triple>::iterator cubeit; //  = cubemap.begin();
    float_triple nullfloat3 = {{0,0,0}};
    //int *varj = (int*)malloc(sizeof(int)*(ndim+1));
    //std::string varjstr;
    
    for (int i=0; i<(eventsC+eventsA); i++) {
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
            ((*cubeit).second).x[(int)data[i][0]]+=data[i][1]; // + (*cubeit).second[(int)data[i][0]];
            
            std::cout<<" here it is (4) "<<varj[0]<<varj[1]<<varj[2]<<varj[3]<<varj[4]<<std::endl;
            std::cout<<" here it is (3) "<<((*cubeit).second).x[0]
                <<" "<<((*cubeit).second).x[1]<<" "<<((*cubeit).second).x[2]<<std::endl;
            std::cout<<" here it is (2) "<<data[i][0]<<" "<<data[i][1]<<std::endl;
            
            
            /*

             */
        }
    }  
    
    int cubemapsize = cubemap.size();
    int itcount = 0;
    float* result_in = (float*)malloc(sizeof(cl_float3)*cubemapsize);
    for (cubeit=cubemap.begin(); cubeit!=cubemap.end(); cubeit++,itcount++) {
        memcpy(&result_in[itcount*cubemapsize],&(((*cubeit).second).x[0]),sizeof(float)*3);
        std::cout<<" cubeit "<<(*cubeit).first[0]<<" "<<(*cubeit).first[ndim-1]<<" "<<((*cubeit).second).x[0]<<std::endl;
    }
    
    //std::cout<<" I did it! "<<cubemap.size()<<std::endl;
    

    gcl_free(mem_in);
    gcl_free(mem_out);    
    
    
    // after this I do my multimap
    // multimap fills the float3 with the 3D result
    // something like this
    // multimap(array_int,float3)
    // the array int defines the cube and is of size N+1 (the extra 1 defines the size of the cube, whether it is 0 or not)
    // filled like so: (note that I am just grabbing the Q=[i]XNX[k] that comes out)
    // if (!multimap[(k,Q)]) multimap[Q]=0
    // multimap[(k,Q)][data[i,0]]+=data[i,1] (note data[i,0] is where it is A,B or C (0,1,2) and data[i,1] is the weigh)
    
    
    mem_in  = gcl_malloc(sizeof(cl_float3) * cubemapsize, result_in,
                         CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    
    float* result_out = (float*)malloc(sizeof(cl_float3)*cubemapsize);
    mem_out  = gcl_malloc(sizeof(cl_float3) * cubemapsize, NULL,
                         CL_MEM_WRITE_ONLY);
    
    
    dispatch_sync(queue, ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(calccuberesult_kernel,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        
        int dim1_size = cubemapsize/wgs;
        cl_ndrange range = {
            1,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {cubemapsize, 0, 0},    // The global range -- this is how many items
            // IN TOTAL in each dimension you want to
            // process.
            
            {dim1_size, 0, 0} // The local size of each workgroup.  This
            // determines the number of workitems per
            // workgroup.  It indirectly affects the
            // number of workgroups, since the global
            // size / local size yields the number of
            // workgroups.  So in our test case, we will
            // have NUM_VALUE / wgs workgroups.
        };
        
        
        
        
        calccuberesult_kernel(&range, (cl_float3*)mem_in, (cl_float3*)mem_out);
        
        gcl_memcpy(result_out, mem_out, sizeof(cl_float3) * cubemapsize);
        
    });          
    
    itcount=0;
    for (cubeit=cubemap.begin(); cubeit!=cubemap.end(); cubeit++,itcount++) {
        memcpy(&(((*cubeit).second).x[0]),&result_out[itcount*cubemapsize],sizeof(float)*3);
        std::cout<<" cubeit "<<(*cubeit).first[0]<<" "<<(*cubeit).first[ndim-1]<<" "<<((*cubeit).second).x[0]<<std::endl;
    }        

    // something is still broken? there should be two cubes and not one (I think)
    
    free(datanorm_out);    
    
    
    // the set of all these float3 is an array
    // and gets sent to calccuberesult
    // and comes back an array of float3 with the same *index*
    
    
    // so now I have the request, as I have for an i
    // the multimap[(k,Q)][] and can 'sum' them up along k to get the total
    
    
    
    // at the end I want to do a weighting by the number of squares (basically)
    // I Can think of it as being the density, so if there are lots of counts in a small square
    // the density can still be quite high and increase the weighting
    
    dispatch_release(queue);
    
    
    // I have memory management stuff to do 
    
        
    return 0;
}

