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
  HPL::Array<HPL::float2, 2> C(M,M);
  
  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    C(i,j)(0) = j + 1;
    C(i,j)(1) = 4;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j)(0) = i*N + j + 1;
    A(i,j)(1) = 2;
   }
  }

  HPL_clblasSetup();

  clblasCherk(clblasUpper, A, C);

  HPL_clblasTeardown();

  return check((C(0, 0)(0) == 47.f) && (C(1, 3)(0) == 402.f) && (C(2, 2)(0) == 465.f) &&
               (C(3, 3)(0) == 866.f) && 
               (C(0, 0)(1) == 0.f) && (C(1, 3)(1) == 68.f) && (C(2, 2)(1) == 0.f) &&
               (C(3, 3)(1) == 0.f));
}
