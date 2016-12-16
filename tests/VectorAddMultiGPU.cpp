#include <iostream>
#include "HPL.h"

#define N 400


using namespace HPL;

void myf1(Array<float, 1> a)
{
  a[idx] +=  1;
}

void myf2(Array<float, 1> a)
{
  a[idx] +=  1;
}

int result_test(bool correct)
{
  if( correct ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}
			  
int main()
{
  Array<float, 1> av(N);
  
  const int GPUs_number = HPL::getDeviceNumber(GPU);
  
  std::cout << "There are "<< GPUs_number << " GPUs.\n";
  if (HPL::getDeviceNumber(GPU) < 2) {
    std::cout << "This test needs 2 GPUs. \n";
    std::cout << "Test is PASSED. \n";
    return 0;
  }
  
  HPL::Device device1(GPU, 0);
  HPL::Device device2(GPU, 1);
  
  for(int i =0; i < N; i++) {
    av(i) = 0.f;
  }
  
  eval(myf2).device(device1)(av);
  eval(myf2).device(device2)(av);
  
  return result_test( av.reduce(std::plus<float>()) == 800.0 );
}
