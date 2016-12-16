#include <iostream>
#include "HPL_clBLAS.h"

#define N 5

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
  HPL::Array<HPL::float2, 1> A((N*N+N)/2);
  HPL::Array<HPL::float2, 1> x(N);
  
  for(int i = 0; i < (N*N+N)/2; i++) {
    A(i)(0) = i + 1;
    A(i)(1) = 4;
  }

  for(int i = 0; i < N; i++) {
   x(i)(0) = 1 << i + 1;
   x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasChpr(clblasUpper, x, A);

  HPL_clblasTeardown();

  return check((A(0)(0) == 9.f) && (A(4)(0) == 73.f) && (A(6)(0) == 43.f) &&
               (A(12)(0) == 273.f) && 
               (A(0)(1) == 0.f) && (A(4)(1) == 64.f) && (A(6)(1) == 12.f) &&
               (A(12)(1) == 0.f));
}
