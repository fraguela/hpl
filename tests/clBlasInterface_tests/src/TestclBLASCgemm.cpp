#include <iostream>
#include "HPL_clBLAS.h"

#define M 4
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
  HPL::Array<HPL::float2, 2> A(M,N);
  HPL::Array<HPL::float2, 2> B(N,M);
  HPL::Array<HPL::float2, 2> C(M,M);
  
  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j)(0) = N*i + j + 1;
    A(i,j)(1) = 1;
   }
  }
  
  for(int i = 0; i < N; i++) {
   for(int j = 0; j < M; j++)
   {
    B(i,j)(0) = M*i + j + 1;
    B(i,j)(1) = 2;
   }
  }

  HPL_clblasSetup();

  clblasCgemm( A, B, C);
  
  HPL_clblasTeardown();

  return check((C(0,0)(0) == 165.f) && (C(1,2)(0) == 470.f) && (C(2,3)(0) == 810.f) && (C(3,1)(0) == 930.f) &&
               (C(0,0)(1) == 75.f) && (C(1,2)(1) == 135.f) && (C(2,3)(1) == 190.f) && (C(3,1)(1) == 230.f));

}
