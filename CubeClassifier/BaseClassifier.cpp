//
//  BaseClassifier.cpp
//  CubeClassifier
//
//  Created by Jonathan Miller on 10/26/12.
//  Copyright (c) 2012 Uppsala Vrije Universiteit Brussel. All rights reserved.
//

#include <iostream>
#include <OpenCL/OpenCL.h>

#include "BaseClassifier.h"
//#include "fillcube.cl"
#include "fillcube2.cl"
#include "fillcube3.cl"

void BaseClassifier::NormData(int qnum, int dim1_size, int dim2_size, float* datanorm_out){
    
    
    
    dispatch_sync(queue[qnum], ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(normdata_kernel,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        int workgroup_size = dim1_size*dim2_size/wgs;
        cl_ndrange range = {
            2,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {dim1_size, dim2_size, 0},    // The global range -- this is how many items
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
        
        gcl_memcpy(datanorm_out, mem_out, sizeof(cl_float) * dim1_size * dim2_size);
        
    }); 
    
    
    gcl_free(mem_in);
    gcl_free(mem_out);
    
    gcl_free(memmax_in);
    gcl_free(memmin_in);
    
    return;
    
}


void BaseClassifier::FillCube(int qnum, int dim1_size, int dim2_size, int dim3_size, int* cubeset_out){
    
    
    dispatch_sync(queue[qnum], ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(fillcube_kernel,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        
        int work_size = dim1_size/wgs*dim2_size*dim3_size;
        cl_ndrange range = {
            3,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {dim1_size, dim2_size, dim3_size},    // The global range -- this is how many items
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
        
        
        
        
        fillcube_kernel(&range, (cl_float*)mem_in, (cl_int*)mem_out);
        
        gcl_memcpy(cubeset_out, mem_out, sizeof(cl_int) * dim1_size * dim2_size * dim3_size);
        
    });          
    
    gcl_free(mem_in);
    gcl_free(mem_out);
    
    return;
    
    
}

void BaseClassifier::FillCube2(int qnum, int dim1_size, int dim2_size, int dim3_size, int* cubeset_out){
    
    
    dispatch_sync(queue[qnum], ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(fillcube_kernel2,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        
        int work_size = dim1_size/wgs*dim2_size*dim3_size;
        cl_ndrange range = {
            3,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {dim1_size, dim2_size, dim3_size},    // The global range -- this is how many items
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
        
        
        
        
        fillcube_kernel2(&range, (cl_float*)mem_in, (cl_int*)mem_out);
        
        gcl_memcpy(cubeset_out, mem_out, sizeof(cl_int) * dim1_size * dim2_size * dim3_size);
        
    });          
    
    gcl_free(mem_in);
    gcl_free(mem_out);
    
    return;
    
    
}

void BaseClassifier::FillCube3(int qnum, int dim1_size, int dim2_size, int dim3_size, int* cubeset_out){
    
    
    dispatch_sync(queue[qnum], ^{
        
        size_t wgs;
        gcl_get_kernel_block_workgroup_info(fillcube3_kernel,
                                            CL_KERNEL_WORK_GROUP_SIZE,
                                            sizeof(wgs), &wgs, NULL);
        
        int work_size = dim1_size/wgs*dim2_size*dim3_size;
        cl_ndrange range = {
            3,                     // The number of dimensions to use.
            
            
            {0, 0, 0},             // The offset in each dimension.  We want to
            // process ALL of our data, so this is 0 for
            // our test case.                          [7]
            
            {dim1_size, dim2_size, dim3_size},    // The global range -- this is how many items
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
        
        
        
        
        fillcube_kernel3(&range, (cl_float*)mem_in, (cl_int*)mem_out);
        
        gcl_memcpy(cubeset_out, mem_out, sizeof(cl_int) * dim1_size * dim2_size * dim3_size);
        
    });          
    
    
    gcl_free(mem_in);
    gcl_free(mem_out);
    
    return;
    
    
}