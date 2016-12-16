#include <iostream>
#include "HPL_clBLAS.h"

#define N 4
#define K 1
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
  HPL::Array<HPL::float2, 1> A(N*(K+1));
  HPL::Array<HPL::float2, 1> x(N);
  HPL::Array<HPL::float2, 1> y(N);
  cl_float2 alpha = {1, 0};
  cl_float2 beta  = {1, 0};
  
  for(int i = 0; i < N*(K+1); i++) {
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

  clblasChbmv(clblasRowMajor,
                       clblasUpper,
                       x.getDimension(0),
                       K,
                       alpha,
                       A,
                       0,
                       K+1,
                       x,
                       0,
                       1,
                       beta,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0)(0) == 3.f) && (y(1)(0) == 49.f) && (y(2)(0) == 153.f) &&
               (y(3)(0) == 169.f) && 
               (y(0)(1) == 25.f) && (y(1)(1) == 45.f) && (y(2)(1) == 81.f) &&
               (y(3)(1) == -3.f) );
}
