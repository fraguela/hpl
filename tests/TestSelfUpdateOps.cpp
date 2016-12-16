#include <iostream>
#include "HPL.h"

using namespace HPL;

void unary_ops(UInt a)
{ Float c;
  Array<float, 1, Local> v(10);
  
  a = !a;
  a = -a;
  a = +a;
  a = ~a;
  ++a;
  a++;
  --a;
  a--;
  
  barrier(LOCAL);
  
  c = acos(v[0]);
  c = acosh(v[0]);
  c = acospi(v[0]);
  c = asin(v[0]);
  c = asinh(v[0]);
  c = asinpi(v[0]);
  c = atan(v[0]);
  c = atanh(v[0]);
  c = atanpi(v[0]);
  
  barrier(LOCAL);
  
  c = cbrt(v[0]);
  c = ceil(v[0]);
  
  barrier(LOCAL);
  
  c = cos(v[0]);
  c = cosh(v[0]);
  c = cospi(v[0]);
  
  barrier(LOCAL);
  
  c = erfc(v[0]);
  c = erf(v[0]);
  
  barrier(LOCAL);
  
  c = exp(v[0]);
  c = exp2(v[0]);
  c = exp10(v[0]);
  c = expm1(v[0]);
  
  barrier(LOCAL);
  
  c = fabs(v[0]);
  c = floor(v[0]);
  c = ilogb(v[0]);
  c = lgamma(v[0]);
  
  barrier(LOCAL);
  
  c = log(v[0]);
  c = log2(v[0]);
  c = log10(v[0]);
  c = log1p(v[0]);
  c = logb(v[0]);
  
  barrier(LOCAL);
  
  c = nan(a);
  c = rint(v[0]);
  c = round(v[0]);
  c = rsqrt(v[0]);
  
  barrier(LOCAL);
  
  c = sin(v[0]);
  c = sinh(v[0]);
  c = sinpi(v[0]);
  
  barrier(LOCAL);
  
  c = sqrt(v[0]);
  c = tan(v[0]);
  c = tanh(v[0]);
  c = tanpi(v[0]);
  
  barrier(LOCAL);
  
  c = tgamma(v[0]);
  c = trunc(v[0]);
}

void self_update_ops(Int a)
{ Int b;
  Float c;
  Array<int, 1, Local> v(10);
  Array<int, 2, Local> w(5,5);
  
  v[0] = a;
  v[1] = w[0][1];     //Different type
  v[2] = v[1];        //Same type
  v[3] = v[1] + 2 * a;
  v[4] = 4;
  
  barrier(LOCAL);
  
  b = c; //Different type
  b = a; //Same type
  b = w[0][1];
  b = v[1];
  b = v[1] + 2 * a;
  b = 4;
  
  barrier(LOCAL);

  v[0] += a;

  v[1] += w[0][1];     //Different type
  v[2] += v[1];        //Same type
  v[3] += v[1] + 2 * a;
  v[4] += 4;
  
  barrier(LOCAL);
 
  b -= c; //Different type
  b -= a; //Same type
  b -= w[0][1];
  b -= v[1];
  b -= v[1] + 2 * a;
  b -= 4;
  
  barrier(LOCAL);
  
  v[0] /= a;
  v[1] /= w[0][1];     //Different type
  v[2] /= v[1];        //Same type
  v[3] /= v[1] + 2 * a;
  v[4] /= 4;
  
  barrier(LOCAL);
  
  b /= c; //Different type
  b /= a; //Same type
  b /= w[0][1];
  b /= v[1];
  b /= v[1] + 2 * a;
  b /= 4;

}

int main()
{ Int a = 1;
  
  std::cout << "Testing generation of self_update operations\n";
  
  eval(self_update_ops)(a);
  
  //The following line is only for debugging
  TheGlobalState().getFHandle((void*)self_update_ops).print();

  std::cout << "Testing generation of unary operations\n";
  
  eval(unary_ops)(a);
  
  //The following line is only for debugging
  TheGlobalState().getFHandle((void*)unary_ops).print();
  
  return 0;
}
