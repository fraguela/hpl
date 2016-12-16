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
/// \file     Array.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include "Array.h"

namespace HPL {

  Size_t idx, idy, idz, lidx, lidy, lidz, gidx, gidy, gidz;
  Size_t szx, szy, szz, lszx, lszy, lszz, ngroupsx, ngroupsy, ngroupsz;

  ArrayFlags checkFlags(ArrayFlags flags)
  {
    const ArrayFlags f = ArrayFlags((int)flags & 3);

    if(HPL_STATE(NotBuilding))
      assert(f != Private);
    else if (HPL_STATE(BuildingFHeader))
      assert(f != Private);
    else
      assert((f != Global) && (f != Constant));

    return flags;
  }
  
  BaseArray::BaseArray(ArrayFlags flags)
  : dbuffer_(NULL), name_(TheGlobalState().getNewID()),  flags_((TheGlobalState().getCodifierNestLevel() == 0) ? checkFlags(flags): ArrayFlags((int)flags & 3))
  { }

  BaseArray::BaseArray(const BaseArray& another)
  :  dbuffer_(NULL), hdm(another.hdm), name_(another.name_), flags_(another.flags_)
  { }
  
  // Some doubt on what to do exactly with the flags_ here.
  // If doactualcopy is true, the variable is not going to be printed, so flags_ does not matter
  BaseArray::BaseArray(const BaseArray& another, bool doactualcopy)
  : dbuffer_(NULL), name_(doactualcopy ? another.name_ : TheGlobalState().getNewID()), flags_(another.flags_)
  { 
    //std::cout << name_ << " from " << another.name_ << " actcopy=" << doactualcopy << "\n";
  }
  
  const String_t& BaseArray::storageName() const {
    static String_t __GlobalString("__global "),
                    __LocalString("__local "),
                    __ConstantString("__constant "),
                    __PrivateString("__private ");
    
    const ArrayFlags f = ArrayFlags((int)flags_ & 3);
    
    switch(f) {
      case Global:   return __GlobalString;
      case Local:    return __LocalString;
      case Constant: return __ConstantString;
      case Private:  return __PrivateString;
    }

    assert(0);
  }
 
//  void BaseArray::copy_in( bool copy ) {
//
//	  cl_device_type d_t_out;
//	  int d_d_out;
//	  int p_id_out;
//	  TheGlobalState().clbinding().check_device(NVIDIA,CL_DEVICE_TYPE_GPU, 0, &p_id_out, &d_t_out, &d_d_out);
//
//	  if( dbuffer_ == NULL )
//	  {
//		  dbuffer_ = TheGlobalState().clbinding().allocate_dbuffer(p_id_out, d_t_out, d_d_out, this);
//	  }
//
//	  if( copy == true )
//	  {
//		  cl_device_type d_t_in;
//		  int d_n_in;
//		  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueueObject(p_id_out,d_t_out, d_d_out);
//		  TheGlobalState().clbinding().copy_in(queue_aux,this, dbuffer_);
//	  }
//  }
//
//  void BaseArray::copy_out( bool copy ) {
//	  if( copy == true )
//	  {
//		  cl_device_type d_t_out;
//		  int d_d_out;
//		  int p_id_out;
//		  TheGlobalState().clbinding().check_device(NVIDIA,CL_DEVICE_TYPE_GPU, 0, &p_id_out, &d_t_out, &d_d_out);
//		  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueueObject(p_id_out, d_t_out, d_d_out);
//		  TheGlobalState().clbinding().copy_out(queue_aux, this, dbuffer_);
//	  }
//	  if( dbuffer_ != NULL ) {
//	    delete dbuffer_;
//	    dbuffer_ = NULL; // for the next round
//	  }
//  }
//
//  void BaseArray::copy_in( bool copy, HPL::Device device, int start, int length) {
//	  cl_device_type d_t_out;
//	  int d_d_out;
//	  int p_id_out;
//	  TheGlobalState().clbinding().check_device(device.get_platform_id(), device.get_device_type(), device.get_device_number(),
//						    &p_id_out, &d_t_out, &d_d_out);
//
//	  if( dbuffer_ == NULL ) dbuffer_ = TheGlobalState().clbinding().allocate_dbuffer(p_id_out, d_t_out,
//											  d_d_out, this);
//	  if( copy == true )
//	  {
//		  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueueObject(p_id_out,d_t_out,
//				  d_d_out);
//		  TheGlobalState().clbinding().copy_in(queue_aux,this, dbuffer_, start, length);
//	  }
//  }
//
//  void BaseArray::copy_out( bool copy, HPL::Device device, int start, int length) {
//	  if( copy == true )
//	  {
//		  cl_device_type d_t_out;
//		  int d_d_out;
//		  int p_id_out;
//		  TheGlobalState().clbinding().check_device(device.get_platform_id(), device.get_device_type(), device.get_device_number(),
//							    &p_id_out, &d_t_out, &d_d_out);
//
//		  cl::CommandQueue* queue_aux = TheGlobalState().clbinding().getCommandQueueObject(p_id_out,d_t_out, d_d_out);
//		  TheGlobalState().clbinding().copy_out(queue_aux, this, dbuffer_, start, length);
//	  }
//	  if(length == -1)
//	  {
//		  if( dbuffer_ != NULL ) {
//			  delete dbuffer_;
//			  dbuffer_ = NULL; // for the next round
//		  }
//	  }
//  }
  
