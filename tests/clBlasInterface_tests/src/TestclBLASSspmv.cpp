#include <iostream>
#include "HPL_clBLAS.h"

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

int test1()
{
  HPL::Array<float, 1> A((N * N + N) / 2);
  HPL::Array<float, 1> x(N);
  HPL::Array<float, 1> y(N);

  for(int i = 0; i < (N*N+N)/2; i++) {
    A(i) = i + 1;
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  clblasSspmv(clblasRowMajor,
                       clblasUpper,
                       x.getDimension(0),
                       1,
                       A,
                       0,
                       x,
                       0,
                       1,
                       1,
                       y,
                       0,
                       1);
  
  HPL_clblasTeardown();

  return check((y(0) == 262.f) && (y(1) == 508.f) && (y(2) == 690.f) && (y(3) == 816.f) && (y(4) == 910.f));
}

int test2()
{
  HPL::Array<float, 1> A((N * N + N) / 2);
  HPL::Array<float, 1> x(N);
  HPL::Array<float, 1> y(N);

  for(int i = 0; i < (N*N+N)/2; i++) {
    A(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  clblasSspmv(clblasRowMajor,
                       clblasLower,
                       x.getDimension(0),
                       1,
                       A,
                       0,
                       x,
                       0,
                       1,
                       1,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0) == 510.f) && (y(1) == 576.f) && (y(2) == 652.f) && (y(3) == 758.f) && (y(4) == 942.f));
}

int main()
{
    test1();
    test2();
}
