#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

float a[N], b[N];

void myf1(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c)
{  
  a[idx] =  b[idx] + c[idx];
}

void myf2(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c)
{ 
  a[idx] =  b[idx] * c[idx];  
}

float mysum(Array<float, 1>& av)
{
  float results = 0.0;
  
  for(int i =0; i < N; i++) 
    results += av(i);
  
  return results;
}

int main()
{ Array<float, 1> av(N, a), bv(N, b);
  Array<float, 1> cv(N), dv(N);
  
  for(int i =0; i < N; i++) {
      av(i) = 5.f;
      bv(i) = 6.f; 
      cv(i) = 0.f;
      dv(i) = 0.f;
    }

  eval(myf1)(cv, av, bv);
  
  //Notice that myf2 uses cv, which has been written by myf1

  eval(myf2)(dv, cv, bv);

  if( mysum(cv) == 4400.f && mysum(dv) == 26400.f ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
  
}
