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

bool tol(float a, float b)
{
    return abs(a - b) < 0.0001;
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
  
  clblasCtrsm(clblasLeft, clblasUpper, clblasNonUnit, A, B);
  
  HPL_clblasTeardown();
 
  return check(tol(B(0, 0)(0), -1.f) && tol(B(1, 2)(0), -1.25f) && tol(B(2, 1)(0), -1.1538f) && 
               tol(B(3,3)(0), 4.1f) && tol(B(0, 0)(1), 2.f) && tol(B(1, 2)(1), 1.25f) && 
               tol(B(2, 1)(1), 0.7692f) && tol(B(3,3)(1), -1.3f));
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
  
  clblasCtrsm(clblasRight, clblasLower, clblasNonUnit, A, B);
  
  HPL_clblasTeardown();

  return check(tol(B(0, 0)(0), -0.2f) && tol(B(1, 2)(0), -0.2377f) && tol(B(2, 1)(0), -0.25f) && 
               tol(B(3,3)(0), -0.23077f) && tol(B(0, 0)(1), 0.4f) && tol(B(1, 2)(1), 0.25f) && 
               tol(B(2, 1)(1), 0.25f) && tol(B(3,3)(1), 0.15385f));
}

int main()
{
    test1();
    test2();
}
