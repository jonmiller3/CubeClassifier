kernel void normdata(global float* input, global float* max, global float* min, global float* output)
{
    
    size_t i = get_global_id(0);
    size_t j = get_global_id(1);
    
    // input is a value that is x1,y1,z1 and so on
    output[i,j] = (input[i,j]-min[j])/(max[j]-min[j]);
    
}