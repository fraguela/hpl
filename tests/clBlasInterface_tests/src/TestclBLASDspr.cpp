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
  HPL::Array<double, 1> AP((N * N + N) / 2);
  HPL::Array<double, 1> x(N);
  


  for(int i = 0; i < (N*N+N)/2; i++) {
    AP(i) = i + 1;
  }
  
  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  try { clblasDspr(clblasUpper, x, AP); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }
  
  HPL_clblasTeardown();

  return check((AP(0) == 5.f) && (AP(4) == 69.f) && (AP(7) == 72.f) && (AP(10) == 139.f) && (AP(14) == 1039.f));
}

int test2()
{
  HPL::Array<double, 1> AP((N * N + N) / 2);
  HPL::Array<double, 1> x(N);

  for(int i = 0; i < (N*N+N)/2; i++) {
    AP(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  HPL_clblasSetup();

  try { clblasDspr(clblasLower, x, AP); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check((AP(0) == 5.f) && (AP(4) == 37.f) && (AP(7) == 72.f) && (AP(10) == 75.f) && (AP(14) == 1039.f));
}

int main()
{
    test1();
    test2();
}
