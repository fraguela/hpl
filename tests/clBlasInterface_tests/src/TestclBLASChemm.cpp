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
  HPL::Array<HPL::float2, 2> A(M,M);
  HPL::Array<HPL::float2, 2> B(M,N);
  HPL::Array<HPL::float2, 2> C(M,N);
  
  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    A(i,j)(0) = j + 1;
    A(i,j)(1) = 4;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j)(0) = i*N + j + 1;
    B(i,j)(1) = 2;
   }
  }

  HPL_clblasSetup();

  (clblasChemm( clblasLeft, clblasUpper, A, B, C) != clblasSuccess);

  HPL_clblasTeardown();

  return check((C(0, 0)(0) == 143.f) && (C(1, 3)(0) == 208.f) && (C(2, 2)(0) == 218.f) &&
               (C(3, 3)(0) == 276.f) && 
               (C(0, 0)(1) == 206.f) && (C(1, 3)(1) == 208.f) && (C(2, 2)(1) == 132.f) &&
               (C(3, 3)(1) == 26.f));
}
