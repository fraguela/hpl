#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

/*** Kernels ***/

void myf0(Array<float, 1> a)
{
  a[idx] = 7.0;  
}

void myf1(Array<float, 1> a, Array<float, 1> b)
{
  a[idx] = b[idx];  
}

void myf2(Array<float, 1> a, Array<float, 1, Constant> b, Array<float, 1, Constant> c)
{
  a[idx] =  b[idx] + c[idx];  
}

void myf3(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c, Array<float, 1> d)
{
  a[idx] =  b[idx] + c[idx] + d[idx];  
}

void myf4(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c, Array<float, 1> d, Array<float, 1> e)
{
  a[idx] =  b[idx] + c[idx] + d[idx] + e[idx];  
}

void myf5(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c, Array<float, 1> d, Array<float, 1> e, Array<float,1> f)
{
  a[idx] =  b[idx] + c[idx] + d[idx] + e[idx] + f[idx];
}

int testtest(bool condition)
{
  if( condition ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

/*** Test drivers ***/

int test0()
{ Array<float, 1> cv(N);
  
  eval(myf0) (cv);
  
  return testtest( cv.reduce(std::plus<float>()) == 2800.f );
}

int test1()
{ Array<float, 1> av(N), cv(N);
  
  for(int i =0; i < N; i++) {
    av(i) = 5.0;
    cv(i) = 0.0;
  }
  
  eval(myf1) (cv, av);
  
  return testtest( cv.reduce(std::plus<float>()) == 2000.f );
}

int test2()
{ Array<float, 1, Constant> av(N), bv(N);
  Array<float, 1> cv(N);
  
  for(int i =0; i < N; i++) {
    av(i) = 5.0;
    bv(i) = 6.0; 
    cv(i) = 0.0;
  }
  
  eval(myf2)(cv, av, bv);
  
  float result = cv.reduce(std::plus<float>());
  
  ProfilingData tmp = getProfile();
  printf("%.13s \t %.13s \t %.13s \t %.13s \t %.13s\n", 
	 "Kernel_creation", "kernel_compilation", 
	 "CPU_GPU_transfer", "GPU_CPU_transfer", 
	 "kernel_execution");
  printf("%.8f(s) \t %.8f(s) \t %.8f(s) \t %.8f(s) \t %.8f(s)\n", 
	 tmp.secsKernelCreation, tmp.secsKernelCompilation, 
	 tmp.secsDataHostToDevice, tmp.secsDataDeviceToHost, 
	 tmp.secsKernelExecution);
  
  return testtest(result == 4400.f);
}

int test3()
{ Array<float, 1> av(N), bv(N), cv(N), ev(N);
  
  for(int i =0; i < N; i++) {
    av(i) = 2.0;
    bv(i) = 3.0; 
    cv(i) = 4.0;
    ev(i) = 0.0;
  }
  
  eval(myf3)(ev, av, bv, cv);
  
  return testtest( ev.reduce(std::plus<float>()) == 3600.f );
}

int test4()
{ Array<float, 1> av(N), bv(N), cv(N), dv(N), ev(N);
  
  for(int i =0; i < N; i++) {
    av(i) = 2.0;
    bv(i) = 3.0; 
    cv(i) = 4.0;
    dv(i) = 5.0; 
    ev(i) = 0.0;
  }
  
  eval(myf4) (ev, av, bv, cv, dv);
  
  return testtest( ev.reduce(std::plus<float>()) == 5600.f );
}

int test5()
{ Array<float, 1> av(N), bv(N), cv(N), dv(N), ev(N), zv(N);
  
  for(int i =0; i < N; i++) {
    av(i) = 2.0;
    bv(i) = 3.0; 
    cv(i) = 4.0;
    dv(i) = 5.0; 
    ev(i) = 6.0; 
    zv(i) = 0.0;
  }
  
  eval(myf5) (zv, av, bv, cv, dv, ev);
  
  return testtest( zv.reduce(std::plus<float>()) == 8000.f );
  
}

int main()
{ 
  return test0() + test1() + test2() + test3() + test4() + test5();
}
