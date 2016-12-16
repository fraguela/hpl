#include <iostream>
#include "HPL_clBLAS.h"

#define N 5
#define K 4
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
  HPL::Array<HPL::float2, 2> A(N, K);
  HPL::Array<HPL::float2, 2> C(N, N);
  cl_float2 alpha = {1, 0};
  cl_float2 beta  = {1, 0};

  for(int i = 0; i < N; i++) {
   for(int j = 0; j < K; j++)
   {
    A(i,j)(0) = i*K + j + 1;
    A(i,j)(1) = 2;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    C(i,j)(0) = j + 1;
    C(i,j)(1) = 3;
   }
  }

  HPL_clblasSetup();

  clblasCsyrk(clblasRowMajor,
                       clblasUpper,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       alpha,
                       A,
                       0,
                       A.getDimension(1),
                       beta,
                       C,
                       0,
                       C.getDimension(1));

  HPL_clblasTeardown();

  return check((C(0, 2)(0) == 97.f) && (C(1, 3)(0) == 370.f) && (C(2, 4)(0) == 771.f) && 
               (C(3, 3)(0) == 834.f) && (C(4, 4)(0) == 1363.f) &&
               (C(0, 2)(1) == 107.f) && (C(1, 3)(1) == 171.f) && (C(2, 4)(1) == 235.f) && 
               (C(3, 3)(1) == 235.f) && (C(4, 4)(1) == 299.f));
}

int test2()
{
  HPL::Array<HPL::float2, 2> A(N, K);
  HPL::Array<HPL::float2, 2> C(N, N);
  cl_float2 alpha = {1, 0};
  cl_float2 beta  = {1, 0};

  for(int i = 0; i < N; i++) {
   for(int j = 0; j < K; j++)
   {
    A(i,j)(0) = i*K + j + 1;
    A(i,j)(1) = 2;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    C(i,j)(0) = j + 1;
    C(i,j)(1) = 3;
   }
  }

  HPL_clblasSetup();

  clblasCsyrk(clblasRowMajor,
                       clblasLower,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       alpha,
                       A,
                       0,
                       A.getDimension(1),
                       beta,
                       C,
                       0,
                       C.getDimension(1));

  HPL_clblasTeardown();

  return check((C(2, 0)(0) == 95.f) && (C(3, 1)(0) == 368.f) && (C(4, 2)(0) == 769.f) && 
               (C(3, 3)(0) == 834.f) && (C(4, 4)(0) == 1363.f) &&
               (C(2, 0)(1) == 107.f) && (C(3, 1)(1) == 171.f) && (C(4, 2)(1) == 235.f) && 
               (C(3, 3)(1) == 235.f) && (C(4, 4)(1) == 299.f));
}

int main()
{
    test1();
    test2();
}
