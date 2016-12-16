#include <iostream>
#include "HPL_clBLAS.h"

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

#define M 5
#define N 4

int test1()
{
  HPL::Array<double, 2> A(M, M);
  HPL::Array<double, 2> B(M, N);
  HPL::Array<double, 2> C(M, N);

  for(int i = 0; i < M; i++) {
   for(int j = i; j < M; j++)
   {
    A(i,j) = j + 1;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j) = i*N + j + 1;
    C(i,j) = i*N + j + 1;
   }
  }

  HPL_clblasSetup();

  clblasDsymm(clblasRowMajor,
                       clblasLeft,
                       clblasUpper,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       B.getDimension(0),
                       B,
                       0,
                       B.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(1));

  HPL_clblasTeardown();

  return check((C(0, 2) == 208.f) && (C(1, 3) == 232.f) && (C(2, 3) == 248.f) && (C(3, 3) == 276.f) && (C(4, 3) == 320.f));
}

int test2()
{
  HPL::Array<double, 2> A(M, M);
  HPL::Array<double, 2> B(M, N);
  HPL::Array<double, 2> C(M, N);

  for(int i = 0; i < M; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = j + 1;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j) = i*N + j + 1;
    C(i,j) = i*N + j + 1;
   }
  }

  HPL_clblasSetup();

  clblasDsymm(clblasRowMajor,
                       clblasLeft,
                       clblasLower,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       B.getDimension(0),
                       B,
                       0,
                       B.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(1));

  HPL_clblasTeardown();

  return check((C(0, 0) == 46.f) && (C(1, 1) == 104.f) && (C(2, 0) == 137.f) && (C(3, 2) == 201.f) && (C(4, 3) == 240.f));
}

int main()
{
    test1();
    test2();
}
