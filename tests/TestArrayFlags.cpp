#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

float a[N][N], b[N][N];

void display4(const Array<float, 2>& av, const char *name)
{
  std::cout << "Array<float, 2> " << name << ":\n";
  
  for(int i =0; i < 4; i++) {
    std::cout << av(i,0) << ' ' << av(i,1) << ' ' << av(i,2) << ' ' << av(i,3) << std::endl;
  }
}

void myf1(Array<float, 2> a, Array<float, 2, Local> b, Array<float, 2, Constant> c)
{
  Array<float, 0> f; 
  Array<float, 1, Local> lf(10);
  Array<float, 2, Local> lf2(5, 8);
  Array<double, 2, Private> pd(5, 2);
  
  lf[0] = 1.f;
  pd[1][1] = lf2[idx-idy][idy+idx];
  
  f = a[idx][idy] + b[idx][idy] + c[idx][idy];

}

int main()
{ Array<float, 2> av(N, N, a), bv(N, N, b);
  Array<float, 2> cv(N, N);
  Array<float, 0> s = 4.5f;
  
  for(int i =0; i < N; i++)
    for(int j =0; j < N; j++) {
      av(i,j) = (float)(i + j);
      bv(i,j) = s.value();
      cv(i,j) = (float)(i * j);
    }
  
  FHandle *fh = eval(myf1).getFhandle();
  
  fh->print();

  return 0;
}
