#include <iostream>
#include "HPL_clBLAS.h"

#define N 4
#define K 1

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
  HPL::Array<HPL::float2, 1> A(N*(K+1));
  HPL::Array<HPL::float2, 1> x(N);
  
  for(int i = 0; i < N*(K+1); i++) {
    A(i)(0) = i + 1;
    A(i)(1) = 3;
  }

  for(int i = 0; i < N; i++) {
     x(i)(0) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtbsv(clblasUpper, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  for(int i = 0; i < N; i++)
      std::cout << x(i)(0) << ',' << x(i)(1) << ' ';

  std::cout << std::endl;

  return check(tol(x(0)(0), -1.11102f) && tol(x(1)(0), 2.01048f) && tol(x(2)(0), -0.92901f) && 
               tol(x(3)(0), 2.03448f) && tol(x(0)(1), 1.30500f) && tol(x(1)(1), -1.00169f) && 
               tol(x(2)(1), 0.44016f) && tol(x(3)(1), -0.58621f));
}

int test2()
{
  HPL::Array<HPL::float2, 1> A(N*(K+1));
  HPL::Array<HPL::float2, 1> x(N);

  for(int i = 0; i < N*(K+1); i++) {
    A(i)(0) = i + 1;
    A(i)(1) = 3;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtbsv( clblasLower, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  return check(tol(x(0)(0), 0.76912f) && tol(x(1)(0), 0.21538f) && tol(x(2)(0), 1.00513f) && 
               tol(x(3)(0), 0.93347f) && tol(x(0)(1), -0.15385f) && tol(x(1)(1), 0.12308f) && 
               tol(x(2)(1), 0.17436) && tol(x(3)(1), 0.32441f));
}

int main()
{
    test1();
    test2();
}
