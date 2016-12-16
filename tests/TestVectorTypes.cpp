#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;


/************************* 1D test *************************/

void donothing(Float x)
{
  return_();
}

float2 myf(Array<float4,1> m)
{
	Float2 f_2;
	Float4 f_4;
	f_4 = m[idx];
	f_2[0] = f_4[2];
	f_2[1] = f_4[3];
	return_(f_2);
}

void myf2(Array<float, 1> a, Array<float4, 1> b, Float4 f)
{ Int tmp = 5;
  Float2 f1(float2(1.1f, 2.2f));
  Int4   i4(int4(10,20,30,40));
  Int4   j4(1.1f, i4[0] + tmp * 8, i4[1], 3.3f);
  Float2 f2 = 3.1f;
  Int2  i2(tmp + 8, 4);
  Int16 limit(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
  
  j4 = int4(10,20,30,40);
  
  i4 = j4 + Int4(100, i2[1], 300, tmp) * 3;
  
  call(donothing)(f2[1]);
  
  call(donothing)(a[0]);
  
  f1[0] = b[idx][0];
  f1[1] = b[idx][1];
  f2 = call(myf)(b);

  a[idx] = f1[0]*1 + f1[1]*2 + f2[0]*3+ f2[1]*4;

  b[idx] = f;
}

float mysumF4(Array<float4, 1>& av)
{
  float results = 0.0;
  for(int i =0; i < N; i++)
    results += av(i)(0) + av(i)(1) + av(i)(2) + av(i)(3);

  return results;
}

int test1D() 
{ Array<float, 1> av(N);
  Array<float4, 1> bv(N);
  float4 v(1.0f,2.0f,3.0f,4.0f);
  Float4 f(v);
  
  for(int i =0; i < N; i++) {
    bv(i)(0) = 1.f;
    bv(i)(1) = 2.f;
    bv(i)(2) = 3.f;
    bv(i)(3) = 4.f;
  }
  
  eval(myf2)( av, bv, f);
  
  if( (av.reduce(std::plus<float>()) == 30*N) && (mysumF4(bv) == 10*N) ) {
    std::cout << "Test 1D is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test 1D is FAILED. \n";
    return -1;
  }
}


/************************* 2D test *************************/

void myf2d(Array<float4, 2> b)
{
  Int j;
  
  for_(j = 0, j < N, j++) {
    b[idx][j][2] = b[idx][j][2] - 3.0;
    b[idx][j][3] = b[idx][j][3] - 4.0;
  }
}

/*
 void display4(const Array<float4, 2>& av, const char *name)
 {
 cout << "Array<float4, 2> s0 " << name << ":\n";
 for(int i =0; i < 4; i++) {
 for(int j =0; j < 4; j++) {
 cout << av(i,j)(0) << "  ";
 }
 cout << endl;
 }
 cout << endl;
 
 cout << "Array<float4, 2> s1" << name << ":\n";
 for(int i =0; i < 4; i++) {
 for(int j =0; j < 4; j++) {
 cout << av(i,j)(1) << "  ";
 }
 cout << endl;
 }
 cout << endl;
 }
*/


float mysumF42(Array<float4, 2>& av)
{
  float results = 0.0;
  for(int i =0; i < N; i++)
    for(int j =0; j < N; j++)
      results += av(i,j)(0) + av(i,j)(1) + av(i,j)(2) + av(i,j)(3);
  
  return results;
}

int test2D() 
{ Array<float4, 2> bv(N,N);
  
  for(int i =0; i < N; i++) {
    for(int j =0; j < N; j++) {
      bv(i,j)(0) = 1.f;
      bv(i,j)(1) = 2.f;
      bv(i,j)(2) = 3.f;
      bv(i,j)(3) = 4.f;
    }
  }
  
  eval(myf2d).global(N).local(1)( bv);
  
  //display4(bv, "bv");
  
  if( mysumF42(bv) == (1*N*N + 2*N*N) ) {
    std::cout << "Test 2D is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test 2D is FAILED. \n";
    return -1;
  }
}

int main()
{ 
  return test1D() + test2D();
}