  void BaseArray::validate(AccessType at) const {
    if( at == HPL_NONE ) return;
///////////////HOST READS
    if(at == HPL_RD){  ((InternalMultiDimArray*)this)->onlyRead((Platform_t)MAX_PLATFORMS, (Device_t)0, 0, NULL);}
///////////////HOST READ AND WRITE
    if(at == HPL_RDWR) TheGlobalState().clbinding().readWrite((InternalMultiDimArray*)this, (Platform_t)MAX_PLATFORMS, (Device_t)0, 0);
///////////////HOST ONLY WRITE
    if(at == HPL_WR) ((InternalMultiDimArray*)this)->onlyWrite((Platform_t)MAX_PLATFORMS, (Device_t)0, 0, NULL);


  }
  
/*
  void BaseArray::copy_in( bool copy, HPL::Device device, int start, int length ) {
  	  cl_device_type d_t_out;
  	  int d_d_out;
  	  TheGlobalState().clbinding().check_device(device.get_device_type(), device.get_device_number(),
  			  &d_t_out, &d_d_out);

  	  if( dbuffer_ == NULL ) dbuffer_ = TheGlobalState().clbinding().allocate_dbuffer(d_t_out,
  											  d_d_out, this);
  	  if( copy == true )
  	  {
  		  cl::CommandQueue queue_aux = TheGlobalState().clbinding().getCommandQueueObject(d_t_out,
  				  d_d_out);
  		  TheGlobalState().clbinding().copy_in(queue_aux,this, dbuffer_, start,length);
  	  }
    }

    void BaseArray::copy_out( bool copy, HPL::Device device, int start, int length) {
  	  if( copy == true )
  	  {
  		  cl_device_type d_t_out;
  		  int d_d_out;
  		  TheGlobalState().clbinding().check_device(device.get_device_type(), device.get_device_number(),
  				  &d_t_out, &d_d_out);

  		  cl::CommandQueue queue_aux = TheGlobalState().clbinding().getCommandQueueObject(d_t_out, d_d_out);
  		  TheGlobalState().clbinding().copy_out(queue_aux, this, dbuffer_,start,length);
  	  }
//  	  if( dbuffer_ != NULL ) {
//  	    delete dbuffer_;
//  	    dbuffer_ = NULL; // for the next round
//  	  }

    }*/

  
  CommonInternalIndexedArray::CommonInternalIndexedArray(const BaseArray* b, const BaseArray* aliasb)
  : bArr_(b), aliasbArr_(aliasb), nIndexes_(1), field_(-1)
  {}
  
