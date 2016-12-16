#include <iostream>
#include "HPL_clBLAS.h"

#define M 4
#define N 3
#define KU 2
#define KL 1

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

int main()
{
  HPL::Array<double, 1> A(M*(KL+KU+1));
  HPL::Array<double, 1> x(N);
  HPL::Array<double, 1> y(M);

  for(int i = 0; i < M*(KL+KU+1); i++) {
    A(i) = i + 1;
  }

  for(int i = 0; i < N; i++) {
     x(i) = 1 << i + 1;
  }

  for(int i = 0; i < M; i++) {
     y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();

  clblasDgbmv(clblasRowMajor,
                       clblasNoTrans,
                       y.getDimension(0),
                       x.getDimension(0),
                       KL,
                       KU,
                       1,
                       A,
                       0,
                       KL+KU+1,
                       x,
                       0,
                       1,
                       1,
                       y,
                       0,
                       1);

  HPL_clblasTeardown();

  return check((y(0) == 52.f) && (y(1) == 98.f) && (y(2) == 132.f) && (y(3) == 136.f));
}
