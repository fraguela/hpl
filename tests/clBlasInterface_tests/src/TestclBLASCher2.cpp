#include <iostream>
#include "HPL_clBLAS.h"

#define M 5

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
{
  HPL::Array<HPL::float2, 2> A(M,M);
  HPL::Array<HPL::float2, 1> x(M);
  HPL::Array<HPL::float2, 1> y(M);
  
  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    A(i,j)(0) = j + 1;
    A(i,j)(1) = 4;
   }
  }

  for(int i = 0; i < M; i++) {
   x(i)(0) = 1 << i + 1;
   x(i)(1) = 2;
  }

  for(int i = 0; i < M; i++) {
   y(i)(0) = 1;
   y(i)(1) = 3;
  }

  HPL_clblasSetup();

  clblasCher2( clblasUpper, x, y, A);

  HPL_clblasTeardown();

  return check((A(0, 0)(0) == 17.f) && (A(1, 3)(0) == 36.f) && (A(2, 2)(0) == 31.f) &&
               (A(3, 3)(0) == 48.f) && 
               (A(0, 0)(1) == 0.f) && (A(1, 3)(1) == 40.f) && (A(2, 2)(1) == 0.f) &&
               (A(3, 3)(1) == 0.f));
}
