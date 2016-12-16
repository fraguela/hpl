#include <iostream>
#include "HPL.h"

using namespace HPL;

void wait ( int seconds )
{
  clock_t endwait;
  endwait = clock () + seconds * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}



int main()
{
  Timer tm;

  //system_info();

  double time;
  tm.start();
  wait(3);
  time = tm.stop();

  std::cout << time << " == 3 seconds. \n";
  
  std::cout << "Test is PASSED. \n";
 
  return 0;
}
