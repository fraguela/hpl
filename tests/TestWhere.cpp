#include <iostream>
#include "HPL.h"

#define N 4

using namespace HPL;

float a[N], b[N],c[N];

void display4(const Array<float, 1>& av, const char *name)
{
  std::cout << "Array<float, 1> " << name << ":\n";
  
  for(int i =0; i < 4; i++) {
    std::cout << av(i) << "  ";
  }
  std::cout << std::endl;
}

void myf(Array<float, 1> r, Array<float, 1, Constant> a, Array<float, 1, Constant> b)
{
  r[idx] =  where(a[idx] > b[idx], a[idx] + 100, a[idx]);
  r[idx] =  where(r[idx] == 0, 99, r[idx]);
}

float mysum(Array<float, 1>& av)
{
  float results = 0.0;
  for(int i =0; i < N; i++)
    results += av(i);

  return results;
}

int main()
{ Array<float, 1, Constant> av(N, a), bv(N, b);
  Array<float, 1> rv(N);
  Float s = 4.5f;

  for(int i =0; i < 4; i++) {
      av(i) = i;
      bv(i) = 1.f; 
      rv(i) = 0.f;
    }
  
  eval(myf)(rv, av, bv);

  display4(rv, "rv"); //Result must be [99 1 102 103]
  
  if( (rv(0) ==  99.f) && (rv(1) ==   1.f) && (rv(2) == 102.f) && (rv(3) == 103.f) && mysum(rv) == 305.f ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}
