#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "HPL.h"

using namespace HPL;


/////////////////////////// TEST1 //////////////////////////////////////

#define N 100
#define M 100
#define P 100

const char * const kernel_code = 
  "__kernel void mxmul_simple(__global float *a, __global float *b,    \n \
                              __global float *c, const int n)          \n \
  { int i, j, k;                   \n \
    float f = 0.;                  \n \
    i = get_global_id(1);          \n \
    j = get_global_id(0);          \n \
    if(i < n && j < n) {           \n \
      for(k=0; k < n; k++)         \n \
        f += a[i*n+k] * b[k*n+j];  \n \
      c[i*n+j] = f;                \n \
    }                              \n \
  }                                \n";

const char * const kernel_code2 = 
TOSTRING(
	 __kernel void mxmul_simple2(__global float *a, __global float *b,
				    __global float *c, const int n)
	 { int i, j, k;
	   float f = 0.;
	   i = get_global_id(1);
	   j = get_global_id(0);
	   if(i < n && j < n) {
	     for(k=0; k < n; k++)
	       f += a[i*n+k] * b[k*n+j];
	     c[i*n+j] = f;
	   }
	 } 
);

void matmulGPU(In<Array<float, 2> > a, In<Array<float, 2> > b, Out<Array<float, 2> > c, Int n)
{ }

void matmulGPU2(In<Array<float, 2> > a, In<Array<float, 2> > b, Out<Array<float, 2> > c, Int n)
{ }

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
 * c = ab where c[M][P] = a[M][N] * b[N][P]
 */
bool test1()
{ int n = N;
  float a[M][N], b[N][P], c[M][P];
  
  fillRandom2D(a, M, N);
  fillRandom2D_v2(b, N, P);
  matmulCPU(a, b, c);

  Array<float, 2> av(M, N, a);
  Array<float, 2> bv(N, P, b);
  Array<float, 2> cv(M,P);

  nativeHandle(matmulGPU, "mxmul_simple", kernel_code);
  
  eval(matmulGPU).global(M,P)(av, bv, cv, n);

  /* compare the results and see if they match */
  if(memcmp(cv.getData(), c, M*P*sizeof(float)) == 0) {
    std::cerr << "Test is PASSED. \n";
  } else {
    std::cerr << "Test is FAILED. \n";
    return false;
  }
  
  memset(cv.getData(), 0, M*P*sizeof(float));
  
  nativeHandle(matmulGPU2, "mxmul_simple2", kernel_code2);
  
  eval(matmulGPU2).global(M,P)(av, bv, cv, n);
  
  /* compare the results and see if they match */
  if(memcmp(cv.getData(), c, M*P*sizeof(float)) == 0) {
    std::cerr << "Test is PASSED. \n";
    return true;
  } else {
    std::cerr << "Test is FAILED. \n";
    return false;
  }
  
}

/////////////////////////// TEST2 //////////////////////////////////////

#define HANDLE_MXV(f) void f(Out<Array<float, 1> > y, \
                             In<Array<float, 1> > mx, \
                             In<Array<float, 1> > v,  \
                             Int rows, Int cols) {}

HANDLE_MXV(mxmulv);
HANDLE_MXV(mxmulv_alt);
HANDLE_MXV(mxmulv_blocked);
HANDLE_MXV(mxmulv_blocked_alt);
HANDLE_MXV(mxmulv_blocked2);

typedef void (*mxv_ptrf) (Out<Array<float, 1> > y,
                          In<Array<float, 1> > mx,
                          In<Array<float, 1> > v,
                          Int rows, Int cols);

mxv_ptrf f[] = { mxmulv, mxmulv_alt, mxmulv_blocked, mxmulv_blocked_alt, mxmulv_blocked2 };
const char* name[] = { "mxmulv", "mxmulv_alt", "mxmulv_blocked", "mxmulv_blocked_alt", "mxmulv_blocked2" };

bool test2()
{ Timer tm;
  const int ROWS = 20480, COLS = 2048;
  Array<float, 2> mx(ROWS, COLS);
  Array<float, 1> v(COLS), y(ROWS);
  
  if (getDeviceNumber(GPU)) {

    Device d(GPU, 0);
    
    HPL::AutoManageExceptions = false;
    
    try {
      for(int i = 0; i < sizeof(f)/sizeof(f[0]); i++) {
        std::cerr << "Initializing " << name[i] << std::endl;
        nativeHandle(*f[i], name[i], i ? "" : "mxmulv.cl");
        eval(*f[i]).device(d).global(ROWS).local(64)(y, mx, v, ROWS, COLS); //Compiled & data moved
      }
      
      d.sync();
      
      for(int i = 0; i < sizeof(f)/sizeof(f[0]); i++) {
        tm.start();
        eval(*f[i]).device(d).global(ROWS).local(64)(y, mx, v, ROWS, COLS);
        d.sync();
        std::cout << std::setw(20) << name[i] << " : " << tm.stop() << " s.\n";
      }
    } catch (HPLException o) {
      std::cout << o.what() << std::endl << "Skipping Test 3 (problem is within OpenCL native kernel, not HPL)\n";
    }
  } else {
    std::cout << "Skipping Test 3 (designed only for GPUs but no GPU is available)\n";
  }

  return true;
}

int main()
{
  return (test1() && test2()) ? 0 : 1;
}