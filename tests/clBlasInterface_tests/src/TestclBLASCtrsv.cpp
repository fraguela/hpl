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

bool tol(float a, float b)
{
    return abs(a - b) < 0.0001;
}

int test1()
{
  HPL::Array<HPL::float2, 2> A(N,N);
  HPL::Array<HPL::float2, 1> x(N);

  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    A(i,j) = j + 1;
    A(i,j)(1) = 2;
   }
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtrsv( clblasUpper, clblasNonUnit, A, x);
  
  HPL_clblasTeardown();

  return check(tol(x(0)(0), -0.4f) && tol(x(1)(0), -1.f) && tol(x(2)(0), -1.84615f) && 
               tol(x(3)(0), -3.2f) && tol(x(4)(0), 5.65517f) && tol(x(0)(1), 0.8) &&
               tol(x(1)(1), 1.f) && tol(x(2)(1), 1.23077f) && tol(x(3)(1), 1.6f) &&
               tol(x(4)(1), -1.86207f));
}

int test2()
{
  HPL::Array<HPL::float2, 2> A(N,N);
  HPL::Array<HPL::float2, 1> x(N);



  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = i + 1;
    A(i,j)(1) = 3;
   }
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtrsv(clblasLower, clblasNonUnit, A, x);

  HPL_clblasTeardown();

  return check(tol(x(0)(0), 0.8f) && tol(x(1)(0), 0.27692f) && tol(x(2)(0), 0.58974f) && 
               tol(x(3)(0), 1.13333f) && tol(x(4)(0), 2.08235f) && tol(x(0)(1), -0.4f) &&
               tol(x(1)(1), -0.21538f) && tol(x(2)(1), -0.38462f) && tol(x(3)(1), -0.6f) &&
               tol(x(4)(1), -0.92941f));
}

int main()
{
    test1();
    test2();
}

