#include <iostream>
#include <functional>
#include "HPL.h"

#define N 10

using namespace HPL;

float a[N];

struct KernelClass {
  
  static float eval_constant;

  static void setup(float v) {
    eval_constant = v;
  }
  
  static void myf(Array<float, 1> a) {
    a[idx] = eval_constant;
  }
  
  static bool checkArray(const Array<float, 1>& array) {
    return checkArray(array, eval_constant);
  }
  
  static bool checkArray(const Array<float, 1>& array, float v) {
    for(int i = 0; i < N; i++)
      if(array(i) != v) return false;
    return true;
  }

};

float KernelClass::eval_constant;

//////////////////////// std::function
void globalfunc(Array<float, 1> a) {
  a[idx] = KernelClass::eval_constant;
}

//////////////////////// dynamic object

class A {
  float v_;
public:
  A(float v)
  : v_(v)
  {}
  
  void operator() (Array<float, 1> a) {
    a[idx] = v_;
  }

  void setup(float v) { v_ = v; }
  
  bool checkArray(const Array<float, 1>& a) {
    KernelClass::setup(v_);
    return KernelClass::checkArray(a);
  }
  
};

int main()
{ bool first_eval, second_eval, third_eval, fourth_eval, fifth_eval;
  Array<float, 1> av(N, a);
  
  KernelClass::setup(2.0f);
  
  eval(KernelClass::myf)(av);
  
  first_eval = KernelClass::checkArray(av);
  
  KernelClass::setup(3.0f);
  
  reeval(KernelClass::myf)(av);
  
  second_eval = KernelClass::checkArray(av);
  
  //////////////////////// std::function
  
  std::function<void(Array<float, 1>)> stdf = globalfunc;
  
  KernelClass::setup(4.0f);
  
  eval(stdf)(av);
  
  third_eval = KernelClass::checkArray(av);
  
  //////////////////////// dynamic object
  
  A a(80.f);
  
  eval(a)(av);
  
  fourth_eval = a.checkArray(av);
  
  a.setup(78.0f);
  
  reeval(a)(av);
  
  fifth_eval = a.checkArray(av);
  
  if(first_eval && second_eval && third_eval && fourth_eval && fifth_eval) {
    std::cout << "Test is PASSED.\n!" << std::endl;
    return 0;
  } else {
    std::cout << "Test is FAILED.\n" << std::endl;
    return -1;
  }
  
}
