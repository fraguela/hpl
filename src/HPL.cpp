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
/// \file     HPL.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cstdlib>
#include "HPL.h"
#include <sstream>

namespace HPL {
  
  void return_() {
    TheGlobalState().getCodifier().return_("");
  }
  
  void get_global_id(const Array<int, 0>& i)
  {
    assert(!HPL_STATE(NotBuilding));
    TheGlobalState().getCodifier().add(i.name() + " = get_global_id(0)", true);
  }
  
  void get_global_id(const Array<int, 0>& i, const Array<int, 0>& j)
  {
    assert(!HPL_STATE(NotBuilding));
    get_global_id(i);
    TheGlobalState().getCodifier().add(j.name() + " = get_global_id(1)", true);
  }
  
  void get_global_id(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k)
  {
    assert(!HPL_STATE(NotBuilding));
    get_global_id(i, j);
    TheGlobalState().getCodifier().add(k.name() + " = get_global_id(2)", true);
  }
 
  void get_local_id(const Array<int, 0>& i)
  {
    assert(!HPL_STATE(NotBuilding));
    TheGlobalState().getCodifier().add(i.name() + " = get_local_id(0)", true);
  }
  
  void get_local_id(const Array<int, 0>& i, const Array<int, 0>& j)
  {
    assert(!HPL_STATE(NotBuilding));
    get_local_id(i);
    TheGlobalState().getCodifier().add(j.name() + " = get_local_id(1)", true);
  }
  
  void get_local_id(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k)
  {
    assert(!HPL_STATE(NotBuilding));
    get_local_id(i, j);
    TheGlobalState().getCodifier().add(k.name() + " = get_local_id(2)", true);
  }

  void get_group_id(const Array<int, 0>& i)
  {
    assert(!HPL_STATE(NotBuilding));
    TheGlobalState().getCodifier().add(i.name() + " = get_group_id(0)", true);
  }
  
  void get_group_id(const Array<int, 0>& i, const Array<int, 0>& j)
  {
    assert(!HPL_STATE(NotBuilding));
    get_group_id(i);
    TheGlobalState().getCodifier().add(j.name() + " = get_group_id(1)", true);
  }
  
  void get_group_id(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k)
  {
    assert(!HPL_STATE(NotBuilding));
    get_group_id(i, j);
    TheGlobalState().getCodifier().add(k.name() + " = get_group_id(2)", true);
  }

  void get_num_groups(const Array<int, 0>& i)
  {
    assert(!HPL_STATE(NotBuilding));
    TheGlobalState().getCodifier().add(i.name() + " = get_num_groups(0)", true);
  }
  
  void get_num_groups(const Array<int, 0>& i, const Array<int, 0>& j)
  {
    assert(!HPL_STATE(NotBuilding));
    get_num_groups(i);
    TheGlobalState().getCodifier().add(j.name() + " = get_num_groups(1)", true);
  }
  
  void get_num_groups(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k)
  {
    assert(!HPL_STATE(NotBuilding));
    get_num_groups(i, j);
    TheGlobalState().getCodifier().add(k.name() + " = get_num_groups(2)", true);
  }
  
  void get_global_size(const Array<int, 0>& i)
  {
    assert(!HPL_STATE(NotBuilding));
    TheGlobalState().getCodifier().add(i.name() + " = get_global_size(0)", true);
  }
  
  void get_global_size(const Array<int, 0>& i, const Array<int, 0>& j)
  {
    assert(!HPL_STATE(NotBuilding));
    get_global_size(i);
    TheGlobalState().getCodifier().add(j.name() + " = get_global_size(1)", true);
  }
  
  void get_global_size(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k)
  {
    assert(!HPL_STATE(NotBuilding));
    get_global_size(i, j);
    TheGlobalState().getCodifier().add(k.name() + " = get_global_size(2)", true);
  }
  
  void get_local_size(const Array<int, 0>& i)
  {
    assert(!HPL_STATE(NotBuilding));
    TheGlobalState().getCodifier().add(i.name() + " = get_local_size(0)", true);
  }
  
  void get_local_size(const Array<int, 0>& i, const Array<int, 0>& j)
  {
    assert(!HPL_STATE(NotBuilding));
    get_local_size(i);
    TheGlobalState().getCodifier().add(j.name() + " = get_local_size(1)", true);
  }
  
  void get_local_size(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k)
  {
    assert(!HPL_STATE(NotBuilding));
    get_local_size(i, j);
    TheGlobalState().getCodifier().add(k.name() + " = get_local_size(2)", true);
  }
  
  void barrier(Sync_t flags)
  { String_t sflags;

    assert(!HPL_STATE(NotBuilding));
    switch (flags) {
      case LOCAL: sflags = "CLK_LOCAL_MEM_FENCE)"; break;
      case GLOBAL: sflags = "CLK_GLOBAL_MEM_FENCE)"; break;
      default:
        assert(static_cast<int>(flags) == ((int)LOCAL|(int)GLOBAL));
        sflags = "CLK_LOCAL_MEM_FENCE|CLK_GLOBAL_MEM_FENCE)";

        break;
    }
    TheGlobalState().getCodifier().add("barrier(" + sflags, true);
  }

  unsigned int getDeviceNumber(Device_t type_n) 
  {
    return TheGlobalState().clbinding().getDeviceNumber(type_n);
  }

  unsigned int getDeviceNumber(Platform_t platform_id, Device_t type_n)
  {
    return TheGlobalState().clbinding().getDeviceNumber(platform_id, type_n);
  }

  String_t getDeviceInfo(Device_t type_n)
  {  DeviceProperties prop;
     std::stringstream ret;
     

     if (!getDeviceNumber(type_n))
       return "There are no available " + Device::getGenericName(type_n) + "s.\n";

     ret << "AVAILABLE " << Device::getGenericName(type_n) <<"s:\n";
     ret << "------------------------------------------------------------------------------\n"
         << "DEVICE_NUMBER\t\tDEVICE_VENDOR\t\tDEVICE_NAME\n"
         << "------------------------------------------------------------------------------\n";
     
     for(int i = 0; i < HPL::MAX_PLATFORMS; i++)
     {
       const unsigned int n = TheGlobalState().clbinding().getDeviceNumber((Platform_t)i, type_n);
    	 for(unsigned int j = 0; j< n; j++)
    	 {
	   HPL::Device d((Platform_t)i, type_n, j);
	   
	   d.getProperties(prop);
    		 
	   ret << j << "\t\t" << prop.deviceVendor << "\t\t" << prop.deviceName  << "\n";
	   ret << "------------------------------------------------------------------------------\n";
    	 }
     }

     return ret.str();
   };

  ProfilingData getProfile() { return TheGlobalState().getProfile(); }
  
  ProfilingData getTotalProfile() { return TheGlobalState().getTotalProfile(); }
  
}
