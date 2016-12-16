#include <HPL.h>
//#include <omp.h>
using namespace HPL;


#define N (1<<27)
#define TYPE float

void saxpy(Array<TYPE,1> dataX, Array<TYPE,1> dataY, Array<TYPE,0> alpha)
{
 dataY[idx] = alpha * dataX[idx] + dataY[idx];
}

/*
void test_single(TYPE* dataX, TYPE* dataY, Device& d)
{
  int alpha = 2;
  Array<TYPE,1> arrayX(N, dataX), arrayY(N, dataY);

  eval(saxpy).device(d)(arrayX, arrayY, alpha);

  arrayY.data(HPL_RDWR);
}
*/

void saxpyOMP(TYPE* dataX, TYPE* dataY, TYPE alpha)
{
//omp_set_num_threads(64);
//#pragma omp parallel for
 for(int i = 0; i < N; i++)
   dataY[i] = alpha * dataX[i] + dataY[i];
}

void partitioner(FRunner& fr,
                 Range ranges[3],
                 Array<TYPE,1>& dataX, Array<TYPE,1>& dataY, Array<TYPE,0>& alpha)
{
  fr(dataX(Range(ranges[0].origin, ranges[0].end)), dataY(Range(ranges[0].origin, ranges[0].end)),alpha);
}

void testEP(Array<TYPE,1>& dataX, Array<TYPE,1>& dataY, ExecutionPlan& ep)
{
  Array<TYPE,0> alpha = (TYPE)2;
  dataX.getData(HPL_RDWR);
  dataY.getData(HPL_RDWR);

  eval(saxpy).global(N).executionPlan(ep)(dataX, dataY, alpha);

  dataY.data(HPL_RDWR);
}

int main()
{ bool passed = true;
  
  const int GPUs_number = getDeviceNumber(GPU);
  const int ACCs_number = getDeviceNumber(ACCELERATOR);
  const int CPUs_number = getDeviceNumber(CPU);
  
  if (CPUs_number && (GPUs_number || ACCs_number)) {
    Timer t0;
    TYPE* dataX = new TYPE[N];
    TYPE* dataY_CPU = new TYPE[N];
    TYPE* dataY_GPU = new TYPE[N];
    Device cpu_d(CPU, 0), gpu_d(GPUs_number ? GPU : ACCELERATOR, 0);
    
    
    for(int i = 0; i < N; i++)
    {
      dataX[i] = dataY_CPU[i] = dataY_GPU[i] = (TYPE)1;
    }
    
    // The goal of these lines is to check the impact of the HPL_UNIFIEDMEMORY_ENABLE flag
    // on a single device execution.
    
    /*
     
     for(int i = 0; i < 5; i++)
     {
     t0.start();
     test_single(dataX, dataY_GPU, gpu_d);
     std::cout << "Time GPU:" << t0.stop() << std::endl;
     }
     
     for(int i = 0; i < 5; i++)
     {
     t0.start();
     test_single(dataX, dataY_CPU, cpu_d);
     std::cout << "Time CPU:" << t0.stop() << std::endl;
     }
     
     for(int i = 0; i < N; i++)
     if(dataY_GPU[i]!=dataY_CPU[i]) std::cout << "Error at:" << i << ", GPU= " << dataY_GPU[i] << ", CPU= " << dataY_CPU[i] <<  std::endl;
     */
    
    Array<TYPE,1> arrayX(N, dataX);
    Array<TYPE,1> arrayY(N, dataY_GPU);
    
    ExecutionPlan ep(partitioner);
    ep.add(cpu_d,50);
    ep.add(gpu_d,50);
  
    testEP(arrayX, arrayY, ep);  // 3 = 2 +1
    
    t0.start();
    testEP(arrayX, arrayY, ep);  //5 = 2 + 3
    std::cout << "Execution Plan Time:" << t0.stop() << " Host Time:";
    
    
    t0.start();
    saxpyOMP(dataX, dataY_CPU, (TYPE)4);  //5 = 4 + 1
    std::cout << t0.stop() << " sec." << std::endl;
    
    TYPE * const ptr_data_Y_GPU = (TYPE*)arrayY.data();
    passed = (memcmp(ptr_data_Y_GPU, dataY_CPU, N*sizeof(TYPE)) == 0);
  } else {
    puts("A CPU and an accelerator are needed to perform this test. Test skipped.");
    return 0;
  }

  
  if (passed) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }

}



