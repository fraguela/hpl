#include <iostream>
#include "HPL_clBLAS.h"

#define N 4
#define is_subb(x,d) (x.getFather()!=NULL && x.getFather()->hdm.get_dbuffer(d.getPlatformId(), d.getDeviceType(), d.getDeviceNumber()) !=NULL)
#define getLowLevelDim1(x,d) (is_subb(x,d) ? x.getTX().end : x.getDimension(1))

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

int main()
{
  HPL::Array<HPL::float2, 1> A((N*N+N)/2);
  HPL::Array<HPL::float2, 1> x(N);
  HPL::Array<HPL::float2, 1> y(N);
  cl_float2 alpha = {1, 0};
  cl_float2 beta  = {1, 0};
  
  for(int i = 0; i < (N*N+N)/2; i++) {
    A(i)(0) = i + 1;
    A(i)(1) = 4;
  }

  for(int i = 0; i < N; i++) {
   x(i)(0) = 1 << i + 1;
   x(i)(1) = 2;
  }

  for(int i = 0; i < N; i++) {
   y(i)(0) = 1;
   y(i)(1) = 3;
  }

  HPL_clblasSetup();

  clblasChpmv(clblasRowMajor,
                       clblasUpper,
                       x.getDimension(0),
                       alpha,
                       A,
                       0,
                       x,
                       0,
                       1,
                       beta,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0)(0) == 75.f) && (y(1)(0) == 177.f) && (y(2)(0) == 247.f) &&
               (y(3)(0) == 293.f) && 
               (y(0)(1) == 135.f) && (y(1)(1) == 131.f) && (y(2)(1) == 95.f) &&
               (y(3)(1) == 7.f));
}
