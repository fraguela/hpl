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

int test1()
{
  HPL::Array<HPL::float2, 2> A(M,M);
  HPL::Array<HPL::float2, 2> B(M,N);
  
  for(int i = 0; i < M; i++) {
   for(int j = 0; j < M; j++)
   {
    A(i,j)(0) = 0;
    A(i,j)(1) = 0;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    A(i,j)(0) = j + 1;
    A(i,j)(1) = 2;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j)(0) = i*N + j + 1;
    B(i,j)(1) = 3;
   }
  }

  HPL_clblasSetup();
  
  clblasCtrmm( clblasLeft, clblasUpper, clblasNonUnit, A, B);
  
  HPL_clblasTeardown();
 
  return check((B(0, 0)(0) == 86.f) && (B(1, 2)(0) == 109.f) && (B(2, 1)(0) == 92.f) && (B(3,3)(0) == 70.f) && (B(0, 0)(1) == 98.f) && (B(1, 2)(1) == 105.f) && (B(2, 1)(1) == 79.f) && (B(3,3)(1) == 50.f));
}

int test2()
{
  HPL::Array<HPL::float2, 2> A(N,N);
  HPL::Array<HPL::float2, 2> B(M,N);
  
  for(int i = 0; i < N; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j)(0) = 0;
    A(i,j)(1) = 0;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j)(0) = i + 1;
    A(i,j)(1) = 2;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j)(0) = i*N + j + 1;
    B(i,j)(1) = 3;
   }
  }

  HPL_clblasSetup();
  
  clblasCtrmm( clblasRight, clblasLower, clblasNonUnit, A, B);
  
  HPL_clblasTeardown();

  return check((B(0, 0)(0) == 25.f) && (B(1, 2)(0) == 92.f) && (B(2, 1)(0) == 170.f) && (B(3,3)(0) == 164.f) && (B(0, 0)(1) == 75.f) && (B(1, 2)(1) == 90.f) && (B(2, 1)(1) == 150.f) && (B(3,3)(1) == 105.f));
}

int main()
{
    test1();
    test2();
}

