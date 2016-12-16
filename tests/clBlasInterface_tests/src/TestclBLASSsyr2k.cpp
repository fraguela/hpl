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
  HPL::Array<float, 2> B(N, K);
  HPL::Array<float, 2> C(N, N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j < K; j++)
   {
    A(i,j) = i*K + j + 1;
    B(i,j) = i*K + j + 1;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    C(i,j) = j + 1;
   }
  }

  HPL_clblasSetup();

  clblasSsyr2k(clblasUpper, A, B, C);

  HPL_clblasTeardown();

  return check((C(0, 2) == 223.f) && (C(1, 3) == 768.f) && (C(2, 4) == 1569.f) && (C(3, 3) == 1696.f) && (C(4, 4) == 2753.f));
}

int test2()
{
  HPL::Array<float, 2> A(N, K);
  HPL::Array<float, 2> B(N, K);
  HPL::Array<float, 2> C(N, N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j < K; j++)
   {
    A(i,j) = i*K + j + 1;
    B(i,j) = i*K + j + 1;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    C(i,j) = j + 1;
   }
  }

  HPL_clblasSetup();

  clblasSsyr2k(clblasLower, A, B, C);

  HPL_clblasTeardown();

  return check((C(0, 0) == 61.f) && (C(1, 1) == 350.f) && (C(2, 0) == 221.f) && (C(3, 2) == 1231.f) && (C(4, 3) == 2160.f));
}

int main()
{
    test1();
    test2();
}
