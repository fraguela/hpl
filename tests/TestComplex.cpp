#include <iostream>
#include "HPL.h"

using namespace HPL;

#define N 100

typedef struct {
  double real_;
  double imag_;
} dcomplex_cpu;

HPL_DEFINE_STRUCT(dcomplex, {double real_; double imag_;} );

Array<dcomplex,0> dcmplx_add(Array<dcomplex,0> a, Array<dcomplex,0> b){
	Array<dcomplex,0> aux;
	aux.at(real_) = (a).at(real_)+(b).at(real_);
	aux.at(imag_) = (a).at(imag_)+(b).at(imag_);
	return aux;
}

Array<dcomplex,0> dcmplx_sub(Array<dcomplex,0> a, Array<dcomplex,0> b){
	Array<dcomplex,0> aux;
	aux.at(real_) = (a).at(real_)-(b).at(real_);
	aux.at(imag_) = (a).at(imag_)-(b).at(imag_);
	return aux;
}

Array<dcomplex,0> dcmplx_mul(Array<dcomplex,0> a, Array<dcomplex,0> b){
	Array<dcomplex,0> aux;
	aux.at(real_) = (a).at(real_)*(b).at(real_) - (a).at(imag_)*(b).at(imag_);
	aux.at(imag_) = (a).at(real_)*(b).at(imag_) + (a).at(imag_)*(b).at(real_);
	return aux;
}

Array<dcomplex,0> dcmplx_div(Array<dcomplex,0> z1, Array<dcomplex,0> z2) {
  Double a, b, c, d, divisor, real, imag;
  a = z1.at(real_);
  b = z1.at(imag_);
  c = z2.at(real_);
  d = z2.at(imag_);

  divisor = c*c + d*d;
  real = (a*c + b*d) / divisor;
  imag = (b*c - a*d) / divisor;
  Array<dcomplex,0> result;
  result.at(real_) = real;
  result.at(imag_) = imag;
  return result;
}

#ifdef __APPLE__
// As of 2013-7-1 this bug in Apple OpenCL applies:
// http://www.khronos.org/message_boards/showthread.php/8220-problem-passing-struct-kernel-arguments-(Apple)

void dcmplx(Array<dcomplex, 1> v1, Array<dcomplex, 1> v2, Array<dcomplex, 1> v3,
		Array<dcomplex, 1> v4, Double real, Double imag)
{
        Array<dcomplex, 0> value;
        value.at(real_) = real;
        value.at(imag_) = imag;
	v1[idx] = dcmplx_add(v1[idx], value);
	v2[idx] = dcmplx_div(v2[idx], value);
	v3[idx] = dcmplx_sub(v3[idx], value);
	v4[idx] = dcmplx_mul(v4[idx], value);
}

#else

void dcmplx(Array<dcomplex, 1> v1, Array<dcomplex, 1> v2, Array<dcomplex, 1> v3,
	    Array<dcomplex, 1> v4, Array<dcomplex, 0> value)
{
  v1[idx] = dcmplx_add(v1[idx], value);
  v2[idx] = dcmplx_div(v2[idx], value);
  v3[idx] = dcmplx_sub(v3[idx], value);
  v4[idx] = dcmplx_mul(v4[idx], value);
}

#endif // __APPLE__

Array<dcomplex, 1> v1(N);
Array<dcomplex, 1> v2(N);
Array<dcomplex, 1> v3(N);
Array<dcomplex, 1> v4(N);
Array<dcomplex, 0> value;
dcomplex_cpu v1_cpu[N] __attribute__ ((aligned (256)));
dcomplex_cpu v2_cpu[N] __attribute__ ((aligned (256)));
dcomplex_cpu v3_cpu[N] __attribute__ ((aligned (256)));
dcomplex_cpu v4_cpu[N] __attribute__ ((aligned (256)));
dcomplex_cpu value_cpu __attribute__ ((aligned (256)));

void init_vectors()
{
  for(int i = 0; i < N; i++) {
    field(v1(i), real_) = i;
    field(v1(i), imag_) = i;
    field(v2(i), real_) = i;
    field(v2(i), imag_) = i;
    field(v3(i), real_) = i;
    field(v3(i), imag_) = i;
    field(v4(i), real_) = i;
    field(v4(i), imag_) = i;

    v1_cpu[i] = (dcomplex_cpu){(double)i,(double)i};
    v2_cpu[i] = (dcomplex_cpu){(double)i,(double)i};
    v3_cpu[i] = (dcomplex_cpu){(double)i,(double)i};
    v4_cpu[i] = (dcomplex_cpu){(double)i,(double)i};
  }

}

/*===================================================================================================
 * CPU FUNCTIONS
 ====================================================================================================*/
