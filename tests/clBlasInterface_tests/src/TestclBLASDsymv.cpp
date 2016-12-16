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
   y(i) = 0.f;
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  try { clblasDsymv(clblasUpper, A, x, y); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }
  
  HPL_clblasTeardown();

  return check((y(0) == 258.f) && (y(1) == 260.f) && (y(2) == 266.f) && (y(3) == 280.f) && (y(4) == 310.f));
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
   y(i) = 0.f;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  try { clblasDsymv(clblasLower, A, x, y); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check((y(0) == 258.f) && (y(1) == 260.f) && (y(2) == 266.f) && (y(3) == 280.f) && (y(4) == 310.f));
}

int main()
{
    test1();
    test2();
}

