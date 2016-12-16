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
  HPL::Array<float, 2> A(M,N);
  HPL::Array<float, 1> x(N);
  HPL::Array<float, 1> y(M);
  
  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j) = N*i + j + 1;
   }
   y(i) = 0.f;
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  clblasSgemv(clblasRowMajor,
                       clblasNoTrans,
                       A.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, HPL::Device()),
                       x,
                       0,
                       1,
                       1,
                       y,
                       0,
                       1);
  
  HPL_clblasTeardown();

  return check((y(0) == 258.f) && (y(1) == 568.f) && (y(2) == 878.f) && (y(3) == 1188.f));

}
