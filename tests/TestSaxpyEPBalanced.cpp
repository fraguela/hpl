#include <HPL.h>
//#include <omp.h>
using namespace HPL;


#define TYPE float
constexpr int N = (128*1048576); //(1048576*128)

void saxpy(Array<TYPE,1> dataX, Array<TYPE,1> dataY, Array<TYPE,0> alpha)
{
	dataY[idx] = alpha * dataX[idx] + dataY[idx];
}

void kk(Array<TYPE,1> nuevo)
{
	nuevo[idx] = (TYPE)3;
}

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
    
    Array<TYPE,1> arrayX(N, dataX);
    Array<TYPE,1> arrayY(N, dataY_GPU);
    
    // Create the ExecutionPlan with specifying the search algorithm.
    // Searh algorithms:
    // --> EXHAUS : Breadth-first search
    // --> ADAPT  : Starting with the same weight for each device, after the execution, we
    //              setup again the weights with the results.
    // --> ADAPT2 : Iterative algorithm that starts with predefined weights and it goes
    //              adjusting the weights until the time differences fall below a threshold (5%)
    ExecutionPlan ep(partitioner, HPL::ADAPT2);
    ep.add(gpu_d,50);
    ep.add(cpu_d,50);
    
    
    // testEP(arrayX, arrayY, ep);
    arrayY.getData(HPL_RDWR);
    
    t0.start();
    testEP(arrayX, arrayY, ep);
    std::cout << "Execution Plan Time:" << t0.stop() << " Host Time:";
    
    t0.start();
    saxpyOMP(dataX, dataY_CPU, 2);
    std::cout << t0.stop() << " sec." << std::endl;
    
    passed = (memcmp(arrayY.data(), dataY_CPU, N*sizeof(TYPE)) == 0);
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



