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
/// \file     Env.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "Env.h"

namespace HPL {

  /*
   *   This method is to get all env variables.
  */
  Env::Env()
  { char *value;

    /*   ENV 1 */
    value =  getenv("HPL_KERNEL_FILE_ENABLE");
    hpl_kernel_file_enable = ( value != NULL ) ? true : false;

    /*   ENV 2 */
    value =  getenv("HPL_OPT_COPY_OUT_DISABLE");
    hpl_opt_copy_out_disable = ( value != NULL ) ? true : false;


    /*   ENV 3 */
    value =  getenv("HPL_KERNEL_FINISH_ENABLE");
    hpl_kernel_finish_enable = ( value != NULL ) ? true : false;

    /*   ENV 4 */
    value =  getenv("HPL_PRINT_COPY");
    hpl_print_copy = ( value != NULL ) ? true : false;

    /*   ENV 5 */
    value =  getenv("HPL_UNIFIEDMEMORY_ENABLE");
    hpl_unifiedmemory_enable = ( value != NULL ) ? true : false;

    /*  ENV 6 */
    hpl_config_file = getenv("HPL_CONFIG_FILE");
  }

  /*
   *   This method is to print all env variables.
  */
  void Env::print_all_env()
  {
    std::cout << "HPL_KERNEL_FILE_ENABLE = " << hpl_kernel_file_enable << std::endl;
    std::cout << "HPL_OPT_COPY_OUT_DISABLE = " << hpl_opt_copy_out_disable << std::endl;
    std::cout << "HPL_KERNEL_FINISH_ENABLE = " << hpl_kernel_finish_enable << std::endl;
    std::cout << "HPL_PRINT_COPY = " << hpl_print_copy << std::endl;
    std::cout << "HPL_UNIFIEDMEMORY_ENABLE = " << hpl_print_copy << std::endl;
  }
}
