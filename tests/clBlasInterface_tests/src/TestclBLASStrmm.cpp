#include <iostream>
#include "HPL_clBLAS.h"

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

#define M 4
#define N 5

int test1()
{
  HPL::Array<float, 2> A(M,M);
  HPL::Array<float, 2> B(M,N);
  
  for(int i = 0; i < M; i++) {
   for(int j = 0; j < M; j++)
   {
    A(i,j) = 0;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    A(i,j) = j + 1;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j) = i*N + j + 1;
   }
  }

  HPL_clblasSetup();
  
  clblasStrmm(clblasLeft, clblasUpper, clblasNonUnit, A, B);
  
  HPL_clblasTeardown();
  
  return check((B(0, 0) == 110.f) && (B(1, 2) == 127.f) && (B(2, 1) == 104.f) && (B(3,3) == 76.f));
}

int test2()
{
  HPL::Array<float, 2> A(N,N);
  HPL::Array<float, 2> B(M,N);
  
  for(int i = 0; i < N; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j) = 0;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = j + 1;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j) = i*N + j + 1;
   }
  }

  HPL_clblasSetup();
  
  clblasStrmm(clblasRight, clblasLower, clblasNonUnit, A, B);
  
  HPL_clblasTeardown();

  return check((B(0, 0) == 15.f) && (B(1, 2) == 81.f) && (B(2, 1) == 108.f) && (B(3,3) == 156.f));
}

int main()
{
    test1();
    test2();
}

