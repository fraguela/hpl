#include <iostream>

#include "HPL.h"

#define N           1048576
#define MAXTHREADS      512
#define FOO             100

using namespace HPL;

float a[N], b[N], c[N];

Array<float, 1> av(N, a), bv(N, b);
Array<float, 1> cv(N, c);

void cpu_execution(float *a, float *b)
{ int i, j;
  
  for(i = 0; i < N; i++)
    for(j = 0; j < FOO; j++) {
      a[i] = b[i] + j/FOO;
    }
}

void myf1(Array<float, 1> a, Array<float, 1> b)
{ Int i, iend, j;
  
  i = idx * (N / MAXTHREADS);
  iend = i + (N / MAXTHREADS);
  
  while_(i < iend) {
    for_(j = 0, j < FOO, j++) {
      a[i] = b[i] + j/FOO;
    }
    i++;
  }
  
}

int main()
{ Timer tm;
  int GPUs_number, CPUs_number;
  int res = 0;
  
  for(int i =0; i < N; i++) {
      av(i) = 5.f;
      cv(i) = 0.f;
    }

  CPUs_number = HPL::getDeviceNumber(CPU);
  GPUs_number = HPL::getDeviceNumber(GPU);

  std::cout << "There are "<< CPUs_number << " CPUs.\n" << HPL::getDeviceInfo(CPU);
  std::cout << "There are "<< GPUs_number << " GPUs.\n" << HPL::getDeviceInfo(GPU);

  std::cout << "\nExecution results:\n";
  
  tm.start();
  cpu_execution(c,a);
  double usecs = tm.stop();
  if( cv.reduce(std::plus<float>()) == 5242880.f )
    std::cout << "Test is PASSED for native code in CPU in "<< usecs <<" seconds.\n";
  else {
    std::cout << "Test is FAILED for native code in CPU.\n";
    res--;
  }
  
  for(int i = 0; i < CPUs_number; i++)
  {
          tm.start();
          eval(myf1).device(CPU,i).global(MAXTHREADS)(cv, av);
          double usecs = tm.stop();
          if( cv.reduce(std::plus<float>()) == 5242880.f )
	    std::cout << "Test is PASSED for HPL in CPU #"<< i << " in "<< usecs <<" seconds.\n";
          else {
	    std::cout << "Test is FAILED for HPL in CPU #"<< i << "\n";
	    res--;
	  }
  }
  
  for(int i = 0; i < GPUs_number; i++)
  {
          tm.start();
          eval(myf1).device(GPU, i).global(MAXTHREADS)(cv, av);
          double usecs = tm.stop();
          if( cv.reduce(std::plus<float>()) == 5242880.f )
	    std::cout << "Test is PASSED for HPL in GPU #"<< i << " in "<< usecs <<" seconds.\n";
          else {
	    std::cout << "Test is FAILED for HPL in GPU #"<< i << "\n";
	    res--;
	  }
  }
  
  return res;
}
