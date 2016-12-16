#include <cstdlib>
#include "Array.h"

using namespace HPL;

#define TESTTYPEINFO(TYPE) std::cout << #TYPE"= " << TypeInfo<TYPE>::String << " length=" << TypeInfo<TYPE>::Length << std::endl;


void test(bool b)
{
  if(!b) {
    std::cout << "Test is FAILED. \n";
    exit(EXIT_FAILURE);
  }  
}

void vectortype_array_test()
{ Array<int2, 2> a(2,2);

  a(0,0)(0) = 1;
  a(0,0)(1) = 2;
  
  a(0,1) = a(0,0);
  a(0,0)(0) = a(0,1)(0) + a(0,1)(1);
  
  for (int i=0; i<a.getDimension(0); i++)
    for(int j=0; j < a.getDimension(1); j++) {
      std::cout << a(i,j) << ' ';
    }

  test(a(0,0)(0) == 3);
  test(a(0,0)(1) == 2);
  test(a(0,1)(0) == 1);
  test(a(0,1)(1) == 2);
}

int main()
{
  Array<float, 2> a(6,8);
  Array<int, 3> b(10,20,30);
  Array<int> ai;
  
  std::cout << "Printing arrays: " << a << ' ' << b << ' ' << ai << std::endl;
  
  for(int i =1; i < 100; i++) {
    Array<int, 1> kk(9);
    
    std::cout << kk << "   ";
  }
  
  std::cout << "\n\n Now testing Array information funcions:\n";
  
  for (int i=0; i<6; i++) {
    for(int j=0; j < 8; j++)
      a(i,j) = i +j;
  }
  
  b(0,0,0) = a(3,3);
  b(0,0,1) = b(0,0,0); 
  b(0,0,2) = a(0,0) + a(1,1) * a(2,2) + 1;
  b(0,0,3) = b(0,0,0) + b(0,0,1);
  

  float *p = a.data();
  for (int i=0; i<a.getDimension(0); i++) {
    for(int j=0; j < a.getDimension(1); j++) {
      std::cout << *p++ << ' ';
    }
    std::cout << std::endl;
  }
  

  test(b(0,0,0) ==  6);
  test(b(0,0,1) ==  6);
  test(b(0,0,2) ==  9);
  test(b(0,0,3) == 12);


  
  vectortype_array_test();
  
  std::cout << "\n---------\n";
  
  TESTTYPEINFO(float);
  TESTTYPEINFO(double);
  TESTTYPEINFO(int);
  TESTTYPEINFO(char);
  TESTTYPEINFO(Array<int>);
  TESTTYPEINFO(float4);
  TESTTYPEINFO(int8);
  
  std::cout << "Test is PASSED. \n";
  return 0;
}
