#include <iostream>
#include <cstdlib>
#include "HPL.h"

#define N 200
#define M 400
#define P 300

using namespace HPL;

void matmulGPU(Array<float, 2> a, Array<float, 2> b, Array<float, 2> c)
{
  Size_t k;
  
  c[idx][idy] = 0;
  for_(k =0, k < N, k++)
    c[idx][idy] += a[idx][k] * b[k][idy];

}



/* random initialisation of input */
void fillRandom2D(float arrayPtr[M][N], const int height, const int width)
{
  float rangeMin = 0.0;
  float rangeMax = 10.0;
  unsigned int seed = (unsigned int)time(NULL);

  srand(seed);
  float range = float(rangeMax - rangeMin) + 1.0;
  /* random initialisation of input */
  for(int i = 0; i < height; i++)
    for(int j = 0; j < width; j++)
      arrayPtr[i][j] = rangeMin + int(range*rand()/(RAND_MAX + 1.0));
}
void fillRandom2D_v2(float arrayPtr[N][P], const int height, const int width)
{
  float rangeMin = 0.0;
  float rangeMax = 10.0;
  unsigned int seed = 133;
  srand(seed);
  float range = float(rangeMax - rangeMin) + 1.0;
  /* random initialisation of input */
  for(int i = 0; i < height; i++)
    for(int j = 0; j < width; j++)
      arrayPtr[i][j] = rangeMin + int(range*rand()/(RAND_MAX + 1.0));
}

void matmulCPU(float a[M][N], float b[N][P], float c[M][P])
{

  for (int i = 0; i < M; i++)
    for (int j = 0; j < P; j++) {
      c[i][j] = 0;
      for (int k = 0; k < N; k++)
        c[i][j] += a[i][k] * b[k][j];
    }
}

/*
 * c = ab where c[M][P] = a[M][N]*b[N][P]
 */
int main()
{ 
  float a[M][N], b[N][P], c[M][P];
  
  fillRandom2D(a,M,N);
  fillRandom2D_v2(b,N,P);
  matmulCPU(a,b,c);

  Array<float, 2> av(M, N, a);
  Array<float, 2> bv(N, P, b);
  Array<float, 2> cv(M,P);

  eval(matmulGPU).global(M,P).local(1,1)(av, bv, cv);

  /* compare the results and see if they match */
  if(memcmp(cv.getData(), c, M*P*sizeof(float)) == 0) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
  
}
