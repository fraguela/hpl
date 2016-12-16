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

int test1()
{
  HPL::Array<float, 1> AP((N*N+N)/2);
  HPL::Array<float, 1> x(N);
  
  for(int i = 0; i < (N*N+N)/2; i++) {
    AP(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  std::cout << std::endl;

  HPL_clblasSetup();

  clblasStpmv( clblasUpper, AP, x);

  HPL_clblasTeardown();

  return check((x(0) == 258.f) && (x(1) == 496.f) && (x(2) == 640.f) && (x(3) == 656.f) && (x(4) == 480.f));
}

int test2()
{
  HPL::Array<float, 1> AP((N * N + N) / 2);
  HPL::Array<float, 1> x(N);

  for(int i = 0; i < (N*N+N)/2; i++) {
    AP(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  clblasStpmv( clblasLower, AP, x);

  HPL_clblasTeardown();

  return check((x(0) == 2.f) && (x(1) == 16.f) && (x(2) == 76.f) && (x(3) == 278.f) && (x(4) == 878.f));
}

int main()
{
    test1();
    test2();
}
