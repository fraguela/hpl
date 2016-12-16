#include <iostream>
#include "HPL_clBLAS.h"

#define N 4

int check(bool is_ok)
{
  if(is_ok) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

int main()
{ HPL::Array<HPL::float2, 1> x(N);
  cl_float2 alpha = {1.5f, 2.5f};
  
  for(int i = 0; i < N; ++i) {
    x(i)(0) = i * 3;         //0  3  6  9
    x(i)(1) = 1;
  }

  HPL_clblasSetup();
  
  clBlasCscal(alpha, x);

  HPL_clblasTeardown();

  return check((x(0)(0) == -2.5f) && (x(1)(0) == 2.f) && (x(2)(0) == 6.5f) && (x(3)(0) == 11.f) &&
               (x(0)(1) == 1.5f) && (x(1)(1) == 9.f) && (x(2)(1) == 16.5f) && (x(3)(1) == 24.f));
}
