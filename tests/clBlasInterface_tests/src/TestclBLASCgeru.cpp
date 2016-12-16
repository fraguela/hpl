#include <iostream>
#include "HPL_clBLAS.h"

#define M 5
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
  HPL::Array<HPL::float2, 2> A(M,N);
  HPL::Array<HPL::float2, 1> x(M);
  HPL::Array<HPL::float2, 1> y(N);
  
  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j)(0) = N*i + j + 1;
    A(i,j)(1) = 4;
   }
  }

  for(int i = 0; i < M; i++) {
   x(i)(0) = 1 << i + 1;
   x(i)(1) = 2;
  }

  for(int i = 0; i < N; i++) {
   y(i)(0) = 1;
   y(i)(1) = 3;
  }

  HPL_clblasSetup();

  (clblasCgeru(x, y, A) != clblasSuccess);

  HPL_clblasTeardown();

  return check((A(0, 0)(0) == -3.f) && (A(1, 3)(0) == 6.f) && (A(2, 2)(0) == 13.f) &&
               (A(4,3)(0) == 46.f) &&
               (A(0, 0)(1) == 12.f) && (A(1, 3)(1) == 18.f) && (A(2, 2)(1) == 30.f) &&
               (A(4,3)(1) == 102.f));
}
