#include <iostream>
#include "HPL_clBLAS.h"

#define M 4
#define N 5

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

bool tol(double a, double b)
{
    return abs(a - b) < 0.0001;
}

int test1()
{
  HPL::Array<double, 2> A(M,M);
  HPL::Array<double, 2> B(M,N);

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < M; j++)
   {
    A(i,j) = 0;
   }
  }

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
   }
  }

  HPL_clblasSetup();
  
  try { clblasDtrsm(clblasLeft, clblasUpper, clblasNonUnit, A, B); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }
  
  HPL_clblasTeardown();
  
  return check(tol(B(0, 0), -5.f) && tol(B(1, 2), -2.5f) && tol(B(2, 1), -1.6667) && tol(B(3,3), 4.7f));
}

int test2()
{
  HPL::Array<double, 2> A(N,N);
  HPL::Array<double, 2> B(M,N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j) = 0;
   }
  }

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = j + 1;
   }
  }

  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    B(i,j) = i*N + j + 1;
   }
  }

  HPL_clblasSetup();

  try { clblasDtrsm(clblasRight, clblasLower, clblasNonUnit, A, B); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }
  
  HPL_clblasTeardown();

  return check(tol(B(0, 0), 0.f) && tol(B(1, 2), 0.41667f) && tol(B(2, 1), 1.66667f) && tol(B(3,3), 0.75f));
}

int main()
{
    test1();
    test2();
}

