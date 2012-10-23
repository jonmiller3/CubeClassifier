kernel void fillcube(global float* input, global int* output)
{
    
    size_t i = get_global_id(0);
    size_t j = get_global_id(1);
    size_t k = get_global_id(2);
    
    // input is a value that is x1,y1,z1 and so on
    
    output[i,j,k]=(int)(input[i,j]*(k+2));
    
    // basically this is a very simple operation
    // I want to determine if it is in the first 1/*input2 or the 2 or the 3rd...
    
    // that should work, I just need to normalize my input
    // and possibly change some things from float to int/etc
    
}