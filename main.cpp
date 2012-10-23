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

#include <map>


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
    
    
    
    // now the data, each bit of data has N values (N = number of variabes = ndim)
    int eventsA=100;
    int eventsC=100;
    int ndim=4;

    float* datanorm_in = (float*)malloc(sizeof(cl_float)*(eventsA+eventsC)*ndim);
    
    
    for (int i=0; i<eventsA; i++) {
        for (int j=0; j<ndim; j++) {
            datanorm_in[i,j]=(cl_float)2;
        }
    }
    
    for (int i=eventsA; i<eventsC; i++) {
        for (int j=0; j<ndim; j++) {
            if ((i+j)%2==0) {
                datanorm_in[i,j]=(cl_float)1;
            } else {
                datanorm_in[i,j]=(cl_float)0;
            }
        }
    }
    // data is done
    
    float* max_in = (float*)malloc(sizeof(cl_float)*ndim);
    float* min_in = (float*)malloc(sizeof(cl_float)*ndim);
    
    for (int j=0; j<ndim; j++) {
        max_in[j]=2;
        min_in[j]=0;
    }
    
    
    // so my idea actually is that I have 2 dimensions for the data (A+B+C) X N

    float* datanorm_out = (float*)malloc(sizeof(cl_float)*(eventsA+eventsC)*ndim);
    
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
    free(datanorm_in);
    
    // so my idea actually is that I have 2 dimensions for the data (A+B+C) X N 
    // and out would be 3 dimensions, W=(A+B+C) X N X M
    
    // this would be another call to the gpu and fillcube
    

    free(datanorm_out);
    
    // after this I do my multimap
    // multimap fills the float3 with the 3D result
    // something like this
    // multimap(array_int,float3)
    // the array int defines the cube and is of size N+1 (the extra 1 defines the size of the cube, whether it is 0 or not)
    // filled like so: (note that I am just grabbing the Q=[i]XNX[k] that comes out)
    // if (!multimap[(k,Q)]) multimap[Q]=0
    // multimap[(k,Q)][data[i,0]]+=data[i,1] (note data[i,0] is where it is A,B or C (0,1,2) and data[i,1] is the weight)
    
    
    
    // the set of all these float3 is an array
    // and gets sent to calccuberesult
    // and comes back an array of float3 with the same *index*
    
    
    // so now I have the request, as I have for an i
    // the multimap[(k,Q)][] and can 'sum' them up along k to get the total
    
    
    
    
    
    dispatch_release(queue);
    
    
    // I have memory management stuff to do 
    
        
    return 0;
}

