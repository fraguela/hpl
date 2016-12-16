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
  HPL::Array<double, 1> y(N);
  
  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    A(i,j) = j + 1;
   }
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  try { clblasDsyr2( clblasUpper, x, y, A); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();
  
  return check((A(0, 0) == 17.f) && (A(1, 2) == 131.f) && (A(2, 3) == 516.f) && (A(3, 4) == 2053.f) && (A(4, 4) == 4101.f));
}

int test2()
{
  HPL::Array<double, 2> A(N,N);
  HPL::Array<double, 1> x(N);
  HPL::Array<double, 1> y(N);

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = i + 1;
   }
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
     y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  try { clblasDsyr2(clblasLower, x, y, A); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check((A(0, 0) == 17.f) && (A(2, 1) == 131.f) && (A(3, 2) == 516.f) && (A(4, 3) == 2053.f) && (A(4, 4) == 4101.f));
}

int main()
{
    test1();
    test2();
}

