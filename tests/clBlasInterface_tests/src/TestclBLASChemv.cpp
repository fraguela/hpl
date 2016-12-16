#include <iostream>
#include "HPL_clBLAS.h"

#define M 4
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
  HPL::Array<HPL::float2, 2> A(M,M);
  HPL::Array<HPL::float2, 1> x(M);
  HPL::Array<HPL::float2, 1> y(M);
  cl_float2 alpha = {1, 0};
  cl_float2 beta  = {1, 0};
  
  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    A(i,j)(0) = j + 1;
    A(i,j)(1) = 4;
   }
  }

  for(int i = 0; i < M; i++) {
     x(i)(0) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasChemv(clblasRowMajor,
                       clblasUpper,
                       A.getDimension(0),
                       alpha,
                       A,
                       0,
                       getLowLevelDim1(A, HPL::Device()),
                       x,
                       0,
                       1,
                       beta,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0)(0) == 74.f) && (y(1)(0) == 92.f) && (y(2)(0) == 114.f) &&
               (y(3)(0) == 144.f) && 
               (y(0)(1) == 132.f) && (y(1)(1) == 110.f) && (y(2)(1) == 66.f) &&
               (y(3)(1) == -24.f));
}
