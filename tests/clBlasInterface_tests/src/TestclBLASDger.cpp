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
#define M 4
#define N 5

int main()
{
  HPL::Array<double, 2> A(M,N);
  HPL::Array<double, 1> x(M);
  HPL::Array<double, 1> y(N);
  
  for(int i = 0; i < M; i++) {
   for(int j = 0; j < N; j++)
   {
    A(i,j) = N*i + j + 1;
   }
  }

  for(int i = 0; i < M; i++) {
   x(i) = 1 << i + 1;
  }

  for(int i = 0; i < N; i++) {
   y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  try { clblasDger(x, y, A); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check((A(0, 0) == 9.f) && (A(0, 3) == 68.f) && (A(1, 4) == 266.f) &&
               (A(3,3) == 531.f));
}
