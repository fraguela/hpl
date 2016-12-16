#include <iostream>
#include "HPL.h"

#define N 40

using namespace HPL;

void pipeline_stage1(Array<float, 1> a, Array<float, 1> b)
{
  b[idx] =  a[idx];  
}

void pipeline_stage2(Array<float, 1> a, Array<float, 1> b)
{
  b[idx] =  a[idx];  
}

int test1()
{ 
  Array<float, 1> a(N), b(N);
  for(int i =0; i < N; i++) {
      a(i) = 5.0;
      b(i) = 6.0; 
  }
  HPL::Device d1(getDeviceNumber(GPU) ? GPU : CPU, 0);
  eval(pipeline_stage1).device(d1)(a, b); // a.intent(IN), b.intent(OUT)
  eval(pipeline_stage2).device(d1)(a, b); // a.intent(IN), b.intent(OUT)

  if( a.hdm.numberof_copy_in == 1 && a.hdm.numberof_copy_out == 0 ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

int test2()
{ 
  Array<float, 1> a(N), b(N);
  for(int i =0; i < N; i++) {
    a(i) = 5.0;
    b(i) = 6.0; 
  }
  HPL::Device d1(getDeviceNumber(GPU) ? GPU : CPU, 0);
  eval(pipeline_stage1).device(d1)(a, b); // a.intent(IN), b.intent(OUT)
  a(1) = b(1);
  eval(pipeline_stage2).device(d1)(a, b); // a.intent(IN), b.intent(OUT)
  
  if( a.hdm.numberof_copy_in == 2 && a.hdm.numberof_copy_out == 0 ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }  
  
}


int main()
{
  return test1() + test2();
}