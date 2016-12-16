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
  HPL::Array<float, 1> A(N*(K+1));
  HPL::Array<float, 1> x(N);
  
  for(int i = 0; i < N*(K+1); i++) {
    A(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  clblasStbmv(clblasUpper, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  return check((x(0) == 10.f) && (x(1) == 44.f) && (x(2) == 136.f) && (x(3) == 112.f));
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

  clblasStbmv(clblasLower, clblasNonUnit, K, A, x);

  HPL_clblasTeardown();

  return check((x(0) == 4.f) && (x(1) == 22.f) && (x(2) == 68.f) && (x(3) == 184.f));
}

int main()
{
    test1();
    test2();
}
