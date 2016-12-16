#include <iostream>
#include "HPL_clBLAS.h"

#define M 5
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

int test1()
{
  HPL::Array<HPL::float2, 2> A(M, M);
  HPL::Array<HPL::float2, 2> B(M, N);
  HPL::Array<HPL::float2, 2> C(M, N);
  cl_float2 alpha = {1, 0};
  cl_float2 beta  = {1, 0};

  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    A(i,j)(0) = j + 1;
    A(i,j)(1) = 2;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j)(0) = i*N + j + 1;
    B(i,j)(1) = 3;
    C(i,j)(0) = i*N + j + 1;
    C(i,j)(1) = 4;
   }
  }

  HPL_clblasSetup();

  clblasCsymm(clblasRowMajor,
                       clblasLeft,
                       clblasUpper,
                       B.getDimension(0),
                       B.getDimension(1),
                       alpha,
                       A,
                       0,
                       B.getDimension(0),
                       B,
                       0,
                       B.getDimension(1),
                       beta,
                       C,
                       0,
                       C.getDimension(1));

  HPL_clblasTeardown();

  return check((C(0, 2)(0) == 178.f) && (C(1, 3)(0) == 202.f) && (C(2, 0)(0) == 161.f) && 
               (C(3, 3)(0) == 246.f) && (C(4, 3)(0) == 290.f) &&
               (C(0, 2)(1) == 159.f) && (C(1, 3)(1) == 172.f) && (C(2, 0)(1) == 148.f) && 
               (C(3, 3)(1) == 187.f) && (C(4, 3)(1) == 199.f));
}

int test2()
{
  HPL::Array<HPL::float2, 2> A(M, M);
  HPL::Array<HPL::float2, 2> B(M, N);
  HPL::Array<HPL::float2, 2> C(M, N);
  cl_float2 alpha = {1, 0};
  cl_float2 beta  = {1, 0};

  for(int i = 0; i < M; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j)(0) = j + 1;
    A(i,j)(1) = 2;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j)(0) = i*N + j + 1;
    B(i,j)(1) = 3;
    C(i,j)(0) = i*N + j + 1;
    C(i,j)(1) = 4;
   }
  }

  HPL_clblasSetup();

  clblasCsymm(clblasRowMajor,
                       clblasLeft,
                       clblasLower,
                       B.getDimension(0),
                       B.getDimension(1),
                       alpha,
                       A,
                       0,
                       B.getDimension(0),
                       B,
                       0,
                       B.getDimension(1),
                       beta,
                       C,
                       0,
                       C.getDimension(1));

  HPL_clblasTeardown();

  return check((C(0, 2)(0) == 28.f) && (C(1, 3)(0) == 94.f) && (C(2, 0)(0) == 107.f) && 
               (C(3, 3)(0) == 186.f) && (C(4, 3)(0) == 210.f) &&
               (C(0, 2)(1) == 129.f) && (C(1, 3)(1) == 151.f) && (C(2, 0)(1) == 130.f) && 
               (C(3, 3)(1) == 166.f) && (C(4, 3)(1) == 169.f));
}

int main()
{
    test1();
    test2();
}
