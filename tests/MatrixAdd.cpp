#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

float a[N][N], b[N][N],c[N][N];

void display4(const Array<float, 2>& av, const char *name)
{
  std::cout << "Array<float, 2> " << name << ":\n";
  
  for(int i =0; i < 4; i++) {
    std::cout << av(i,0) << ' ' << av(i,1) << ' ' << av(i,2) << ' ' << av(i,3) << std::endl;
  }
}

void add1(Array<float, 2> a, Array<float, 2> b, Array<float, 2> c)
{
  a[idx][idy] =  b[idx][idy] + c[idx][idy];
}

void add2(Array<float, 2>& a, Array<float, 2> b, Array<float, 2> c)
{
  a[idx][idy] =  b[idx][idy] + c[idx][idy];
}

void add3(Array<float, 2>& a, const Array<float, 2>& b, const Array<float, 2>& c)
{
  a[idx][idy] =  b[idx][idy] + c[idx][idy];
}

void add4(Array<float, 2>& a, const Array<float, 2> b, const Array<float, 2> c)
{
  a[idx][idy] =  b[idx][idy] + c[idx][idy];
}

int test_sum(Array<float, 2>& av, float v)
{
  bool correct = ( av.reduce(std::plus<float>()) == v); 
  std::cout << (correct ? "Test is PASSED. \n" : "Test is FAILED. \n");
  return (correct ? 0 : -1);
}

int main()
{ Array<float, 2> av(N, N, a), bv(N, N, b);
  Array<float, 2> cv(N, N);
  Array<float, 0> s = 4.5f;
  int result = 0;
  
  for(int i =0; i < N; i++)
    for(int j =0; j < N; j++) {
      av(i,j) = 5.f;
      bv(i,j) = 6.f; 
      cv(i,j) = 0.f;
    }
  
  eval(add1)(cv, av, bv);
  
  result += test_sum(cv, 1760000.f);

  eval(add2)(cv, av, bv);
  
  result += test_sum(cv, 1760000.f);
  
  eval(add3)(cv, av, bv);
  
  result += test_sum(cv, 1760000.f);
  
  eval(add4)(cv, av, bv);
  
  result += test_sum(cv, 1760000.f);
  
  return result;
}
