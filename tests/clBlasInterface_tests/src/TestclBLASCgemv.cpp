#include <iostream>
#include "HPL_clBLAS.h"

#define M 4
#define N 5
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
  HPL::Array<HPL::float2, 2> A(M,N);
  HPL::Array<HPL::float2, 1> x(N);
  HPL::Array<HPL::float2, 1> y(M);
  cl_float2 alpha = {1, 1};
  cl_float2 beta  = {1, 1};

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j)(0) = N*i + j + 1;
    A(i,j)(1) = 1;
   }
   y(i)(0) = 1.f;
   y(i)(1) = 3.f;
  }
  
  for(int i = 0; i < N; i++) {
     x(i)(0) = 1 << i + 1;
     x(i)(1) = 2;
  }

  HPL_clblasSetup();

  clblasCgemv(clblasRowMajor,
		clblasNoTrans,
		A.getDimension(0),
		A.getDimension(1),
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

  return check((y(0)(0) == 154.f) && (y(1)(0) == 414.f) && (y(2)(0) == 674.f) && (y(3)(0) == 934.f) &&
               (y(0)(1) == 344.f) && (y(1)(1) == 704.f) && (y(2)(1) == 1064.f) && (y(3)(1) == 1424.f));

}
