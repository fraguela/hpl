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

  clblasCtbmv(clblasUpper, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  for(int i = 0; i < N; i++)
      std::cout << x(i)(0) << ',' << x(i)(1) << ' ';

  return check((x(0)(0) == -2.f) && (x(1)(0) == 32.f) && (x(2)(0) == 124.f) && (x(3)(0) == 106.f) &&
               (x(0)(1) == 24.f) && (x(1)(1) == 50.f) && (x(2)(1) == 94.f) && (x(3)(1) == 62.f));
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

  clblasCtbmv(clblasLower, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  for(int i = 0; i < N; i++)
      std::cout << x(i)(0) << ',' << x(i)(1) << ' ';

  return check((x(0)(0) == -2.f) && (x(1)(0) == 10.f) && (x(2)(0) == 56.f) && (x(3)(0) == 172.f) &&
               (x(0)(1) == 10.f) && (x(1)(1) == 32.f) && (x(2)(1) == 58.f) && (x(3)(1) == 102.f));
}

int main()
{
    test1();
    test2();
}
