#include <iostream>
#include "HPL_clBLAS.h"

#define M 4
#define N 3
#define KU 2
#define KL 1
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
  HPL::Array<HPL::float2, 1> A(M*(KL+KU+1));
  HPL::Array<HPL::float2, 1> x(N);
  HPL::Array<HPL::float2, 1> y(M);
  cl_float2 alpha = {1, 1};
  cl_float2 beta  = {1, 1};

  for(int i = 0; i < M*(KL+KU+1); i++) {
    A(i)(0) = i + 1;
    A(i)(1) = 3;
  }

  for(int i = 0; i < N; i++) {
     x(i)(0) = 1 << i + 1;
     x(i)(1) = 2;
  }

  for(int i = 0; i < M; i++) {
     y(i)(0) = 1;
     y(i)(1) = 3;
  }

  HPL_clblasSetup();

  clblasCgbmv(clblasRowMajor,
                       clblasNoTrans,
                       y.getDimension(0),
                       x.getDimension(0),
                       KL,
                       KU,
                       alpha,
                       A,
                       0,
                       KL+KU+1,
                       x,
                       0,
                       1,
                       beta,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0)(0) == -32.f) && (y(1)(0) == -8.f) && (y(2)(0) == 28.f) && (y(3)(0) == 46.f) &&
               (y(0)(1) == 94.f) && (y(1)(1) == 154.f) && (y(2)(1) == 182.f) && (y(3)(1) == 152.f));
}
