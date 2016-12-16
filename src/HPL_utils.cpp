/*
 HPL : Heterogeneous Programming Library
 Copyright (c) 2012-2016 The HPL Team as listed in CREDITS.txt

 This file is part of HPL

 HPL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 HPL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with HPL.  If not, see <http://www.gnu.org/licenses/>.
*/

///
/// \file     HPL_utils.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include "HPL_utils.h"
#include "GlobalState.h"
#include <fstream>

namespace HPL {
  
  double Wtime()
  {
    return clock() / (double) CLOCKS_PER_SEC;
  }
  
  Timer::Timer()
  {
    this->start();
  }
  
  void Timer::start()
  {
    gettimeofday(&begin,NULL);
  }
  
  double Timer::stop()
  {
    gettimeofday(&end,NULL);
    double secs = (double)((end.tv_sec+(end.tv_usec/1000000.0)) - (begin.tv_sec+(begin.tv_usec/1000000.0)));
    return secs;
  }
  
  
  namespace utils {
    
    bool findWord(const std::string &str, const char* substr) 
    { std::size_t pos = 0, len = strlen(substr);
      
      do {
	
	pos = str.find(substr, pos, len);
	
	if (pos == std::string::npos) return false;
	
	if ((!pos || notInWord(str[pos-1])) && notInWord(str[pos + len])) return true;
	
	pos++;
	
      } while (1);
      
    }
    
#ifdef HPL_PROFILE
    void showTotalProfilerStatistics()
    {
    	std::ofstream myfile;
    	char s[512];
    	sprintf(s, "-----------------------------------------\n"
    			"|       HPL PROFILER OUTPUT             |\n"
    			"-----------------------------------------\n"
    			"|       STAGE        |     TIME (s)     |\n"
    			"-----------------------------------------\n"
    			"| Kernels creation   |     %3.6f     |\n"
    			"| Kernels compilation|     %3.6f     |\n"
    			"| CPU->GPU           |     %3.6f     |\n"
    			"| GPU->CPU           |     %3.6f     |\n"
    			"| Kernels execution  |     %3.6f     |\n"
    			"-----------------------------------------\n", HPL::TheGlobalState().total_secs_kernel_creation,
    			HPL::TheGlobalState().total_secs_kernel_compilation, HPL::TheGlobalState().total_secs_data_CPU_to_GPU,
    			HPL::TheGlobalState().total_secs_data_GPU_to_CPU, HPL::TheGlobalState().total_secs_device_execution);


    	myfile.open("HPL_PROFILER_OUTPUT.txt");
    	myfile << s;
    	myfile.close();
    }

    void clearProfilerStatistics()
    {
    	HPL::TheGlobalState().secs_data_CPU_to_GPU = 0;
    	HPL::TheGlobalState().secs_data_GPU_to_CPU = 0;
    }
#else
    void showTotalProfilerStatistics()
    {
    	std::cout << "The HPL profiler needs to enable HPL_PROFILE flag, please "
    			"set this compilation flag to see the profiler data." << std::endl;
    }
#endif
    
  }

}
