#include <iostream>
#include <cstdlib>
#include <cmath>
#include "HPL.h"

#define N 10
#define M 40

using namespace HPL;

void matrixVectorGPU(Array<float, 2> a, Array<float, 1> x, Array<float, 1> y)
{
  Array<int, 0> k;
  Array<float, 0> sum = 0.0;

  for_(k =0, k < N, k++)
    sum += (a[idx][k] * x[k]);

  y[idx] =  sum;
}



/* random initialisation of input */
void fillRandom2D(float arrayPtr[M][N], const int width, const int height)
{
  float rangeMin = 0.0;
  float rangeMax = 10.0;
  unsigned int seed = 133;
  srand(seed);
  float range = float(rangeMax - rangeMin) + 1.0;
  /* random initialisation of input */
  for(int i = 0; i < height; i++)
    for(int j = 0; j < width; j++)
      arrayPtr[i][j] = rangeMin + float(range*rand()/(RAND_MAX + 1.0));
}
void fillRandom1D(float * arrayPtr, const int width)
{
  float rangeMin = 0.0;
  float rangeMax = 10.0;
  unsigned int seed = 133;
  srand(seed);
  float range = float(rangeMax - rangeMin) + 1.0;
  /* random initialisation of input */
  for(int i = 0; i < width; i++)
    arrayPtr[i] = rangeMin + float(range*rand()/(RAND_MAX + 1.0));
}

void matrixVectorCPU(float a[M][N], float *x, float *y)
{

  for (int i=0;i<M;i++) {
    float sum = 0.0f;
    for (int k=0;k<N;k++) {
      sum += a[i][k] * x[k];
    }
    y[i] = sum;
  }
}

bool compare(const float *refData, const float *data, const int length)
{
  const float epsilon = 1e-6f;
  float error = 0.0f;
  float ref = 0.0f;

  for(int i = 1; i < length; ++i) {
    float diff = refData[i] - data[i];
    error += diff * diff;
    ref += refData[i] * refData[i];
  }

  float normRef =::sqrtf((float) ref);
  if (::fabs((float) ref) < 1e-7f) {
    return false;
  }
  float normError = ::sqrtf((float) error);
  error = normError / normRef;

  return error < epsilon;
}

/*
 * y = ax where y[M] = a[M][N]*x[N]
 */
int main()
{ 
  float a[M][N], x[N], y[M], ycopy[M];
  fillRandom2D(a,N,M);
  fillRandom1D(x,N);
  matrixVectorCPU(a,x,y);

  Array<float, 2> av(M, N, a);
  Array<float, 1> xv(N, x);
  Array<float, 1> yv(M);

  eval(matrixVectorGPU).global(M).local(1)(av, xv, yv);

  // copy result for verification
  for(int i =0; i < M; i++) ycopy[i] = yv(i); 
  /* compare the results and see if they match */
  if(compare(ycopy, y, M)) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}
