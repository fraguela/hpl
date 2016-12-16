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
/// \file     HDM.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include "HDM.h"
#include "Array.h"
#include "FRunner.h"
#include "exceptions/HPLException.hpp"

//#define LOG(...)   do{ std::cerr << __VA_ARGS__ << std::endl; }while(0)

namespace HPL {

  using namespace cl;
  
  HDM::HDM()
  : numberof_copy_in(0), numberof_copy_out(0),
    argIntend(HPL_ARG_INTEND_NO),
    platform_id((HPL::Platform_t)/*-1*/MAX_PLATFORMS),
    device_type((HPL::Device_t)/*-1*/0), device_number(/*-1*/0)
  {

	  for(int j = 0; j <= MAX_PLATFORMS; j++)
	  {
		  for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
			  for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
				  dbuffer[j][k][i] = NULL;
				  deviceValid[j][k][i] = false;
			  }
		  }
	  }
	deviceValid[MAX_PLATFORMS][0][0] = true;
  }
  
  void HDM::add_dbuffer( Platform_t p_id, Device_t d_type, int d_n, Buffer *buffer)
  {  
    dbuffer[p_id][d_type][d_n] = buffer;
  }


  Buffer *HDM::get_dbuffer( Platform_t p_id, Device_t d_type, int d_n)
  {
    return dbuffer[p_id][d_type][d_n];
  }

  bool HDM::get_hostValid()
  {
    return deviceValid[MAX_PLATFORMS][0][0];
  }

  void HDM::set_hostValid( bool value )
  {
    deviceValid[MAX_PLATFORMS][0][0] = value;
  }

  bool HDM::get_deviceValid( Platform_t p_id, Device_t d_type, int d_n )
  {
    return deviceValid[p_id][d_type][d_n];
  }

  void HDM::set_deviceValid( Platform_t p_id, Device_t d_type, int d_n, bool value )
  {
    deviceValid[p_id][d_type][d_n] = value;
  }

  ArgIntend_t HDM::get_argIntend()
  {
    return argIntend;
  }

  void HDM::set_argIntend( ArgIntend_t value )
  {
    argIntend = value;
  }

  void HDM::free_dbuffers()
  {
          for(int j = 0; j <= MAX_PLATFORMS; j++)
                  for(int k = 0; k < MAX_DEVICE_TYPES; k++)
                          for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++)
                                  if(dbuffer[j][k][i] != NULL) delete(dbuffer[j][k][i]);

  }

  void HDM::store_argIntend(bool isWrite, bool isWriteBeforeRead)
  {
    ArgIntend_t argintend = HPL_ARG_INTEND_INOUT;
    if(isWrite) {
      if(isWriteBeforeRead)
	argintend = HPL_ARG_INTEND_OUT; // The array a is written before being read. 
      else
	argintend = HPL_ARG_INTEND_INOUT; // The array a is read before written.
    }
    else {
      argintend = HPL_ARG_INTEND_IN; //The array a is only read. 

      // we need to propagate argIntend from previous frunner. Reason:
      // First call: eval(foo)(a,b) a.out, b.in
      // Second call: eval(foo)(b,a) a is changed at previous call.
//      if( this->argIntend == HPL_ARG_INTEND_INOUT || this->argIntend == HPL_ARG_INTEND_OUT )
//	return;
    }
    
    set_argIntend(argintend);
  }

  // This will invalidate all device buffers copies.
  void HDM::invalidate_dbuffers()
  {
//    int nGPUs = get_GPUs_number();
//    int nCPUs = get_CPUs_number();
//    for(int i = 0; i < nGPUs; i++) {
//      set_deviceValid(CL_DEVICE_TYPE_GPU, i, false);
//    }
//    for(int i = 0; i < nCPUs; i++) {
//      set_deviceValid(CL_DEVICE_TYPE_CPU, i, false);
//    }
    
    
    for(int j = 0; j < MAX_PLATFORMS; j++) {
	    for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
		  for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
			  set_deviceValid((HPL::Platform_t)j, (HPL::Device_t)k, i, false);
		  }
	  }
    }
    deviceValid[MAX_PLATFORMS][0][0] = true;
  }


 // This will validate basearray.
