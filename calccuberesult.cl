kernel void calccuberesult(global float3* input, global float3* output)
{

    // is this defined right?
    size_t i = get_global_id(0);

    // remember at the end what you want is a set of rules/etc to classify
    // in this case you want a set of cubes
    // you need some determinations of whether to keep the cube
    // (num events) (systematic error)
    // and the value of the cube
    // value = C/(A+C)
    // num events - C+A+B
    // sys = A/(A+B)
    // very simple
    
    output[i][0] = input[i][2]/(input[i][0]+input[i][2]);
    output[i][1] = input[i][0]/(input[i][0]+input[i][1]);
    output[i][2] = input[i][2]+input[i][1]+input[i][0];

    // or maybe
    // value = C/(A+C)
    // num events = C+A+B
    // num of used = A+C
    
    
}
    
// basically you use the last two to trim the cubes