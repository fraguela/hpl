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
/// \file     FRunner.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include "FRunner.h"

namespace HPL {

  FRunner::FRunner(FHandle* ifhandle)
  : fhandle(ifhandle), is_multifrunner(0), exec_plan_(0)
  {
    device_.push_back(HPL::Device());
    nd_global.emplace_back(-1, -1, -1);
    nd_local.emplace_back(0, 0, 0);

    fhandle->fprint();
  }

  void FRunner::run()
  {
    try {
      
      TheGlobalState().clbinding().setup_and_run_lite(this);

    } catch(HPLException o){
      std::cout << o.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  
}
