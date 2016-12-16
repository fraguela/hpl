#include <cstdlib>
#include <cstdio>
#include "HPL.h"

#define N 200
#define M 400
#define P 300

using namespace HPL;


int grain_rows = 4;
int grain_cols = 4;
constexpr int sub_grain_cols = 2;

void matmulGPU(Array<float, 2> a, Array<float, 2> b, Array<float, 2> c)
{ Size_t i, j, k;
  
  const Size_t first_row = idx * grain_rows;
  const Size_t last_row = first_row + (grain_rows - 1);
  const Size_t first_column = idy * grain_cols;
  const Size_t last_column = first_column + (grain_cols - 1);
  
  for_(i = first_row, i <= last_row, ++i) {
    for_(j = first_column, j <= last_column, j+=sub_grain_cols ) {
      
      Float f[sub_grain_cols];
      
      for(int i_ = 0; i_ < sub_grain_cols; i_++)
	f[i_] = 0.f;
      
      for_(k = 0, k < N, ++k) {
	for(int i_ = 0; i_ < sub_grain_cols; i_++)
	  f[i_] += a[i][k] * b[k][j+i_];
      }
      
      for(int i_ = 0; i_ < sub_grain_cols; i_++)
	c[i][j+i_] = f[i_];
      
    } 
  } 
  
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
  
  if(argc>1) {
    if(argc == 3) {
      grain_rows = atoi(argv[1]);
      grain_cols = atoi(argv[2]);
    } else{
      puts("Matmul_granularity [rows] [columns]");
      puts(" They indicate the number of elements of the output computed by each thread");
      exit(EXIT_FAILURE);
    }
  }
  
  if(M % grain_rows) {
    puts("The grain size for rows is not a divisor of the number of rows");
    exit(EXIT_FAILURE);
  }
  
  if(P % grain_cols) {
    puts("The grain size for columns is not a divisor of the number of columns");
    exit(EXIT_FAILURE);
  }
  
  if(grain_cols % sub_grain_cols) {
    printf("The grain size for columns is not a multiple of subblock size %d\n", sub_grain_cols);
    exit(EXIT_FAILURE);
  }
  
  printf("Performing Matrix product of %dx%d Matrix by %dx%d Matrix computing %dx%d elements per thread\n", 
	 M, N, N, P, grain_rows, grain_cols);
  
  fillRandom2D<M,N>(a);
  fillRandom2D<N,P>(b, 133);
  
  tm.start();
  matmulCPU(a,b,c);
  printf("CPU code run in %lf s.\n", tm.stop());
       
  Array<float, 2> av(M, N, a);
  Array<float, 2> bv(N, P, b);
  Array<float, 2> cv(M,P);

  tm.start();
  eval(matmulGPU).global(M/grain_rows, P/grain_cols).local(1,1)(av, bv, cv);
  printf("GPU code run in %lf s.\n", tm.stop());
  
  /* compare the results and see if they match */
  return testtest(memcmp(cv.getData(), c, sizeof(c)) == 0);

}
