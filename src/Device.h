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
/// \file     Device.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef DEVICE_H_
#define DEVICE_H_

#include "HPL_utils.h"

namespace HPL {
  
typedef struct {
  unsigned int maxDimensions;
  size_t maxLocalDomainSizes[3];
  size_t maxLocalDomainSize;
  unsigned long long globalMemSize;
  unsigned long long localMemSize;
  bool dedicatedLocalMem;
  bool unifiedMemory;
  unsigned long long maxConstantBufferSize;
  unsigned long long maxMemAllocSize;
  unsigned long long globalMemCacheSize;
  AccessType globalMemCacheType;
  unsigned int computeUnits;
  std::string deviceName;
  std::string deviceVendor;
  std::string extensions;
} DeviceProperties;


  /// Device type allowed
  enum Device_t { CPU = 0, GPU = 1, ACCELERATOR=2, MAX_DEVICE_TYPES = 3 };
  
  
  /// Platform type allowed
  enum Platform_t { AMD = 0, APPLE=1, INTEL = 2, NVIDIA = 3, MAX_PLATFORMS = 4 };

  
  extern const char * const PlatformName[MAX_PLATFORMS];
  
  
  namespace internal {
    
    /// Unique combination of Platform_t + Device_t + device number that identifies a device
    struct DeviceDescriptor {
      Platform_t  platform_id_;
      Device_t    devt_;
      int         devn_;
    };
    
    struct DeviceImplementation;
  }
  
  
  /**
   * @brief Abstract representation of a device.
   */
  class Device {
    
    
  public:
    
    /**
     * @brief Device constructor.
     * @param[in] platform_t member of the enumeration HPL::Platform_t.
     * @param[in] device_t member of the enumeration HPL::Device_t.
     * @param[in] device_n number of the device that will be used.
     * If there are four devices, the numbers are 0, 1, 2 and 3.
     */
    Device(HPL::Platform_t platform_id, HPL::Device_t device_t, int device_n = 0);
    
    /**
     * @brief Device constructor. This version uses a default platform.
     * @param[in] device_t member of the enumeration HPL::Device_t.
     * @param[in] device_n number of the device that will be used.
     * If there are four devices, the numbers are 0, 1, 2 and 3.
     */
    Device(HPL::Device_t device_t, int device_n = 0);

    /// Default constructor
    Device();
    
    /**
     * @brief Complete all the task asigned in this device.
     */
    void sync();
    
    /**
     * @brief Returns the platform identifier
     */
    Platform_t getPlatformId() const { return ((internal::DeviceDescriptor *)d_)->platform_id_; }
    
    /**
     * @brief Returns the HPL device type    
     */
    Device_t getDeviceType() const { return ((internal::DeviceDescriptor *)d_)->devt_; }
    

    /**
     * @brief Returns the device number of the device.
     */
    int getDeviceNumber() const { return ((internal::DeviceDescriptor *)d_)->devn_; }
    
    static const String_t& getGenericName(Device_t type);
    
    //static cl_device_type getGenericClDeviceType(Device_t type);
    
    /**
     * @brief Returns in \ dP the properties of the device.
     * @param[out] dp deviceProperties object to fill with the properties of the device
     */
    void getProperties(DeviceProperties& dp) const;

    Device& operator= (const Device& other) { d_ = other.d_; return *this; }

    internal::DeviceImplementation *operator->() { return d_; }

    const internal::DeviceImplementation *operator->() const { return d_; }
    
    /// Report whether the device support double precision operations
    bool supportsDoubles() const;
    
  private:
    internal::DeviceImplementation * d_;
  };
  
}


#endif
