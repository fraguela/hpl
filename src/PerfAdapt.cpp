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
/// \file     PerfAdapt.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

// /usr/local/gcc-4.7.1/bin/g++  -g  -std=c++0x -I. -I/Users/basilio/Projects/hpl/trunk/src PerfAdapt.cpp HPL.a  -framework OpenCL 

#include <cstring>
#include "CLbinding.cpp"

const char * const kernel_code = "__kernel void add1(__global char *p, int n) { for(int i = get_global_id(0); i < n; i += get_global_size(0)) p[i]++; } ";

constexpr cl_int BufSize = 1024 * 1024;
char hostBuffer[BufSize];


void eval(const HPL::Platform_t pt, const HPL::Device_t dt)
{ cl_int err;
  cl::Event e;
  HPL::Timer tm;
  double time_2transfers, time_copybuffer;

  HPL::internal::DeviceImplementation *di0 = HPL::TheCLbinding().getDevice(pt, dt, 0);
  HPL::internal::DeviceImplementation *di1 = HPL::TheCLbinding().getDevice(pt, dt, 1);
  HPL::internal::DeviceTypeData *dtd = di0->dtd_;
  
  cl::Buffer b0(dtd->context_, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, BufSize, hostBuffer);
  cl::Buffer b1(dtd->context_, CL_MEM_READ_WRITE, BufSize);
 
  cl::Program::Sources sources(1, std::make_pair(kernel_code, strlen(kernel_code)+1));
  dtd->program_ = cl::Program(dtd->context_, sources, &err);
  assert(err == CL_SUCCESS);
  
  err = dtd->program_.build(dtd->devices_);
  assert(err == CL_SUCCESS);
  
  cl::Kernel kernel(dtd->program_, "add1", &err);
  assert(err == CL_SUCCESS);
  
  kernel.setArg(0, b0);
  kernel.setArg(1, BufSize);

  cl::NDRange global(128);
  
  HPLSafeMsg(di0->cq_.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange, NULL, &e), "enqueueNDRangeKernel");
  HPLSafeMsg(e.wait(), "e.wait()");
  
  tm.start();
  err = di0->cq_.enqueueReadBuffer(b0, CL_TRUE, 0, BufSize, static_cast<void *> (hostBuffer));
  assert(err == CL_SUCCESS);
  err = di1->cq_.enqueueWriteBuffer(b1, CL_TRUE, 0, BufSize, static_cast<void *> (hostBuffer));
  assert(err == CL_SUCCESS);
  time_2transfers = tm.stop();
  
  
  HPLSafeMsg(di0->cq_.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange, NULL, &e), "enqueueNDRangeKernel");
  HPLSafeMsg(e.wait(), "e.wait()");
  
  tm.start();
  err = di1->cq_.enqueueCopyBuffer(b0, b1, 0, 0, BufSize, NULL, &e);
  assert(err == CL_SUCCESS);
  err = e.wait();
  assert(err == CL_SUCCESS);
  time_copybuffer = tm.stop();
  
  //std::cout << HPL::Device::getGenericName(dt);
  
  //Format: HPL::Device_t HPL::Platform_t { T | C } /* Transfer or Copy */
  std::cout << static_cast<int> (dt) << ' ' << static_cast<int> (pt);
  std::cout << ' ' << ((time_2transfers < time_copybuffer) ? 'T' : 'C') << std::endl;

}

void eval_subb(const HPL::Platform_t pt, const HPL::Device_t dt)
{
	cl_int err;
	HPL::internal::DeviceImplementation *di0 = HPL::TheCLbinding().getDevice(pt, dt, 0);
	HPL::internal::DeviceTypeData *dtd = di0->dtd_;
	cl::Buffer b0(dtd->context_, CL_MEM_READ_WRITE, BufSize);
	cl_buffer_region region;
	region.origin = 4;
	region.size = 4;
	cl::Buffer* subbuffer = new cl::Buffer((b0.createSubBuffer(CL_MEM_READ_WRITE, CL_BUFFER_CREATE_TYPE_REGION, &region, &err)));
	std::cout <<  static_cast<int> (dt) << ' ' << static_cast<int> (pt);
	if(err == CL_MISALIGNED_SUB_BUFFER_OFFSET)
		std::cout << " N" << std::endl;
	else
		std::cout << " S" << std::endl;;
}

int main(int argc, char **argv)
{
  memset(static_cast<void *> (hostBuffer), 1, sizeof(hostBuffer));

  HPL::CLbinding& cl = HPL::TheCLbinding();
  
  int ndevicetypes = static_cast<int> (HPL::MAX_DEVICE_TYPES);
  
  for (int i = 0; i < ndevicetypes; i++) {
    HPL::Device_t dt = static_cast<HPL::Device_t>(i);
    if( (DefaultPlatform[i] != HPL::MAX_PLATFORMS) && (cl.getDeviceNumber(DefaultPlatform[i], dt) > 1) ){
      eval(DefaultPlatform[i], dt);
    }
  }
  std::cout << std::endl;
  for (int i = 0; i < ndevicetypes; i++) {
    HPL::Device_t dt = static_cast<HPL::Device_t>(i);
    if( (DefaultPlatform[i] != HPL::MAX_PLATFORMS) ){
      eval_subb(DefaultPlatform[i], dt);
    }
  }

}
