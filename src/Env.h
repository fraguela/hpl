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
/// \file     Env.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef ENV_H
#define ENV_H

namespace HPL {
    
  class Env {

  public:
    
    Env();

    bool hpl_kernel_file_enable;        //if true, the kernel file is generated.
    bool hpl_opt_copy_out_disable;      //if true, the system copy in and copy out every args.
    bool hpl_kernel_finish_enable;      //if true, finish will be called for the kernel
    bool hpl_unifiedmemory_enable;      //if true, will use unified memory
    bool hpl_print_copy;                //if true, print copy info
    const char *hpl_config_file;        //HPL configuration file

    void print_all_env();
  };
  
}

#endif
