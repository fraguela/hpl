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
/// \file     FHandle.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cstring>
#include <cctype>
#include "FHandle.h"
#include "Array.h"
#include <iostream>

namespace HPL {
  
  FHandle::FHandle()
  : numArgs_(0), name_(TheGlobalState().getNewID()), is_fprint(false), isNative_(false)
  { }
  
   /** @todo as the name is user-provided, it could collide with HPL generated names.. */
  FHandle::FHandle(const String_t& name, const String_t& native_code)
  : numArgs_(0), name_(name), code_(native_code), is_fprint(false), isNative_(true)
  { }
  
  void FHandle::fillIn(int num_args)
  {
    if(num_args > MAX_KERNEL_NARG) {
      char aux_message[256];
      sprintf(aux_message, "Exception thrown in FHandle::fillIn: cannot handle more than %d arguments per function", MAX_KERNEL_NARG);
      throw HPLException(0, aux_message);
    }
    
    numArgs_ = num_args;
    
    if(!isNative_)
      TheGlobalState().getCodifier().state(BuildingFHeader);
  }

  void FHandle::endDefinition(const char *rettypestr)
  { static const char * const kernelvoidstr = "__kernel void";

    const String_t fbody = TheGlobalState().getCodifier().endDefinition();

    //If \p double keyword is found in the code we enable the double precision.

    if(utils::findWord(fbody, "double"))
       TheGlobalState().addPragma(GlobalState::CL_KHR_FP64);
    
    // (kernelvoidstr + 9) points to "void"
    const String_t rettype( (!rettypestr || !strcmp(rettypestr, kernelvoidstr + 9)) ? kernelvoidstr : rettypestr);

    code_ = rettype + " " + name_ + fbody;
  }
  
  void FHandle::setTable(std::initializer_list<HPL::AccessType> init_list)
  {
    const int num_args = static_cast<int>(init_list.size());
    
    if(num_args > MAX_KERNEL_NARG) {
      char aux_message[256];
      sprintf(aux_message, "Exception thrown in FHandle::set: cannot handle more than %d arguments per function", MAX_KERNEL_NARG);
      throw HPLException(0, aux_message);
    }
    
    numArgs_ = num_args;
    
    const HPL::AccessType* const it_end = init_list.end();
    for(const HPL::AccessType *it = init_list.begin(); it != it_end; ++it) {
      int i = num_args - static_cast<int>(it_end - it);
      switch (*it) {
	case HPL_RD :
	  fhandleTable[i].isWrite =  false;
	  fhandleTable[i].isWriteBeforeRead = false;
	  break;
	case HPL_WR:
	  fhandleTable[i].isWrite =  true;
	  fhandleTable[i].isWriteBeforeRead = true;
	  break;
	case HPL_RDWR:
	  fhandleTable[i].isWrite =  true;
	  fhandleTable[i].isWriteBeforeRead = false;
	  break;
	default:
	  assert(0);
      }
    }
  }
  
  void FHandle::print()
  {
    std::cout << "Function " << name_ << " args: " <<  numArgs_ << std::endl;
    std::cout << "code: " << code_ << std::endl;
  }
  
  void FHandle::fprint()
  {
    // is only written kernel file once
    if(!is_fprint) {
      TheGlobalState().dumpCode(code_);
      is_fprint = true;
    }
  }

  void FHandle::copyFormalArrayInfo(const int argnum, const BaseArray *arg)
  {
      const ArrayInfo * const arinfo = TheGlobalState().getArrayInfo(arg);
      fhandleTable[argnum].isWrite = arinfo->get_isWritten();
      fhandleTable[argnum].isWriteBeforeRead = arinfo->get_isWrittenBeforeRead();
  }

  void FHandle::checkPrivateArgs()
  {
	  std::size_t head_beg  = code_.find("__kernel ", 0);
	  if(head_beg!=-1)
	  {
		  std::size_t head_end  = code_.find("{", 0);
		  std::string subs = code_.substr(head_beg, head_end - head_beg);
		  if(subs.find("__private",0)!=-1)
		  {
			  code_.replace(0,9,"");
		  }
	  }
  }

};

