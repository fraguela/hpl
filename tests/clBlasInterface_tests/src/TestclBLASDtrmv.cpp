#include <iostream>
#include "HPL_clBLAS.h"

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

int test1()
{
  HPL::Array<double, 2> A(N,N);
  HPL::Array<double, 1> x(N);
  


  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    A(i,j) = j + 1;
   }
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  try { clblasDtrmv(clblasUpper, clblasNonUnit, A, x); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }
  
  HPL_clblasTeardown();

  return check((x(0) == 258.f) && (x(1) == 256.f) && (x(2) == 248.f) && (x(3) == 224.f) && (x(4) == 160.f));
}

int test2()
{
  HPL::Array<double, 2> A(N,N);
  HPL::Array<double, 1> x(N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = i + 1;
   }
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  try { clblasDtrmv(clblasLower, clblasNonUnit, A, x); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check((x(0) == 2.f) && (x(1) == 12.f) && (x(2) == 42.f) && (x(3) == 120.f) && (x(4) == 310.f));
}

int main()
{
    test1();
    test2();
}

