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

#define N 5

int test1()
{
  HPL::Array<double, 2> A(N,N);
  HPL::Array<double, 1> x(N);
  
  HPL_clblasSetup();

  for(int i = 0; i < N; i++) {
   for(int j = i; j < N; j++)
   {
    A(i,j) = j + 1;
   }
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  if (clblasDsyr( clblasUpper, x, A) != clblasSuccess)
      std::cout << "Failed on launch. \n";

  HPL_clblasTeardown();

  return check((A(0, 2) == 19.f) && (A(1, 3) == 68.f) && (A(2, 4) == 261.f) && (A(3, 3) == 260.f) && (A(4, 4) == 1029.f));
}

int test2()
{
  HPL::Array<double, 2> A(N,N);
  HPL::Array<double, 1> x(N);

  HPL_clblasSetup();

  for(int i = 0; i < N; i++) {
   for(int j = 0; j <= i; j++)
   {
    A(i,j) = i + 1;
   }
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  if (clblasDsyr( clblasLower, x, A) != clblasSuccess)
      std::cout << "Failed on launch. \n";

  HPL_clblasTeardown();

  return check((A(2, 0) == 19.f) && (A(3, 1) == 68.f) && (A(4, 2) == 261.f) && (A(3, 3) == 260.f) && (A(4, 4) == 1029.f));
}

int main()
{
    test1();
    test2();
}