  String_t CommonInternalIndexedArray::string(bool has_size, const String_t * const indexes_) const
  {  
    String_t tmp((aliasbArr_ ? aliasbArr_->name()  : bArr_->name()) + '[');
    
    for(int i = 0; i < nIndexes_ - 1; i++) {
      tmp += indexes_[i];
      
      if (HPL_STATE(NotBuilding) || has_size)
	tmp += "][";
      else
	tmp += " * " + (bArr_->name() + '_' + stringize(i)) + " + ";
      //tmp += indexes_[i] + " * " + ((HPL_STATE(NotBuilding) || bArr_->getSize()) ? stringize(cumulSize[i]) : (bArr_->name() + '_' + stringize(i))) + " + ";
    }
    
    tmp += indexes_[nIndexes_ - 1] + ']';
    
    if(!name_aux_.empty()) tmp += '.' + name_aux_;
    
    if(field_ != -1) tmp += VectorType::getFieldText(field_);
    
    return tmp;
  }
    
  String_t InternalIndexedArray<0>::string(bool has_size) const {
    
    String_t tmp(name_aux_);
    
    if(field_ != -1) tmp += VectorType::getFieldText(field_);
    
    return tmp;
  }
  
  InternalScalar::InternalScalar()
  : BaseArray(Local)
  { }
  
  InternalScalar::InternalScalar(const InternalScalar& another)
  : BaseArray(another)
  { }
  
  InternalScalar::InternalScalar(const InternalScalar& another, bool doactualcopy)
  : BaseArray(another, doactualcopy)
  { }
  
  /** It does not check HPL_STATE(NotBuilding) because it is only called from
     the children print, who already check it */
  void InternalScalar::print(const char* typestr, const String_t& value) const {
    String_t s(typestr);

    s += ' ' + name() + value;

    if (HPL_STATE(BuildingFHeader))
      TheGlobalState().getCodifier().add(s + ", ");
    else
      TheGlobalState().getCodifier().add(s, true);
  }

  InternalMultiDimArray::InternalMultiDimArray(ArrayFlags flags, void *p, unsigned int size)
  : BaseArray(flags), owned_(p <= HPL_FAST_MEM), fastMem_(p == HPL_FAST_MEM), size_(size),
    _tX(), _tY(), _tZ(), father(NULL), updated_(true), childrenDisabled_(false), childrenTotallyCover_(false)
  {
    for(int i1 = 0; i1 < (MAX_PLATFORMS + 1); i1++)
      for(int j = 0; j < MAX_DEVICE_TYPES; j++)
        for(int k = 0; k < HPL_MAX_DEVICE_COUNT; k++)
          buf_[i1][j][k] = None;
    buf_[MAX_PLATFORMS][0][0] = OwnBuffer;
  }
  
  /// @internal Notice it does not copy _tX, _tY, _tZ, father, and coherency variables
  /// @todo TODO: Remove it?
  InternalMultiDimArray::InternalMultiDimArray(const InternalMultiDimArray& another)
  : BaseArray(another), owned_(another.owned_), fastMem_(another.fastMem_), size_(another.size_)
  { }

//  InternalMultiDimArray::InternalMultiDimArray(InternalMultiDimArray* another)
//  : BaseArray(), owned_(false), size_(0)
//  {}

  String_t InternalMultiDimArray::string(int nd) const {
    String_t tmp = name();
    for (int i=0; i < (nd - 1); i++)
      tmp += ", int " + name() + '_' + stringize(i);
    
    return tmp;
  }


  void InternalMultiDimArray::print(const char* typestr, const int *dims, int nd) const {
    String_t s(storageName() + typestr + " ");

    if (HPL_STATE(BuildingFHeader)) {
       TheGlobalState().getCodifier().add(s + '*' + string(nd) + ", ");
    } else {
      s += name();
      for (int i=0; i < nd; i++) {
	s += '[' + stringize(dims[i]) + ']';
      }
      TheGlobalState().getCodifier().add(s, true);
    }
  }

}

