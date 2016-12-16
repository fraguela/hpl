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
/// \file     Device.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include "Device.h"
#include "Array.h"

namespace HPL {
  
  const char * const PlatformName[MAX_PLATFORMS] = { "AMD", "Apple", "Intel", "NVIDIA" };
  
  Device::Device()
  : d_(TheGlobalState().clbinding().defaultDevice())
  { }

  Device::Device(HPL::Device_t device_t, int device_n)
  : d_(TheGlobalState().clbinding().defaultDevice(device_t, device_n))
  { }

  Device::Device(HPL::Platform_t platform_id, HPL::Device_t device_t, int device_n)
  : d_(TheGlobalState().clbinding().getDevice(platform_id, device_t, device_n))
  { }
  
  void Device::sync()
  {
    TheGlobalState().clbinding().sync(d_);
  }
  
  const String_t& Device::getGenericName(Device_t type)
  { static String_t Name[] = { "CPU", "GPU", "ACCELERATOR" };
    
    return Name[type];
  }
  
  /*
  cl_device_type Device::getGenericClDeviceType(Device_t type)
  {
    return (type == GPU) ? CL_DEVICE_TYPE_GPU: CL_DEVICE_TYPE_CPU;
  }
  */
  
  void Device::getProperties(DeviceProperties& dp) const
  {
    TheGlobalState().clbinding().getDeviceProperties(d_, dp);
  }

  bool Device::supportsDoubles() const
  { DeviceProperties dp;

    TheGlobalState().clbinding().getDeviceProperties(d_, dp);
    std::string::size_type tmp = dp.extensions.find("cl_khr_fp64");
    return (tmp != std::string::npos);
  }
  
}




