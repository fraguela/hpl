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
/// \file     HDM.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef HDM_H_
#define HDM_H_

#include "HPL_utils.h"
#include "CLbinding.h"

namespace HPL {
  const unsigned int HPL_MAX_DEVICE_COUNT = 8;

	/// Argument Intend
  enum ArgIntend_t { HPL_ARG_INTEND_NO = -1, HPL_ARG_INTEND_IN = 0, HPL_ARG_INTEND_INOUT = 1, HPL_ARG_INTEND_OUT = 2 };

	/**
	 * @brief This class presents an internal representation
	 * of HDM(   Host Device Memories )
	 */
	class HDM {
	  public:

	  /**
	   * @brief HDM constructor.
	   */
	  HDM();

	  void add_dbuffer( Platform_t platform_id, Device_t device_type, int device_n, cl::Buffer *buffer );

	  cl::Buffer *get_dbuffer( Platform_t platform_id, Device_t device_type, int device_n );

	  bool get_hostValid();

	  void set_hostValid( bool value );

	  bool get_deviceValid( Platform_t platform_id, Device_t device_type, int device_n );

	  void set_deviceValid( Platform_t platform_id, Device_t device_type, int device_n, bool value );

//	  bool is_generate_copy_in( Platform_t platform_id, Device_t device_type, int device_n, BaseArray* basearray);
//	  bool test_copy_in( Platform_t platform_id, Device_t device_type, int device_n, BaseArray* basearray);
//
//	  bool is_generate_copy_out( Platform_t platform_id, Device_t device_type, int device_n);

	  ArgIntend_t get_argIntend();
	  
	  void set_argIntend( ArgIntend_t value );

	  void store_argIntend(bool isWrite, bool isWriteBeforeRead);

	  void invalidate_dbuffers();          

	  void free_dbuffers();
	  
	  void set_device(Platform_t Platform_t, Device_t device_type, int device_number);
	  
//	  void force_validate_basearray(BaseArray *basearray);

	  Platform_t get_device_platform() const;
	  Device_t get_device_type() const;
	  int get_device_number() const;

	  ////////////////////////////////BEGIN MEMORY COHERENCY///////////////////////////////////////

	  void invalidate(Platform_t platform_id, Device_t device_type, int device_n);
	  void where(Platform_t *platform_id, Device_t *device_type, int* device_n) const;
	  void readWrite(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq, InternalMultiDimArray* ba);
	  bool isOnlyCopy(Platform_t platform_id, Device_t device_type, int device_number) const;
	  void onlyWrite(Platform_t platform_id, Device_t device_type, int device_n);
	  void onlyRead(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq, InternalMultiDimArray* ba);
	  void enqueueCopy(Platform_t p_id, Device_t d_type, int d_n, cl::CommandQueue *cq, InternalMultiDimArray* ba);
	  void enqueueMap(BaseArray* ba);
	  void where_primero(Platform_t *platform_id, Device_t *device_type, int* device_n) const;
	  //////////////////////////////////END MEMORY COHERENCY///////////////////////////////////////

	  int numberof_copy_in;                  ///< keeps total number of copy_in performed for internal performance testing.
	  int numberof_copy_out;                 ///< keeps total number of copy_out performed for internal performance testing.	  

	private:
	  
	  //bool  hostValid;         ///< true if the BaseArray has valid data.
	  ArgIntend_t argIntend;
	  cl::Buffer *dbuffer[MAX_PLATFORMS+1][MAX_DEVICE_TYPES][HPL_MAX_DEVICE_COUNT]; ///< keeps device buffer for each device
	  bool deviceValid[MAX_PLATFORMS+1][MAX_DEVICE_TYPES][HPL_MAX_DEVICE_COUNT];    ///< true if the device has valid copy data.
	  Platform_t platform_id; ///< keeps the type of the last device used
	  Device_t device_type;    ///< keeps the type of the last device used
	  int device_number;       ///< keeps the number of the last device used

	};

}


#endif
