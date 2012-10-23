kernel void normdata(global float* input, global float* max, global float* min, global float* output)
{
    
    int i = get_global_id(0);
    int j = get_global_id(1);
    
    int p = get_global_size(0);
    int r = get_global_size(1);
    
    output[i*p+j]=0;
 
    float trange = max[j]-min[j];
    float difference = input[i*r+j]-min[j];
    output[i*r+j] = difference/trange;

}