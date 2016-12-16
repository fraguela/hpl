#include <iostream>
#include "HPL.h"

#define N (64*1048576)

using namespace HPL;

int check(bool is_ok)
{
  if(is_ok) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

void increase(Array<float, 1> r)
{
    r[idx] = r[idx] + 1.f;
}

double benchmark(Array<float, 1>& av, HPL::Device d, float correct, int& ret)
{   Timer tm;
  
    tm.start();
    eval(increase).device(d).global(1024)(av);
    if(av(0) != correct) ret++;
    return tm.stop();
}

int test(HPL::Device d)
{   Array<float, 1> av(N), bv(N, HPL_FAST_MEM);
    DeviceProperties auxDP;
    int ret = 0;
  
    d.getProperties(auxDP);
    const char *str = auxDP.unifiedMemory ? "WITH" : "WITHOUT";
    
    std::cout << "Test on a " << HPL::Device::getGenericName(d.getDeviceType()) << ' ' << str << " unified memory. ";
    
    std::cout << "HPL support for unified memory " <<  (TheGlobalState().env.hpl_unifiedmemory_enable ? "ENABLED" : "DISABLED") << std::endl;
    
    std::cout << "[Platform " << d.getPlatformId() << " DeviceType " << d.getDeviceType() << " Device number " << d.getDeviceNumber() << "]\n";


    float *a = (float *)av.getData(HPL_WR);
    float *b = (float *)bv.getData(HPL_WR);
    
    for(int i =0; i < N; i++) {
        a[i] = 1.f;
        b[i] = 1.f;
    }
    
    eval(increase).device(d).global(1024)(av);
    if(av(0) != 2.f) ret++;
    eval(increase).device(d).global(1024)(bv);
    if(bv(0) != 2.f) ret++;
    
    av(0) = 3.f;
    bv(0) = 3.f;
    
    std::cout << "      Normal time " << benchmark(av, d, 4.f, ret) << std::endl;
    std::cout << "HPL_FAST_MEM time " << benchmark(bv, d, 4.f, ret) << std::endl;

    return ret;
}

int main()
{   int ret = 0;
    
    if(HPL::getDeviceNumber(CPU)) {
        TheGlobalState().env.hpl_unifiedmemory_enable = false;
        ret += test(HPL::Device(CPU,0));
        TheGlobalState().env.hpl_unifiedmemory_enable = true;
        ret += test(HPL::Device(CPU,0));
    }
    
    if(HPL::getDeviceNumber(GPU)) {
        TheGlobalState().env.hpl_unifiedmemory_enable = false;
        ret += test(HPL::Device(GPU,0));
        TheGlobalState().env.hpl_unifiedmemory_enable = true;
        ret += test(HPL::Device(GPU,0));
    }

    return check(ret == 0);
 
}
