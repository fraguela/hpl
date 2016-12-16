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
{
  HPL::Array<HPL::float2, 1> A((N*N+N)/2);
  HPL::Array<HPL::float2, 1> x(N);
  HPL::Array<HPL::float2, 1> y(N);
  
  for(int i = 0; i < (N*N+N)/2; i++) {
    A(i)(0) = i + 1;
    A(i)(1) = 4;
  }

  for(int i = 0; i < N; i++) {
   x(i)(0) = 1 << i + 1;
   x(i)(1) = 2;
  }

  for(int i = 0; i < N; i++) {
   y(i)(0) = 1;
   y(i)(1) = 3;
  }

  HPL_clblasSetup();

  clblasChpr2(clblasUpper, x, y, A);

  HPL_clblasTeardown();

  return check((A(0)(0) == 17.f) && (A(4)(0) == 25.f) && (A(6)(0) == 39.f) &&
               (A(8)(0) == 45.f) && 
               (A(0)(1) == 0.f) && (A(4)(1) == 0.f) && (A(6)(1) == 40.f) &&
               (A(8)(1) == 28.f));
}
