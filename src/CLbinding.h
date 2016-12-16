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
/// \file     CLbinding.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef CLBINDING_H
#define CLBINDING_H

#include"HPL_utils.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
//#include <map>

#define __NO_STD_STRING
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

/*
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
*/

#include "cl.hpp"

#include "exceptions/HPLException.hpp"

#include "Device.h"

#ifdef HPL_PROFILE
	#include <sys/time.h>
#endif

#define MAX_KERNEL_NARG 25

namespace HPL {

  //Forward declaration
  class BaseArray;

  //Forward declaration
  class InternalMultiDimArray;
  
  //Forward declaration
  class FHandle;

  //Forward declaration
  class FRunner;
  
  class KeyChildrenMap {
  public:
          KeyChildrenMap(int tXO, int tXE, int tYO, int tYE, int tZO, int tZE)
          {
                  this->tXO = tXO;
                  this->tXE = tXE;
                  this->tYO = tYO;
                  this->tYE = tYE;
                  this->tZO = tZO;
                  this->tZE = tZE;
          }

          bool operator<(const KeyChildrenMap &right) const {

                  if(tXO < right.tXO) return true;
                  else
                  {
                       	if(tXO == right.tXO)
                        {
                               	if(tXE < right.tXE) return true;
                                else
                               	{
                                       	if(tXE == right.tXE)
                                       	{
                                               	if(tYO < right.tYO) return true;
                                               	else
                                                {
                                                        if(tYO == right.tYO)
                                                       	{
                                                                if(tYE < right.tYE) return true;
                                                               	else
                                                                {
                                                                       	if(tYE == right.tYE)
                                                                       	{
                                                                               	if(tZO < right.tZO) return true;
                                                                                else
                                                                                {
                                                                                        if(tZO == right.tZO) return (tZE < right.tZE);
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                  }
                  return false;
          }

          int tXO;
          int tXE;
          int tYO;
          int tYE;
          int tZO;
          int tZE;
  };



//  /* This class is used to keep the number of basearrays which it is used for device syncronization.
//   * Device will cereive this basearray from fhandle and when device.sych() is used all of the arg list has to be syncronized with the host.
//   */
//  class ArgListForDevice {
//
//  public:
//    ArgListForDevice() {
//      nargs = 0;
//      for(int i = 0; i < MAX_KERNEL_NARG; i++)
//	fhandleTable[i].base_array = NULL;
//    };
//    struct arg_type {
//      BaseArray *base_array;		///< Pointer to where this array is allocated in host memory.
//    } fhandleTable[MAX_KERNEL_NARG];///< Contains the data of the arguments of the current kernel.
//    int nargs;						///< Number of arguments of the current kernel.
//  };

  /**
   * @brief This class presents a friendly interface to OpenCL.
   */
  class CLbinding {
    
    /**
     * @brief Empty constructor of the object that will manage all the
     * connections with OpenCL libraries.
     */
    CLbinding();
    
    ~CLbinding();

    /**
     * @brief Initialize the OpenCL platform, devices, contexts, queues, etc.
     */
    void platform_init();

    /**
     * @brief Create all arrays needed to executing the kernel.
     * All these arrays READ_WRITE.
     */
    void create_buffers(FRunner * const frunner);

    /**
     * @brief Make the memory copies between host arrays and device buffers.
     * @param[in] frunner Contains the execution environment to the FHandle associated.
     * If the selected device is not valid, the \p frunner object will change to
     * contains the new device selected.
     */
    void copy_to_buffers(FRunner * const frunner);
    
    /**
     * @brief Links the scalars modified with kernel arguments.
     *
     void set_kernel_args_lite(FRunner *frunner);
     */

    
  public:
    
    friend CLbinding& TheCLbinding();
    
    cl::Kernel* setup_and_notrun(FRunner * const frunner);
    /// Do everything needed to compile, set up arguments and run this FRunner
    void setup_and_run(FRunner * const frunner);

    /// Do everything needed to set up and run am already previously run FRunner 
    void setup_and_run_lite(FRunner * const frunner);
    
    /// Helper function to run external kernels
    cl::CommandQueue& external_setup(FRunner * const frunner);

    /**
     * @brief Finish(sync) the command queue on the selected device.
     * @param[in] device Device to synchronize
     */
    void sync(internal::DeviceImplementation *device);

    /**
     * @brief Returns the number of devices of a given kind.
     *
     * If there are several platforms for this kind of device, the maximum number of
     * devices found in any platform is returned.
     */
    unsigned int getDeviceNumber(Device_t type_n) const;
    
    /// Returns the number of devices of a given kind supported by a given platform
    unsigned int getDeviceNumber(Platform_t platform_id, Device_t type_n) const;
    
    void *allocMem(size_t buf_size, bool fast_mem);
    void freeMem(void *p);

    /**
     * @brief store_arglistfordevice is to save arglist for device sync.
     * @param[in] frunner handle.
     */
    //void store_arglistfordevice(FRunner *frunner);

    /**
     * @brief copy_from_buffer_to_basearray bring recent data from dbuffer to base_array.
     * @param[in] frunner handle.
     * @param[in] base_array.
     */
    void copy_from_buffer_to_basearray(Platform_t platform_id, Device_t device_type, int device, InternalMultiDimArray *base_array );

    /**
     * @brief copy_from_buffer_to_basearray_range bring recent data from a range of dbuffer to base_array
     * @param[in] platform_id Platform of the device where the data are allocated
     * @param[in] device_type Device type of the device where the data are allocated
     * @param[in] device Number of the device where the data are allocated
     * @param[in] base_array Array whose part of it host side will be updated
     * @param[in] ox X coordinate of the origin of the region to be updated in the device data.
     * @param[in] oy Y coordinate of the origin of the region to be updated in the device data.
     * @param[in] oz Z coordinate of the origin of the region to be updated in the device data.
     * @param[in] lx length in words of each row of the region (width).
     * @param[in] ly length in rows of each column of the region (height).
     * @param[in] lz length in slices of the region (depth).
     * @param[in] row_bytes Length in bytes of the each row of the dbuffer.
     * @param[in] slice_bytes Length in bytes of the each slice of the dbuffer.
     * @param[in] rango Pointer to the memory where the data will be copied to.
     */
    void copy_from_buffer_to_basearray_range(Platform_t platform_id, Device_t device_type, int device,
    		const InternalMultiDimArray *base_array, int ox, int oy, int oz, int lx, int ly, int lz, int row_bytes,
    		int slice_bytes, void* rango );

    void copy_from_buffer_to_basearray_range_offset(Platform_t platform_id, Device_t device_type,
                      int device, const InternalMultiDimArray *base_array, int ox, int oy, int oz, int lx, int ly, int lz,
                      int host_row_bytes, int host_slice_bytes, int row_bytes,
                      int slice_bytes, void* rango);

    /**
     * @brief copy_to_buffer_from_basearray_range Copy data pointed by rango to a range in the
     * device pointer of the base_array array.
     * @param[in] platform_id Platform of the device where the data are allocated
     * @param[in] device_type Device type of the device where the data are allocated
     * @param[in] device Number of the device where the data are allocated
     * @param[in] base_array Array whose part of it device side will be updated
     * @param[in] ox X coordinate of the origin of the region to be updated in the device data.
     * @param[in] oy Y coordinate of the origin of the region to be updated in the device data.
     * @param[in] oz Z coordinate of the origin of the region to be updated in the device data.
     * @param[in] lx length in words of each row of the region (width).
     * @param[in] ly length in rows of each column of the region (height).
     * @param[in] lz length in slices of the region (depth).
     * @param[in] row_bytes Length in bytes of the each row of the dbuffer.
     * @param[in] slice_bytes Length in bytes of the each slice of the dbuffer.
     * @param[in] rango Pointer to the memory where the data will be copied from.
     */
//    void copy_to_buffer_from_basearray_range(Platform_t platform_id, Device_t device_type, int device,
//        		const InternalMultiDimArray *base_array, int ox, int oy, int oz, int lx, int ly, int lz, int row_bytes,
//        		int slice_bytes, void* rango );

    void copy_to_buffer_from_basearray_range(Platform_t platform_id, Device_t device_type, int device,
                        const InternalMultiDimArray *base_array, int ox, int oy, int oz, int lx, int ly, int lz, int row_bytes,
                        int slice_bytes, int host_row_bytes, int host_slice_bytes, void* rango );

    void copy_to_buffer_from_basearray_range_offset(Platform_t platform_id, Device_t device_type, int device,
                        InternalMultiDimArray *base_array, int ox, int oy, int oz, int lx, int ly, int lz, int row_bytes,
                        int slice_bytes, void* rango );

    void enqueue_copy(Platform_t dst_platform_id, Device_t dst_device_type, int dst_device,
                       	Platform_t src_platform_id, Device_t src_device_type, int src_device, const InternalMultiDimArray *dst,
                       	const InternalMultiDimArray *src, int dst_offset, int src_offset, int bytes);

    /**
     * @brief Copy the buffers allocated on the selected device onto the arrays allocated
     * on the host. This correspondence is stored in the struct arg_type. For each argument used
     * in the kernel, there is an object of type arg_type that has, among others, two fields
     * named dbuffer and base_array. Well, the dbuffer is the pointer to the device memory space
     * where is allocated the kernel argument and base_array is the pointer to the host memory space.
     * @param[in] frunner Execution environment.
     */
    void copy_from_buffers(FRunner *frunner, bool enable = false );

    /**
     * @brief Finishes all remaining commands in the cl::clCommandQueue associated
     * and frees the memory.
     */
    void free_resources();
    bool get_copy_faster(Platform_t platform_id, Device_t device_type);
    bool allow_subbuffer(Platform_t platform_id, Device_t device_type);

    /*
     * @brief Calculate the total size of the kernel arguments based in the number of dimensions
     * and the elements by dimension.
     
    void calculate_args_sizes(FHandle *fhandle);

    int calculate_BaseArray_size(BaseArray *base_array);
    */
    
    void copy_in( cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer,Platform_t p_id, Device_t d_type, int d_n, int start = 0, int length = -1);

    void copy_out( cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer, Platform_t p_id, Device_t d_type, int d_n, int start = 0, int length = -1);
    void copy_in_s( cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer, int start = 0, int length = -1);
    void enqueueMap(Platform_t platform_id, Device_t device_type, int device_n, InternalMultiDimArray* base_array);
    void copy_out_s( cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer, int start = 0, int length = -1);

    cl::Buffer  *allocate_dsubbuffer( Platform_t platform_id, Device_t device_type, int device, InternalMultiDimArray *base_array );

    cl::Buffer *allocate_dbuffer(Platform_t platform_id, Device_t device_type, int device, InternalMultiDimArray *base_array );

    void readWrite(InternalMultiDimArray* base_array, Platform_t platform_id, Device_t device_type, int device);
    cl::CommandQueue* getCommandQueue(Platform_t platform_id, Device_t devt, int device_n) const;

    /**
     * @brief Mark the argument list for
     * alias.
     * @param[in] device Device to checking.
     */
    void handle_alias(FRunner* const frunner);

    internal::DeviceImplementation *defaultDevice() const;
    
    internal::DeviceImplementation *defaultDevice(Device_t devt, int device_n) const;
    
    internal::DeviceImplementation *getDevice(HPL::Platform_t platform_id, HPL::Device_t devt, int device_n) const;
    
    void getDeviceProperties(const internal::DeviceImplementation *p, DeviceProperties& dp) const;
    
    /// Erases internal IR code storage (allCode)
    void resetCode();
    
    String_t allCode; 	 			      ///< Keeps all kernel codes if the system does not use ::kernelfile.
    
    //    std::map<KeyProgramMap,ArgListForDevice*> arglistfordevice; ///< Map of the device which keep the last arglist from fhandle for this device.
    
    std::ofstream kernelfile; 			///< File containing the program code.
    
  };
 
  CLbinding& TheCLbinding();
}

#endif
