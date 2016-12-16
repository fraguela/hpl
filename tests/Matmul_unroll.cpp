#include <cstdlib>
#include <cstdio>
#include "HPL.h"

#define N 200
#define M 400
#define P 300

using namespace HPL;

int unroll_factor = 4;

void matmulGPU(Array<float, 2> a, Array<float, 2> b, Array<float, 2> c)
{ Size_t k;
  Float f = 0.f;

  for_(k = 0, k <= (N - unroll_factor), k += unroll_factor) {
    for(int j = 0; j < unroll_factor; j++)
      f += a[idx][k+j] * b[k+j][idy];
  } 
  
  if(N % unroll_factor) {
    while_(k < N) {
      f += a[idx][k] * b[k][idy];
      k++;
    }
  }

  c[idx][idy] = f;
}



/* random initialisation of input */
template<unsigned int ROWS, unsigned int COLUMNS>
void fillRandom2D(float arrayPtr[ROWS][COLUMNS], unsigned int seed = 0)
{
  const float rangeMin = 0.0;
  const float rangeMax = 10.0;
  
  if(!seed)
    seed = (unsigned int)time(NULL);

  srand(seed);
  const float range = float(rangeMax - rangeMin) + 1.0;
  
  /* random initialisation of input */
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLUMNS; j++)
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

/*
 * c = ab where c[M][P] = a[M][N]*b[N][P]
 */
int main(int argc, char **argv)
{ float a[M][N], b[N][P], c[M][P];
  Timer tm;
  
  if(argc>1)
    unroll_factor = atoi(argv[1]);
  
  printf("Performing Matrix product of %dx%d Matrix by %dx%d Matrix using an unroll factor %d\n", M, N, N, P, unroll_factor);
  
  fillRandom2D<M,N>(a);
  fillRandom2D<N,P>(b, 133);
  
  tm.start();
  matmulCPU(a,b,c);
  printf("CPU code run in %lf s.\n", tm.stop());
       
  Array<float, 2> av(M, N, a);
  Array<float, 2> bv(N, P, b);
  Array<float, 2> cv(M,P);

  tm.start();
  eval(matmulGPU).global(M,P).local(1,1)(av, bv, cv);
  printf("GPU code run in %lf s.\n", tm.stop());
  
  /* compare the results and see if they match */
  return testtest(memcmp(cv.getData(), c, sizeof(c)) == 0);

}
