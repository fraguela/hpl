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

  clblasCtpsv( clblasUpper, AP, x);

  HPL_clblasTeardown();

  return check(tol(x(0)(0), 0.18198f) && tol(x(1)(0), -0.35586f) && tol(x(2)(0), -0.56375f) && 
               tol(x(3)(0), -1.04521f) && tol(x(4)(0), 2.11354f) && tol(x(0)(1), 0.79355f) &&
               tol(x(1)(1), 0.17554f) && tol(x(2)(1), 0.11293f) && tol(x(3)(1), 0.14938f) &&
               tol(x(4)(1), -0.14847f));
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

  clblasCtpsv(clblasLower, AP, x);

  HPL_clblasTeardown();

  return check(tol(x(0)(0), 1.2f) && tol(x(1)(0), 0.24615f) && tol(x(2)(0), 0.20923f) && 
               tol(x(3)(0), 0.31231f) && tol(x(4)(0), 0.54339f) && tol(x(0)(1), -0.4f) &&
               tol(x(1)(1), -0.03077f) && tol(x(2)(1), 0.07385f) && tol(x(3)(1), 0.04462f) &&
               tol(x(4)(1), 0.01083f));
}

int main()
{
    test1();
    test2();
}