//  void HDM::validate_basearray(BaseArray *basearray)
//  {
//    if(get_hostValid() == true) return;  // basearray is valid.
//
//    if(device_number == -1) throw HPLException(0, "Exception thrown in HDM::validate_basearray has   null frunner");
////    bool saved_state_async = TheGlobalState().clbinding().is_async;
////    TheGlobalState().clbinding().is_async = false;   // This should be sync, needs data right now.
//    TheGlobalState().clbinding().copy_from_buffer_to_basearray(platform_id,device_type,device_number, (InternalMultiDimArray*)basearray);
////    TheGlobalState().clbinding().is_async = saved_state_async;
//    set_hostValid(true);
//  }

  void HDM::set_device(Platform_t p_id, Device_t d_type, int d_number)
  {
	  this->platform_id = p_id;
	  this->device_type = d_type;
	  this->device_number = d_number;
  }

//  void HDM::force_validate_basearray(BaseArray *basearray)
//  {
//	  if(get_hostValid() == true) return;
//
//
//	    for(int j = 0; j < MAX_PLATFORMS; j++)
//	    {
//	      for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
//			for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
//				if(basearray->hdm.deviceValid[j][k][i] == true)
//				{
//				    TheGlobalState().clbinding().copy_from_buffer_to_basearray((HPL::Platform_t)j, (HPL::Device_t)k, i, (InternalMultiDimArray*)basearray);
//				    set_hostValid(true);
//				    return;
//				}
//			}
//	      }
//	    }
//
//  }

  Platform_t HDM::get_device_platform() const
  {
	  return platform_id;
  }

  Device_t HDM::get_device_type() const
  {
	  return device_type;
  }

  int HDM::get_device_number() const
  {
	  return device_number;
  }

  ////////////////////////////////BEGIN MEMORY COHERENCY///////////////////////////////////////

  void HDM::invalidate(Platform_t p_id, Device_t d_type, int d_n)
  {
//	    if (deviceValid[p_id][d_type][d_n]) {
//	      std::cout << "Invalidating Array in D ("  << p_id << "," << d_type << ","<< d_n << ")" << std::endl;
//	    }
	    deviceValid[p_id][d_type][d_n] = false;
  }

  void HDM::readWrite(Platform_t p_id, Device_t d_type, int d_n, cl::CommandQueue *cq, InternalMultiDimArray* ba)
  {
	  Platform_t w_platform_id;
	  Device_t w_device_type;
	  int w_device_n;

	  if(!deviceValid[p_id][d_type][d_n])
	  {
		  where(&w_platform_id, &w_device_type, &w_device_n);

		  if(p_id == MAX_PLATFORMS)
			  TheGlobalState().clbinding().copy_from_buffer_to_basearray(w_platform_id,w_device_type,w_device_n, (InternalMultiDimArray*)ba);

		  else
		  {
			  if(/*TheGlobalState().clbinding().allow_subbuffer(p_id,d_type) &&*/ ba->getFather()!=NULL && ba->getFather()->hdm.get_dbuffer( p_id, d_type, d_n) !=NULL)
			  {
				  TheGlobalState().clbinding().copy_in_s( cq, (InternalMultiDimArray*)ba, ba->getFather()->hdm.get_dbuffer(p_id,d_type,d_n));
			  }
			  else
				  TheGlobalState().clbinding().copy_in( cq, (InternalMultiDimArray*)ba, dbuffer[p_id][d_type][d_n], p_id, d_type,d_n);

		  }
	  }

	  for(int j = 0; j <= MAX_PLATFORMS; j++) {
		  for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
			  for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
				  if((j != p_id) | (k != d_type) | (d_n != i)) invalidate((Platform_t)j,(Device_t)k,i);
			  }
		  }
	  }
	  deviceValid[p_id][d_type][d_n] = true;
  }

  void HDM::enqueueMap(BaseArray* ba)
  {
          Platform_t w_platform_id;
          Device_t w_device_type;
          int w_device_n;

                  where_primero(&w_platform_id, &w_device_type, &w_device_n);
if(w_platform_id != MAX_PLATFORMS)
{
                          TheGlobalState().clbinding().enqueueMap(w_platform_id,w_device_type,w_device_n, (InternalMultiDimArray*)ba);
}
  }


  void HDM::onlyWrite(Platform_t p_id, Device_t d_type, int d_n)
  {
    for(int j = 0; j <= MAX_PLATFORMS; j++) {
	    for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
		  for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
			  	  if((j != p_id) | (k != d_type) | (d_n != i)) invalidate((Platform_t)j,(Device_t)k,i);
		  }
	    }
    }
    deviceValid[p_id][d_type][d_n] = true;
  }

  void HDM::onlyRead(Platform_t p_id, Device_t d_type, int d_n, cl::CommandQueue *cq, InternalMultiDimArray* ba)
  {
	  Platform_t w_platform_id;
	  Device_t w_device_type;
	  int w_device_n;

	  if(!deviceValid[p_id][d_type][d_n])
	  {
		  where(&w_platform_id, &w_device_type, &w_device_n);
		  if(p_id == MAX_PLATFORMS)
		  {
			  TheGlobalState().clbinding().copy_from_buffer_to_basearray(w_platform_id,w_device_type,w_device_n, (InternalMultiDimArray*)ba);
		  }
		  else
		  {
			  if(/*TheGlobalState().clbinding().allow_subbuffer(p_id,d_type) &&*/ ba->getFather()!=NULL && ba->getFather()->hdm.get_dbuffer( p_id, d_type, d_n) !=NULL)
			  {

				  TheGlobalState().clbinding().copy_in_s( cq, (InternalMultiDimArray*)ba, ba->getFather()->hdm.get_dbuffer(p_id,d_type,d_n));
			  }
			  else
			  {

				  TheGlobalState().clbinding().copy_in( cq, (InternalMultiDimArray*)ba, dbuffer[p_id][d_type][d_n], p_id, d_type, d_n);
			  }
		  }
	  }

	  deviceValid[p_id][d_type][d_n] = true;
  }

  void HDM::enqueueCopy(Platform_t p_id, Device_t d_type, int d_n, cl::CommandQueue *cq, InternalMultiDimArray* ba)
  {
	  Platform_t w_platform_id;
	  Device_t w_device_type;
	  int w_device_n;
	  if(!deviceValid[p_id][d_type][d_n])
	  {
		  cl_int ecode = CL_INVALID_VALUE;
		  where(&w_platform_id, &w_device_type, &w_device_n);
		  if(ba->getFather()!=NULL)
		  {
			  if((ba->getFather()->hdm.get_dbuffer( p_id, d_type, d_n) ==NULL) && (ba->getFather()->hdm.get_dbuffer( w_platform_id, w_device_type, w_device_n) ==NULL))
			  {
				  cl::Buffer *dbuffer_dst = ba->hdm.get_dbuffer(p_id, d_type, d_n);
				  cl::Buffer *dbuffer_src = ba->hdm.get_dbuffer(w_platform_id, w_device_type, w_device_n);
				  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueue(w_platform_id, w_device_type, w_device_n);
				  queue_aux->finish();
				  ecode = cq->enqueueCopyBuffer(*dbuffer_src, *dbuffer_dst, 0,0, ((InternalMultiDimArray*)ba)->getSize()*ba->getDataItemSize());
			  }
			  if((ba->getFather()->hdm.get_dbuffer( p_id, d_type, d_n) ==NULL) && (ba->getFather()->hdm.get_dbuffer( w_platform_id, w_device_type, w_device_n) !=NULL) /*&& TheGlobalState().clbinding().allow_subbuffer(w_platform_id,w_device_type)*/)
			  {
				  cl::Buffer *dbuffer_dst = ba->hdm.get_dbuffer(p_id, d_type, d_n);
				  cl::Buffer *dbuffer_src = ba->getFather()->hdm.get_dbuffer(w_platform_id, w_device_type, w_device_n);
				  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueue(w_platform_id, w_device_type, w_device_n);
				  queue_aux->finish();
				  ecode = cq->enqueueCopyBuffer(*dbuffer_src, *dbuffer_dst, ba->getTX().origin*ba->getDataItemSize(), 0, ((InternalMultiDimArray*)ba)->getXYZDim(0)*ba->getDataItemSize());
				  ///offset en source

			  }
			  if((ba->getFather()->hdm.get_dbuffer( p_id, d_type, d_n) !=NULL) && (ba->getFather()->hdm.get_dbuffer( w_platform_id, w_device_type, w_device_n) ==NULL))
			  {
				  cl::Buffer *dbuffer_dst = ba->getFather()->hdm.get_dbuffer(p_id, d_type, d_n);
				  cl::Buffer *dbuffer_src = ba->hdm.get_dbuffer(w_platform_id, w_device_type, w_device_n);
				  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueue(w_platform_id, w_device_type, w_device_n);
				  queue_aux->finish();
				  ecode = cq->enqueueCopyBuffer(*dbuffer_src, *dbuffer_dst, 0, ba->getTX().origin*ba->getDataItemSize(), ((InternalMultiDimArray*)ba)->getXYZDim(0)*ba->getDataItemSize());
			  }
			  if((ba->getFather()->hdm.get_dbuffer( p_id, d_type, d_n) !=NULL)&&/*TheGlobalState().clbinding().allow_subbuffer(p_id,d_type) &&*/ (ba->getFather()->hdm.get_dbuffer( w_platform_id, w_device_type, w_device_n) !=NULL) /*&& (TheGlobalState().clbinding().allow_subbuffer(w_platform_id, w_device_type))*/)
			  {
				  ///offset en dest y src
				  cl::Buffer *dbuffer_dst = ba->getFather()->hdm.get_dbuffer(p_id, d_type, d_n);
				  cl::Buffer *dbuffer_src = ba->getFather()->hdm.get_dbuffer(w_platform_id, w_device_type, w_device_n);
				  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueue(w_platform_id, w_device_type, w_device_n);
				  queue_aux->finish();
				  ecode = cq->enqueueCopyBuffer(*dbuffer_src, *dbuffer_dst, ba->getTX().origin*ba->getDataItemSize(), ba->getTX().origin*ba->getDataItemSize(), ((InternalMultiDimArray*)ba)->getXYZDim(0)*ba->getDataItemSize());
			  }
			  if (ecode != CL_SUCCESS)
				  throw HPLException(ecode, "Exception thrown in HDM::CommandQueue::enqueueCopyBuffer: failed");
		  }
		  else
		  {
			  cl::Buffer *dbuffer_dst = ba->hdm.get_dbuffer(p_id, d_type, d_n);
			  cl::Buffer *dbuffer_src = ba->hdm.get_dbuffer(w_platform_id, w_device_type, w_device_n);
              cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueue(w_platform_id, w_device_type, w_device_n);
              queue_aux->finish();
			  ecode = cq->enqueueCopyBuffer(*dbuffer_src, *dbuffer_dst, 0,0, ((InternalMultiDimArray*)ba)->getSize()*ba->getDataItemSize());
			  if (ecode != CL_SUCCESS)
				  throw HPLException(ecode, "Exception thrown in HDM::CommandQueue::enqueueCopyBuffer: failed");
		  }
	  }
	  deviceValid[p_id][d_type][d_n] = true;

  }


  void HDM::where(Platform_t* p_id, Device_t* d_type, int* d_n) const
  {

    for(int j = 0; j <= MAX_PLATFORMS; j++) {
            for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
                  for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
                          if(deviceValid[(HPL::Platform_t)j][(HPL::Device_t)k][i]) { p_id[0] = (Platform_t)j, d_type[0] = (Device_t)k, d_n[0] = i;};
                  }
          }
    }

  }

  void HDM::where_primero(Platform_t* p_id, Device_t* d_type, int* d_n) const
  {

    for(int j = 0; j <= MAX_PLATFORMS; j++) {
            for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
                  for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
                          if(deviceValid[(HPL::Platform_t)j][(HPL::Device_t)k][i]) { p_id[0] = (Platform_t)j, d_type[0] = (Device_t)k, d_n[0] = i; return;};
                  }
          }
    }

  }


  bool HDM::isOnlyCopy(Platform_t p_id, Device_t d_type, int d_number) const
  {
    assert(deviceValid[p_id][d_type][d_number]);

    for(int j = 0; j <= MAX_PLATFORMS; j++) {
	    for(int k = 0; k < MAX_DEVICE_TYPES; k++) {
		  for(int i = 0; i < HPL_MAX_DEVICE_COUNT; i++) {
			  if(deviceValid[(HPL::Platform_t)j][(HPL::Device_t)k][i] && (((HPL::Platform_t)j != p_id) | ((HPL::Device_t)k!=d_type) | (i != d_number)))
				  return false;
		  }
	  }
    }
    return true;
  }

  //////////////////////////////////END MEMORY COHERENCY///////////////////////////////////////

}




