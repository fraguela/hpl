#include <iostream>
#include "HPL.h"

#define N 128

using namespace HPL;


void display4(const Array<float, 1>& av, const char *name)
{
  std::cout << "Array<float, 1> " << name << ":\n";
  
  for(int i =0; i < 4; i++) {
    std::cout << av(i) << "  ";
  }
  std::cout << std::endl;
}

void setto2(Array<float4, 1> p)
{
  p[0] = 2.f;
}

void myf(Array<float, 1> r, Array<float, 1, Constant> a, Array<float, 1> b)
{
  AliasArray<float4> pt1(a[4*idx]);
  AliasArray<float4> pt2(b[4*idx]);
  AliasArray<float4> pt3(r[4*idx]);
  
  pt3[0] = pt1[0] + pt2[0];
  
  AliasArray<float4> p1(a); //__constant float4 *
  AliasArray<float4> p2(b); //__constant float4 *
  AliasArray<float4> p3(r); //__global float4 *

  //p3[idx] = p1[idx] + p2[idx];
  for (int i = 0; i < 4; ++i) {
    p3[idx+szx][i] = p1[idx+szx][i] + p2[idx+szx][i];
  }
  
  call(setto2)(pt2);
}

float mysum(Array<float, 1>& av)
{
  float results = 0.0;
  for(int i =0; i < N; i++)
    results += av(i);

  return results;
}

int main()
{ Array<float, 1, Constant> av(N);
  Array<float, 1> rv(N), bv(N);

  for(int i =0; i < N; i++) {
      av(i) = i;
      bv(i) = 1.f; 
      rv(i) = 0.f;
    }
  
  eval(myf).global(N/8)(rv, av, bv);

  display4(rv, "rv"); //Result must be [1 2 3 4]
  
  bool ok = (rv(0) ==  1.f) && (rv(1) ==  2.f) &&(rv(2) ==  3.f) && (rv(3) ==  4.f) &&
            (bv(0) == 2.f) && (bv(3) == 2.f) && (bv(4) == 2.f) && (bv(7) == 2.f);
  
  std::cout << mysum(rv) << "\n";
  
  if( ok && mysum(rv) == 8256.f ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}
