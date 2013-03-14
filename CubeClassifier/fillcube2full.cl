// I need to think about local versus global (versus private)

//#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
//#pragma OPENCL EXTENSION cl_intel_printf : enable
//#pragma OPENCL EXTENSION cl_amd_printf : enable


kernel void fillcube2full(global const float* input, constant float* max, constant float* min, global int* output)
{
    
    
    int i = get_global_id(0);
    int j = get_global_id(1);
    int k = get_global_id(2);
    
    int p = get_global_size(0);
    int r = get_global_size(1); 
    int q = get_global_size(2); 
    
    
    //output[0]=1;
    //output[i*r*q+r*k+j]=1;

    //printf(" simple %i %i \n",p,r);
    
    float trange = max[j]-min[j];
    float difference = input[i*r+j]-min[j];
    
    int mult = k*k+2;
    
    // input is a value that is x1,y1,z1 and so on
    //output[(i*r+j)*q+k]=0;;
    
    //printf("here it is %i %i %i %f %f \n",mult,i,j,trange,difference);
    
    output[i*r*q+r*k+j]=(int)(difference/trange*mult);
    
    //output[(i*r+j)*q+k]=k;
    
    //output[0]=k;
    
    // basically this is a very simple operation
    // I want to determine if it is in the first 1/*input2 or the 2 or the 3rd...
    
    // that should work, I just need to normalize my input
    // and possibly change some things from float to int/etc
    
}