#include <iostream>
#include "HPL_clBLAS.h"

#define N 5
#define K 4

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
  HPL::Array<float, 2> A(N, K);
  HPL::Array<float, 2> C(N, N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j < K; j++)
   {
    A(i,j) = i*K + j + 1;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    C(i,j) = j + 1;
   }
  }

  HPL_clblasSetup();

  clblasSsyrk(clblasUpper, A, C);

  HPL_clblasTeardown();

  return check((C(0, 2) == 113.f) && (C(1, 3) == 386.f) && (C(2, 4) == 787.f) && (C(3, 3) == 850.f) && (C(4, 4) == 1379.f));
}

int test2()
{
  HPL::Array<float, 2> A(N, K);
  HPL::Array<float, 2> C(N, N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j < K; j++)
   {
    A(i,j) = i*K + j + 1;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    C(i,j) = j + 1;
   }
  }

  HPL_clblasSetup();

  clblasSsyrk(clblasLower, A, C);

  HPL_clblasTeardown();

  return check((C(0, 0) == 31.f) && (C(1, 1) == 176.f) && (C(2, 0) == 111.f) && (C(3, 2) == 617.f) && (C(4, 3) == 1082.f));
}

int main()
{
    test1();
    test2();
}
