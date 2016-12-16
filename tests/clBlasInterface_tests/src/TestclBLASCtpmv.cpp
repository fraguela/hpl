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
  HPL::Array<HPL::float2, 1> AP((N*N+N)/2);
  HPL::Array<HPL::float2, 1> x(N);

  for(int i = 0; i < (N*N+N)/2; i++) {
    AP(i)(0) = i + 1;
    AP(i)(1) = 2; 
  }

  for(int i = 0; i < N; i++) {
     x(i)(0) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtpmv(clblasUpper, AP, x);

  HPL_clblasTeardown();

  for(int i = 0; i < N; i++)
      std::cout << x(i)(0) << ',' << x(i)(1) << ' ';

  std::cout << std::endl;

  return check((x(0)(0) == 238.f) && (x(1)(0) == 480.f) && (x(2)(0) == 628.f) && 
               (x(3)(0) == 648.f) && (x(4)(0) == 476.f) && (x(0)(1) == 154.f) &&
               (x(1)(1) == 180.f) && (x(2)(1) == 178.f) && (x(3)(1) == 150.f) &&
               (x(4)(1) == 94.f));
}

int test2()
{
  HPL::Array<HPL::float2, 1> AP((N * N + N) / 2);
  HPL::Array<HPL::float2, 1> x(N);

  for(int i = 0; i < (N*N+N)/2; i++) {
    AP(i) = i + 1;
    AP(i)(1) = 2; 
  }

  for(int i = 0; i < N; i++) {
     x(i)(0) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCtpmv(clblasLower, AP, x);

  HPL_clblasTeardown();

  for(int i = 0; i < N; i++)
      std::cout << x(i)(0) << ',' << x(i)(1) << ' ';

  std::cout << std::endl;

  return check((x(0)(0) == -2.f) && (x(1)(0) == 8.f) && (x(2)(0) == 64.f) && 
               (x(3)(0) == 262.f) && (x(4)(0) == 858.f) && (x(0)(1) == 6.f) &&
               (x(1)(1) == 22.f) && (x(2)(1) == 58.f) && (x(3)(1) == 128.f) &&
               (x(4)(1) == 254.f));
}

int main()
{
    test1();
    test2();
}
