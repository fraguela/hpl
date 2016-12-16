#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

Device_t dev_type;

/*** Kernels ***/

void myf0(Array<float, 1> a)
{
  a[idx] = 7.0;  
}

void myf0_partitioner(FRunner& fr,
     Range ranges[3],
     Array<float,1>& A)
{
  fr(A(ranges[0]));
}

int test0()
{ Array<float, 1> cv(N);
  
 ExecutionPlan ep(myf0_partitioner);
 ep.add(HPL::Device(dev_type, 0), 50);
 ep.add(HPL::Device(dev_type, 1), 50);
// ep.setPlanner(myf0_partitioner);

  //eval(myf0).global(N,1,1).local(1,1,1).executionPlan(ep)(cv);
  eval(myf0).global(N).executionPlan(ep)(cv);

  if( (cv(0) == cv(N/2)))
  {
    std::cout << "Test PASSED!\n";
    return 0;
  }
  else
  {
   std::cout << "Test FAILED!\n";
   return -1;
  }
}

void matmulGPU(Array<float, 2> a, Array<float, 2> b, Array<float, 2> c)
{
  Size_t k;
  
  c[idy][idx] = 0;
  for_(k =0, k < N, k++)
    c[idy][idx] += a[idy][k] * b[k][idx];

}

void mxm_partitioner(FRunner& fr,
     Range ranges[3],
     Array<float,2>& A,
	Array<float,2>& B,
	Array<float,2>& C)
{
  fr(A(ranges[0], Tuple(0,N-1)), B, C(ranges[0], Tuple(0,N-1)));
}


int test1()
{
 Array<float,2> av(N,N), bv(N,N), cv(N,N);

 for(int i = 0; i < N; i++)
   for(int j = 0; j < N; j++)
   {
     av(i,j) = i + 1.0f;
     bv(i,j) = j + 1.0f;
     cv(i,j) = 0.0f;
   }

  float ctest[N][N];
#pragma omp parallel for
  for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++)
    {
        ctest[i][j] = 0;
        for(int k =0; k < N; k++)
          ctest[i][j] += av(i,k) * bv(k,j);
    }

  ExecutionPlan ep(mxm_partitioner);
 ep.add(HPL::Device(dev_type, 0), 20);
 ep.add(HPL::Device(dev_type, 1), 60);
 ep.add(HPL::Device(CPU, 0), 20);
 //ep.setPlanner(mxm_partitioner);
 eval(matmulGPU).global(N,N).executionPlan(ep)(av,bv,cv);

 if( memcmp(ctest, cv.getData(), N * N * sizeof(float)) == 0)
 {
  std::cout << "Test PASSED!\n";
  return 0;
 }
 else
 {
   std::cout << "Test FAILED!\n";
   return -1;
 }

}

int main()
{ int result = 0;

  const int GPUs_number = HPL::getDeviceNumber(GPU);
  const int ACCs_number = getDeviceNumber(ACCELERATOR);
  const int CPUs_number = HPL::getDeviceNumber(CPU);
  std::cout << "There are "<< GPUs_number << " GPUs, " << ACCs_number << " acceledators and " << CPUs_number << " CPUs\n";
  
  if(GPUs_number > 1 ) {
    dev_type = GPU;
  } else if(ACCs_number > 1) {
    dev_type = ACCELERATOR;
  } else {
    std::cout << "This test needs 2 accelerators. Skipped\n";
    return result;
  }

  result = test0();
  if(CPUs_number) {
    result = result + test1();
  }
  
  return result;
}
