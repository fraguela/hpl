#ifndef DEBUG
#define DEBUG 1
#endif

#include "HPL.h"

using namespace HPL;


void f0(Array<float, 2> a, Array<int, 3> b)
{ 
  Array<int> ai, bi;
  Array<int, 2, Private> cx(3,3);
  Array<int, 2, Local> dx(3,3);
  
  ai = a[8][ai];
  //computing expressions inside the indices
  bi = a[ai + 3][3*(ai+4*bi)-7];
  
  bi = b[ai][3-bi][bi];
  
}

void f1(Array<float, 2> a, Array<int, 3> b)
{ Array<int> ai, bi;
  
  ai = 99;
  
  ai = bi;
  
  ai = a[8][ai-4];
   
  ai = bi + 9 - b[1][bi][ai*2];
   
  ai=  ++ai;
   
  ai = ai--;
}

void f2(Array<float, 2> a, Array<int, 3> b)
{ Array<int> ai, bi;
  
  a[ai][bi] = 90.0;
  
  a[ai][bi] = ai;
  
  a[ai][bi] = b[bi][ai+4][3];
  
  a[ai][bi] = (a[ai+1][bi] + a[ai-1][bi] + a[ai][bi+1] + a[ai][bi-1]) / 4.0;
  
  a[ai][bi] = ai + bi + ai + bi * ai;
  
  a[ai][bi] = ai && bi;
}

int main()
{ Array<int> ai, bi;
  Array<float, 2> a(8,8);
  Array<int, 3> b(10,20,30);
  
  std::cout << "----- building indexed arrays: ----\n";
  
  eval(f0).getFhandle()->print();
  
  std::cout << "\n---- building assignments to scalars: ----\n";
  
  ai = 99;
  
  ai = bi;
  
  eval(f1).getFhandle()->print();
  
  std::cout << "---- building assignments to indexed arrays: ----\n";
  
  eval(f2).getFhandle()->print();
  
  return 0;
}
