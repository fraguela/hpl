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
  HPL::Array<float, 1> y(N);
  
  for(int i = 0; i < N*(K+1); i++) {
    A(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  clblasSsbmv(clblasRowMajor,
                       clblasUpper,
                       x.getDimension(0),
                       K,
                       1,
                       A,
                       0,
                       K+1,
                       x,
                       0,
                       1,
                       1,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0) == 14.f) && (y(1) == 56.f) && (y(2) == 168.f) && (y(3) == 192.f));
}

int test2()
{
  HPL::Array<float, 1> A(N*(K+1));
  HPL::Array<float, 1> x(N);
  HPL::Array<float, 1> y(N);

  for(int i = 0; i < N*(K+1); i++) {
    A(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  clblasSsbmv(clblasRowMajor,
                       clblasLower,
                       x.getDimension(0),
                       K,
                       1,
                       A,
                       0,
                       K+1,
                       x,
                       0,
                       1,
                       1,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0) == 20.f) && (y(1) == 70.f) && (y(2) == 196.f) && (y(3) == 216.f));
}

int main()
{
    test1();
    test2();
}
