kernel void fillcube(global float* input, global int* output)
{

    
    int i = get_global_id(0);
    int j = get_global_id(1);
    int k = get_global_id(2);
    
    int p = get_global_size(0);
    int r = get_global_size(1); 
    int q = get_global_size(2); 
    
    
    // input is a value that is x1,y1,z1 and so on
    //output[(i*r+j)*q+k]=0;;
    
    output[i*r*q+j*q+k]=(int)(input[i*r+j]*(k+2));
    
    //output[(i*r+j)*q+k]=k;
    
    //output[0]=k;
    
    // basically this is a very simple operation
    // I want to determine if it is in the first 1/*input2 or the 2 or the 3rd...
    
    // that should work, I just need to normalize my input
    // and possibly change some things from float to int/etc
    
}