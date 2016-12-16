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

#define N 5

int test1()
{
  HPL::Array<HPL::float2, 2> A(N,N);
  HPL::Array<HPL::float2, 1> x(N);
  


  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    A(i,j)(0) = j + 1;
    A(i,j)(1) = 2;
   }
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtrmv(clblasUpper, clblasNonUnit, A, x);
  
  HPL_clblasTeardown();

  return check((x(0)(0) == 238.f) && (x(1)(0) == 240.f) && (x(2)(0) == 236.f) && 
               (x(3)(0) == 216.f) && (x(4)(0) == 156.f) && (x(0)(1) == 154.f) &&
               (x(1)(1) == 148.f) && (x(2)(1) == 136.f) && (x(3)(1) == 114.f) &&
               (x(4)(1) == 74.f));
}

int test2()
{
  HPL::Array<HPL::float2, 2> A(N,N);
  HPL::Array<HPL::float2, 1> x(N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j)(0) = i + 1;
    A(i,j)(1) = 3;
   }
  }

  for(int i = 0; i < N; i++) {
     x(i)(0) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtrmv(clblasLower, clblasNonUnit, A, x);

  HPL_clblasTeardown();

  return check((x(0)(0) == -4.f) && (x(1)(0) == 0.f) && (x(2)(0) == 24.f) && 
               (x(3)(0) == 96.f) && (x(4)(0) == 280.f) && (x(0)(1) == 8.f) &&
               (x(1)(1) == 26.f) && (x(2)(1) == 60.f) && (x(3)(1) == 122.f) &&
               (x(4)(1) == 236.f));
}

int main()
{
    test1();
    test2();
}

