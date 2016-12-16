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

  HPL_clblasSetup();

  clblasCher(clblasUpper, x, A);

  HPL_clblasTeardown();

  return check((A(0, 0)(0) == 9.f) && (A(1, 3)(0) == 72.f) && (A(2, 2)(0) == 71.f) &&
               (A(4, 4)(0) == 1033.f) && 
               (A(0, 0)(1) == 0.f) && (A(1, 3)(1) == 28.f) && (A(2, 2)(1) == 0.f) &&
               (A(4, 4)(1) == 0.f));
}
