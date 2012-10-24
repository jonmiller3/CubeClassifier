kernel void calccuberesult(global float3* input, global float3* output)
{

    // is this defined right?
    size_t i = get_global_id(0);

    if (input[i][0]!=0){
        output[i][0] = input[i][2]/input[i][0];
        output[i][1] = sqrt(input[i][0])/input[i][0];
        if (input[i][1]!=0) {
            output[i][2] = (input[i][2]/input[i][0]-input[i][2]/input[i][1])/(input[i][2]/input[i][0]);
        } else if (input[i][2]==0) {
            output[i][2] = 1; 
        } else {
            output[i][2] = 0;
        }
    } else {
        output[i][0]=1;
        // I don't know if I should use this as statistical error?
        output[i][1]=sqrt(input[i][2])/input[i][2];
        if (input[i][1]==0){
            output[i][2]=0;
        } else {
            output[i][2]=1;
        }
    }
    
    // I think that sqrt works

    // I need to look at the output
    
}