void cpu_dcmplx_add()
{
	for(int i = 0; i < N; i++)
	{
		v1_cpu[i] = (dcomplex_cpu){v1_cpu[i].real_ + value_cpu.real_, v1_cpu[i].imag_ + value_cpu.imag_};
	}
}

void cpu_dcmplx_div()
{
	double a,b,c,d,divisor,real,imag;
	for(int i = 0; i < N; i++)
	{
		a = v2_cpu[i].real_;
		b = v2_cpu[i].imag_;
		c = value_cpu.real_;
		d = value_cpu.imag_;

		divisor = c*c + d*d;
		real = (a*c + b*d) / divisor;
		imag = (b*c - a*d) / divisor;
		v2_cpu[i] = (dcomplex_cpu){real, imag};
	}
}

void cpu_dcmplx_sub()
{
	for(int i = 0; i < N; i++)
	{
		v3_cpu[i] = (dcomplex_cpu){v3_cpu[i].real_ - value_cpu.real_, v3_cpu[i].imag_ - value_cpu.imag_};
	}
}

void cpu_dcmplx_mul()
{
	double a, b;
	for(int i = 0; i < N; i++)
	{
		a = v4_cpu[i].real_ * value_cpu.real_ - v4_cpu[i].imag_ * value_cpu.imag_;
		b = v4_cpu[i].real_ * value_cpu.imag_ + v4_cpu[i].imag_ * value_cpu.real_;
		v4_cpu[i] = (dcomplex_cpu){a,b};
	}
}


/*===================================================================================================
 * CHECK FUNCTION
 ====================================================================================================*/
int check()
{
	int i = 0;
	for(i = 0; i< N; i++)
	{
		if((field(v1(i), real_)!=v1_cpu[i].real_) || (field(v1(i), imag_)!=v1_cpu[i].imag_))
		{
			std::cout << "v1(" << i << ").real_=" << field(v1(i), real_) << " Should be " << v1_cpu[i].real_ << std::endl;
			std::cout << "v1(" << i << ").imag_=" << field(v1(i), imag_) << " Should be " << v1_cpu[i].imag_ << std::endl;
			break;
		}
		if((field(v2(i), real_)!=v2_cpu[i].real_) || (field(v2(i), imag_)!=v2_cpu[i].imag_))
		{
			std::cout << "v2(" << i << ").real_=" << field(v2(i), real_) << " Should be " << v2_cpu[i].real_ << std::endl;
			std::cout << "v2(" << i << ").imag_=" << field(v2(i), imag_) << " Should be " << v2_cpu[i].imag_ << std::endl;
			break;
		}
		if((field(v3(i), real_)!=v3_cpu[i].real_) || (field(v3(i), imag_)!=v3_cpu[i].imag_))
		{
			std::cout << "v3(" << i << ").real_=" << field(v3(i), real_) << " Should be " << v3_cpu[i].real_ << std::endl;
			std::cout << "v3(" << i << ").imag_=" << field(v3(i), imag_) << " Should be " << v3_cpu[i].imag_ << std::endl;
			break;
		}
		if((field(v4(i), real_)!=v4_cpu[i].real_) || (field(v4(i), imag_)!=v4_cpu[i].imag_))
		{
			std::cout << "v4(" << i << ").real_=" << field(v4(i), real_) << " Should be " << v4_cpu[i].real_ << std::endl;
			std::cout << "v4(" << i << ").imag_=" << field(v4(i), imag_) << " Should be " << v4_cpu[i].imag_ << std::endl;
			break;
		}
	}

	if (i == N) {
		std::cout << "Test is PASSED.\n";
	        return 0;
	} else {
		std::cout << "Test is FAILED.\n";
	        return -1;
	}
}

int main()
{
  
  std::cout << "Using array of structs with " << v1.getNumberOfDimensions() << " dims\n";
  std::cout << "Element size= " << v1.getDataItemSize() << std::endl;
  
  init_vectors();
  value.value().real_ = 1.0;
  value.value().imag_ = 2.0;
  value_cpu.real_ = 1.0;
  value_cpu.imag_ = 2.0;

  cpu_dcmplx_add();
  cpu_dcmplx_div();
  cpu_dcmplx_sub();
  cpu_dcmplx_mul();

  HPL::AutoManageExceptions = false;

  try {
#ifdef __APPLE__
    eval(dcmplx)(v1,v2,v3,v4, value.value().real_, value.value().imag_);
#else
    eval(dcmplx)(v1,v2,v3,v4,value);
#endif
  } catch(HPLException o) {
    std::cout << o.what() << std::endl;
    return 0; //Because it must be because the device does not support doubles
  }
  
  return check();

}
