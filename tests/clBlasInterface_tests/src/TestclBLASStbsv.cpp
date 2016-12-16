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
  HPL::Array<float, 1> A(N*(K+1));
  HPL::Array<float, 1> x(N);
  


  for(int i = 0; i < N*(K+1); i++) {
    A(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  clblasStbsv(clblasUpper, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  return check(tol(x(0), -3.7143f) && tol(x(1), 2.8571f) && tol(x(2), -1.1429f) && tol(x(3), 2.2857f));
}

int test2()
{
  HPL::Array<float, 1> A(N*(K+1));
  HPL::Array<float, 1> x(N);

  for(int i = 0; i < N*(K+1); i++) {
    A(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  clblasStbsv(clblasLower, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  return check(tol(x(0), 1.f) && tol(x(1), 0.25f) && tol(x(2), 1.125f) && tol(x(3), 1.01562f));
}

int main()
{
    test1();
    test2();
}
