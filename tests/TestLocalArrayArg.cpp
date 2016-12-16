#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

void display4(const Array<float, 1>& av, const char *name)
{
  std::cout << "Array<float, 1> " << name << ":\n";
  
  for(int i =0; i < 4; i++) {
    std::cout << av(i) << "  ";
  }
  std::cout << std::endl;
}

void squareGPU(Array<float, 1> input, Array<float, 1> output, Array<float, 1, Local> temp)
{
  if_ (idx < N) {
    temp[lidx] = input[idx];
    output[idx] =  temp[lidx] * temp[lidx];
  }
  
  //output[idx] =  input[idx] * input[idx];
}

void squareCPU(float *input, float *output)
{
  for(int i =0; i < N; i++) output[i] =  input[i] * input[i];
}


int main()
{ 
  float input[N], output[N];

  for(int i =0; i < N; i++) 
    input[i] = i;
  
  squareCPU(input,output);

  Array<float, 1> inputv(N, input), outputv(N);
  Array<float, 1, Local> localv(N/4);
  
  eval(squareGPU).global(N).local(N/4) (inputv, outputv, localv);

  for(int i =0; i < 10; i++) {
    std::cout << i << " : " << output[i] << " vs " << outputv.data()[i] << std::endl;
  }
  
  if(memcmp(output, outputv.getData(), N*sizeof(float)) == 0) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }

}
