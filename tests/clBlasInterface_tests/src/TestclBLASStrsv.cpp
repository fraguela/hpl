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
  HPL::Array<float, 2> A(N,N);
  HPL::Array<float, 1> x(N);
  
  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    A(i,j) = j + 1;
   }
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  clblasStrsv(clblasUpper, clblasNonUnit, A, x);
  
  HPL_clblasTeardown();
  
  return check(tol(x(0), -2.f) && (tol(x(1), -2.f)) && (tol(x(2), -2.666f)) && tol(x(3), -4.f) && tol(x(4), 6.4f));
}

int test2()
{
  HPL::Array<float, 2> A(N,N);
  HPL::Array<float, 1> x(N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = i + 1;
   }
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  clblasStrsv(clblasLower, clblasNonUnit, A, x);

  HPL_clblasTeardown();

  return check(tol(x(0), 2.f) && tol(x(1), 0.f) && tol(x(2), 0.666f) && tol(x(3),  1.3333f) && tol(x(4),2.4f));
}

int main()
{
    test1();
    test2();
}

