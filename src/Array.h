/*
 HPL : Heterogeneous Programming Library
 Copyright (c) 2012-2015 The HPL Team as listed in CREDITS.txt

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
/// \file     Array.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef ARRAY_H
#define ARRAY_H

#include <iostream>
#include <cassert>

#include "GlobalState.h"
#include "ExprManipulation.h"
#include "Device.h"
#include "HDM.h"
#include "Tuple.h"
#include <algorithm>

#define HPL_FAST_MEM   ((void *)0x32)
#define HPL_NOHOST_MEM ((void *)0x64)


/** @file Array.h
  * @brief Definition of all possible types of arguments of any kernel.
  * With the classes defined here, the programmer will define scalar arguments,
  * 1-D arguments, 2-D arguments, ...
  */

namespace HPL {

  /// Common ancestor to all the Arrays
  class BaseArray {
  public:
    /** @brief Creates a HPL::BaseArray that will be allocated in
     * \c Global memory by default.
     * @param[in] flags The space of memory where the array is allocated.
     */
    BaseArray(ArrayFlags flags = Global);
    
    /** @brief Creates a HPL::BaseArray from another already created.
     *  @param[in] another The array that will be cloned.
     */
    BaseArray(const BaseArray& another);
    
    /// Special constructor to avoid creation of new variables for scalars passed by value
    BaseArray(const BaseArray& another, bool doactualcopy);
    
    /** @brief Obtains the HPL::String_t that represents
     * this array in the kernel code.
     */
    const String_t &name() const { return name_; }
  
    /** @brief Obtains the HPL::ArrayFlags of this
     * array.
     */
    ArrayFlags flags() const { return flags_; }

    /** @brief Obtains the text representation of the
     *  type of storage of this array.
     *
     *  For example, if the array is allocated in
     *  global memory, this method will return "__global".
     */
    const String_t& storageName() const;

    /**
     * @brief Obtains the number of dimensions of the array.
     */
    virtual int getNumberOfDimensions() const = 0;
    
    /**
     * @brief Obtains the number of elements of this array for
     * the indicated dimension.
     * @param [in] d Analyzed dimension.
     */
    virtual int getDimension(int d) const = 0;
    
    /**
     * @brief Obtains a pointer to the data of this array.
     */
    virtual void *getData(AccessType at = HPL_RDWR) const = 0;
    
    /**
     * @brief Obtains the size of one data item of the array.
     */
    virtual std::size_t getDataItemSize() const = 0;

    cl::Buffer *dbuffer_;  ///< Device buffer at device 0 to hold data
    mutable HDM hdm;   ///< keeps dbuffer for each device.
    
  protected:
    
    void validate(AccessType at) const;
    
  private:
    
    const String_t name_; ///< Name of the Array
    
    const ArrayFlags flags_; ///< Kind of storage
    
  };
  
#ifndef HPL_NO_AUTOSYNC
  
  /// Proxy for the access to an element of an Array in host code.
  /** Its purpose is to allow to identify whether the array is read or written in the access */
  template<typename T>
  class Proxy {
    T& v_;                      ///< Managed value
    BaseArray * const bar_;     ///< Origin of the value
    
  public:
    
    Proxy(T& v, BaseArray *bar)
    : v_(v), bar_(bar)
    {}
    
    /// Write operation
    Proxy& operator=(const T& v) { bar_->hdm.enqueueMap(bar_); TheGlobalState().clbinding().readWrite((InternalMultiDimArray*)bar_,(Platform_t)MAX_PLATFORMS, (Device_t)0,0); v_ = v; return *this; }
    
    Proxy& operator=(const Proxy& v) { bar_->hdm.enqueueMap(bar_); TheGlobalState().clbinding().readWrite((InternalMultiDimArray*)bar_,(Platform_t)MAX_PLATFORMS, (Device_t)0,0); v_ = (T)v; return *this; }
  
    /// Read operation
    operator T () const { bar_->getData(HPL_RD);/*bar_->hdm.validate_basearray(bar_);*/ return v_; }
    
    template<class T2>
    friend T2& self_contents(Proxy<T2>& proxy);
    
    template<class T2>
    friend T2& self_contents(const Proxy<T2>& proxy);

    template<class T2, class T3>  
    friend Proxy<T3> proxy_child(Proxy<T2>& proxy, T3& v);
    
    template<class T2, class T3>  
    friend Proxy<T3> proxy_child(const Proxy<T2>& proxy, T3& v);
    
  };

  /// Specialization of Proxy for vector types that allows () indexing
  template<typename T, int N>
  class Proxy<InternalVector<T, N> > {
    InternalVector<T, N>& v_;  ///< Managed value
    BaseArray * const bar_;    ///< Origin of the value
  public:
    
    Proxy(InternalVector<T, N>& v, BaseArray *bar)
    : v_(v), bar_(bar)
    {}
    
    /// Write operation
    Proxy& operator=(const InternalVector<T, N>& v) { v_ = v; bar_->hdm.invalidate_dbuffers(); return *this; }
    
    Proxy& operator=(const Proxy& v) { v_ = (InternalVector<T, N>)v; bar_->hdm.invalidate_dbuffers(); return *this; }

    /// Read operation
    operator InternalVector<T, N> () const { bar_->getData(HPL_RD); return v_; }
    
    /// Vector type indexing
    Proxy<T> operator()(int i) {
      assert((i>=0) && (i<N));
      return Proxy<T>(v_(i), bar_);
    }
    
  };
  
  /// Allows to print a Proxy to an ostream
  template<typename T>
  std::ostream &operator<<(std::ostream &os, const Proxy<T> &a)
  {
    os << (T)a;
    return os;
  }
  
  /** @name Support functions for accessor to struct fields through macro ::field
   *  Thanks to these functions macro ::field can be applied both to Proxy and to standard structs
   */
  ///@{
  
  template<typename T>
  T& self_contents(T& v) { return v; }
  
  template<typename T>
  const T& self_contents(const T& v) { return v; }
  
  template<typename T>
  T& self_contents(Array<T, 0>& v) { return v.value(); }
  
  template<typename T>
  const T& self_contents(const Array<T, 0>& v) { return v.value(); }
  
  template<typename T>
  T& self_contents(Proxy<T>& proxy) { return proxy.v_; }
  
  template<typename T>
  T& self_contents(const Proxy<T>& proxy) { return proxy.v_; }
  
  template<typename T, typename T2>
  T2& proxy_child(T& v, T2& v2) { return v2; }
  
  template<typename T, typename T2>
  const T2& proxy_child(const T& v, const T2& v2) { return v2; }
  
  template<typename T, typename T2>
  Proxy<T2> proxy_child(Proxy<T>& proxy, T2& v2) { return Proxy<T2>(v2, proxy.bar_); }
  
  template<typename T, typename T2>
  Proxy<T2> proxy_child(const Proxy<T>& proxy, T2& v2) { return Proxy<T2>(v2, proxy.bar_); }
  
   ///@}
  
#endif
  
  
  template<typename T, int NDIM>
  class MultiDimArray;
  
  class CommonInternalIndexedArray {
    
  public:
    
    CommonInternalIndexedArray(const BaseArray* b, const BaseArray* aliasb);
    
    /// Returns a string representation of this object
    String_t string(bool has_size, const String_t * const indexes_) const;

    
  protected:
    const BaseArray * const bArr_;       ///< Points to the MultiDimArray being indexed
    const BaseArray * const aliasbArr_;  ///< Points to the alias through which the MultiDimArray is indexed
    int nIndexes_;                      ///< Number of indexes already stored in this object
    int field_;		                ///< The field accessed if the data type is a vector.
    String_t name_aux_;	                ///< Internal name of the the Array<T,0> OR field accessed
    
  };
  
  template<int NDIM>
  class InternalIndexedArray : protected CommonInternalIndexedArray {
    
  public:
    
    /// Constructor
    InternalIndexedArray(const BaseArray* b, const String_t& first_index, const BaseArray* aliasb)
    : CommonInternalIndexedArray(b, aliasb)
    {
      indexes_[0] = first_index;
    }

    /// Returns a string representation of this object.
    String_t string(bool has_size) const {

      assert(nIndexes_ == NDIM);
      
      return CommonInternalIndexedArray::string(has_size, indexes_);
    }
    
  protected:
    
    void addIndex(const String_t& newindex) {
      
      HPLExceptionIf(NDIM == nIndexes_, "Vector type indexing requires a constant");
      
      indexes_[nIndexes_++] = newindex;
    }
    
    void addIndex(int i, int NELEMSVECTOR) {
      
      if(NDIM == nIndexes_) {
	assert( (i>=0) && (i < NELEMSVECTOR) );
	HPLExceptionIf(field_ != -1, "Reindexing vector type");
	field_ = i;
      } else
	indexes_[nIndexes_++] = stringize(i);
      
    }
    
    void atfield(const char *field_name)
    {
      
      HPLExceptionIf(NDIM != nIndexes_, "Struct field access requested before full Array indexing completed");
      HPLExceptionIf(!name_aux_.empty(), "No nested struct access allowed!");

      name_aux_ = String_t(field_name);
    }
    
  private:
    
    String_t indexes_[NDIM];        ///< Array with the indexes stored in this object
  };
  
  template<>
  class InternalIndexedArray<0> {
    
  public:
    
    /// Constructor
    InternalIndexedArray(const String_t& name, int index)
    : bArr_(0), field_(index), name_aux_(name)
    { }
    
    /// Returns a string representation of this object.
    String_t string(bool has_size) const;
    
  protected:

    const BaseArray * const bArr_;      ///< Points to the MultiDimArray being indexed

  private:
    
    int field_;		            ///< The field accessed if the data type is a vector.
    String_t name_aux_;	           ///< Internal name of the the Array<T,0> OR field accessed
  };

  
  /// Array with one or more levels of indexing with operator[]
  /** @tparam T    type of the elements of the underlying Array
      @tparam NDIM number of dimensions of the underlying Array */
  template<typename T, int NDIM>
  class IndexedArray : protected InternalIndexedArray<NDIM> {
    
    using InternalIndexedArray<NDIM>::bArr_;
    
  public:
    
    /// Constructor
    IndexedArray(const MultiDimArray<T, NDIM>* b, const String_t& first_index, const BaseArray* aliasb = 0)
    : InternalIndexedArray<NDIM>(b, first_index, aliasb)
    { }

    // Constructor for accesses to elements of scalar vectors or fields of scalar structs
    IndexedArray(const String_t& name, int index)
    : InternalIndexedArray<0>(name, index)
    {
      static_assert(NDIM == 0, "constructor reserved to 0-DIM Arrays");
    }

    /**
     *  @brief Indexing with something that is not a PETE expression template in parallel code
     */
    template<class T2>
    IndexedArray<T, NDIM>& operator[](const T2& i)
    {
      InternalIndexedArray<NDIM>::addIndex(stringize(i));
      return *this;
    }

    /**
     * @brief Indexing with an integer constant in parallel code
     */
    IndexedArray<T, NDIM>& operator[](int i)
    {
      InternalIndexedArray<NDIM>::addIndex(i, TypeInfo<T>::Length);      
      return *this;
    }

    /// Indexing with an arbitrary PETE expression template in parallel code
    template<class T2>
    IndexedArray<T, NDIM>& operator[](const Expression<T2>& i)
    {
      InternalIndexedArray<NDIM>::addIndex(i.string());
      i.remove();
      return *this;
    }

    IndexedArray<T, NDIM>& atfield(const char *field_name)
    {
      InternalIndexedArray<NDIM>::atfield(field_name);
      
      DEBUGACTION(TheGlobalState().checkStructField(HPL::TypeInfo<T>::String, field_name));
      
      return *this;
    }
    
    const BaseArray *getBaseArray() const { return bArr_; }
    
    /// Returns a string representation of this object.
    String_t string() const {
      const MultiDimArray<T, NDIM> * const mdarr = getMultiDimArray();
      const bool has_size = (mdarr != 0) && (mdarr->getSize() != 0);
      return InternalIndexedArray<NDIM>::string(has_size); 
    }
    
    const MultiDimArray<T, NDIM> * getMultiDimArray() const { 
      return static_cast<const MultiDimArray<T, NDIM> *>(bArr_); 
    }
    
    /**
     * @name Automatically generated assignment operations
     */
    ///@{
#include "IndexedArray_T_NDIM_extraOps.h"
    ///@}

  };

  
  ///Simplifies accessing struct fields
#define at(afield) atfield(#afield)
  
  
   /// Dumps any IndexedArray to a std::ostream
  template<typename T, int NDIM>
  std::ostream &operator<<(std::ostream &os, const IndexedArray<T, NDIM> &a)
  {
    os << a.string();
    return os;
  }


  /// Class with common points for all the scalar Arrays
  class InternalScalar : public BaseArray {

  public:
    
    /// Default constructor
    InternalScalar();
    
    /// Copy constructor
    InternalScalar(const InternalScalar& another);
    
    /// Special constructor to avoid creation of new variables for scalars passed by value
    InternalScalar(const InternalScalar& another, bool doactualcopy);

    /// Returns a string representation of this object
    const String_t& string() const { return name(); }

    int getNumberOfDimensions() const final { return 0; }
    
    int getDimension(int d) const final { return 1; }
    
    int getXYZDim(int d) const { return 1; }

    DistribType getDistrib() { return D_NONE; }
    
    int getDistribSize() { return 0; }
    
    void killChildren() { }
  
  protected:
    /**
     * @brief This method make a part of the performance of printing
     * an array. The \c print() of the subclasses make the rest of
     * print performance making a call to this method.
     */
    void print(const char* typestr, const String_t& value) const;
  };
  
  /// InternalScalar specialization for scalars (0 dimensions)
  /** @tparam T data type of the scalar 
    *
    * @internal Notice that this template has nothing to do with the Array<T, NDIM, ArrayFlags>
    *           predefined in HPL_utils.h . They are completely different and separate. So for
    *           example it is impossible to define a storage Flag for an Array<T, 0>
    */
  template<typename T>
  class Array<T, 0> : public InternalScalar {
  public:
    
    /// Default constructor
    Array()
    : InternalScalar(), v_(T())
    { if(!HPL_STATE(NotBuilding)) InternalScalar::print(TypeInfo<T>::String, String_t()); }
    
    /**
     * @brief Constructor from a constant
     * @param[in] v Initialization constant.
     */
    Array(T v)
    : InternalScalar(), v_(v)
    { if(HPL_STATE(BuildingFBody)) print(stringize(v_)); }
    
    /**
     * @brief Copy constructor
     * @param[in] another Initialization array.
     * @internal in order to enable initializations like "Int x = y" in kernels, this method breaks the
     *           actual semantics of copy construction by generating a new scalar with InternalScalar()
     *           rather than copying the input one with InternalScalar(another)
     */
    Array(const Array<T, 0>& another)
    : InternalScalar(another, HPL_STATE(BuildingFBody) && TheGlobalState().getCodifier().pendingScalarsByValue()),
      v_(another.v_)
    { const String_t& n = another.name();
      if(HPL_STATE(BuildingFBody) && n.compare(name())) print(n); 
    }

    /**
     * @brief Move constructor. The variable assumes the name of the input one
     * @param[in] another Initialization array.
     */
    Array(Array<T, 0>&& another)
    : InternalScalar(another), v_(std::move(another.v_))
    { }

    /**
     * @brief initialization from an IndexedArray in a kernel
     * @param[in] another Initialization IndexedArray
     * Only valid inside kernels
     */
    template<int NDIM>
    Array(const IndexedArray<T, NDIM>&  another)
    : InternalScalar(), v_(T())
    { assert(HPL_STATE(BuildingFBody)); print(another.string()); }
    
    /**
     * @brief initialization from an arbitrary expression in a kernel
     * @param[in] another Initialization IndexedArray
     * Only valid inside kernels
     */
    template<class T2>
    Array(const Expression<T2>&  expr)
    : InternalScalar(), v_(T())
    { 
      assert(HPL_STATE(BuildingFBody)); 
      print(expr.string());
      expr.remove();
    }

    /// Accesses a field in a scalar struct
    IndexedArray<T, 0> atfield(const char *field_name)
    {      
      DEBUGACTION(TheGlobalState().checkStructField(HPL::TypeInfo<T>::String, field_name));

      return IndexedArray<T, 0>((name() + ".") + field_name, -1);
    }

    /**
     * @name Automatically generated assignment operations
     */
    ///@{
#include "Array_T_extraOps.h"
    ///@}
    
    void *getData(AccessType at = HPL_RDWR) const final { return (void *)(&v_); }

    std::size_t getDataItemSize() const final { return sizeof(T); }
    
    /// Get the value of this scalar as a modifiable value
    T &value() { return v_; }
    
    /// Get the value of this scalar as a constant
    T value() const { return v_; }

  protected:

    void print(const String_t& value) const {
      InternalScalar::print(TypeInfo<T>::String, "=" + value);
    }
    
    T v_;                  ///< Value of the scalar
    
  };

  
  /// InternalScalar specialization for scalars (0 dimensions) of classes InternalVector<T, N>
  /** @tparam TYPE data type of the InternalVector 
   *  @tparam N    length of the InternalVector
   *
   * @internal Notice that this template has nothing to do with the Array<T, NDIM, ArrayFlags>
   *           predefined in HPL_utils.h . They are completely different and separate. So for
   *           example it is impossible to define a storage Flag for an Array<T, 0>
   */
  template<typename TYPE, int N>
  class Array<InternalVector<TYPE, N>, 0> : public InternalScalar {
  public:
    
    /// This is so that we do not have to rewrite all the class, and more importantly
    /// the included Array_T_extraOps.h file!
    typedef InternalVector<TYPE, N> T; 
    
    /// Default constructor
    Array()
    : InternalScalar(), v_(T())
    { if(!HPL_STATE(NotBuilding)) InternalScalar::print(TypeInfo<T>::String, String_t()); }
    
    /**
     * @brief Constructor from a constant vector
     * @param[in] v Initialization constant.
     */
    Array(const T& v)
    : InternalScalar(), v_(v)
    { if(HPL_STATE(BuildingFBody)) print(stringize(v_)); }
    
    /**
     * @brief Constructor from a constant element of vector
     * @param[in] v Initialization constant.
     */
    Array(TYPE v)
    : InternalScalar(), v_(v)
    { if(HPL_STATE(BuildingFBody)) print_from_initializer(stringize(v)); }
    
    /**
     * @brief Copy constructor
     * @param[in] another Initialization array.
     * @internal in order to enable initializations like "Int x = y" in kernels, this method breaks the
     *           actual semantics of copy construction by generating a new scalar with InternalScalar()
     *           rather than copying the input one with InternalScalar(another)
     */
    Array(const Array<T, 0>& another)
    : InternalScalar(another, HPL_STATE(BuildingFBody) && TheGlobalState().getCodifier().pendingScalarsByValue()),
    v_(another.v_)
    { const String_t& n = another.name();
      if(HPL_STATE(BuildingFBody) && n.compare(name())) print(n); 
    }
    
    /**
     * @brief Move constructor. The variable assumes the name of the input one
     * @param[in] another Initialization array.
     */
    Array(Array<T, 0>&& another)
    : InternalScalar(another), v_(std::move(another.v_))
    { }

    /**
     * @brief initialization from an IndexedArray in a kernel
     * @param[in] another Initialization IndexedArray
     * Only valid inside kernels
     */
    template<int NDIM>
    Array(const IndexedArray<T, NDIM>&  another)
    : InternalScalar(), v_(T())
    { assert(HPL_STATE(BuildingFBody)); print(another.string()); }
    
    /**
     * @brief initialization from an arbitrary expression in a kernel
     * @param[in] expr an arbitrary expression
     * Only valid inside kernels
     */
    template<typename T1>
    Array(const Expression<T1>&  expr)
    : InternalScalar(), v_(T())
    { 
      assert(HPL_STATE(BuildingFBody)); 
      print(expr.string());
      expr.remove();
    }
    
    template<typename T1, typename T2>
    Array(const T1& v1, const T2& v2)
    : InternalScalar(), v_(T())
    { 
      static_assert(N == 2, "Constructor for vectors of length 2");
      assert(HPL_STATE(BuildingFBody)); 
      print_from_initializer(stringize(v1)+ ", " + stringize(v2));
      ArgBehavior<T1>::remove(v1);
      ArgBehavior<T2>::remove(v2);
    }
    
    template<typename T1, typename T2, typename T3, typename T4>
    Array(const T1& v1, const T2& v2, const T3& v3, const T4& v4)
    : InternalScalar(), v_(T())
    { 
      static_assert(N == 4, "Constructor for vectors of length 4");
      assert(HPL_STATE(BuildingFBody)); 
      print_from_initializer(stringize(v1)+ ", " + stringize(v2) + ", " +
			     stringize(v3)+ ", " + stringize(v4));
      ArgBehavior<T1>::remove(v1);
      ArgBehavior<T2>::remove(v2);
      ArgBehavior<T3>::remove(v3);
      ArgBehavior<T4>::remove(v4);
    }
    
    template<typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename T8>
    Array(const T1& v1, const T2& v2, const T3& v3, const T4& v4,
	  const T5& v5, const T6& v6, const T7& v7, const T8& v8)
    : InternalScalar(), v_(T())
    { 
      static_assert(N == 8, "Constructor for vectors of length 8");
      assert(HPL_STATE(BuildingFBody)); 
      print_from_initializer(stringize(v1)+ ", " + stringize(v2) + ", " +
			     stringize(v3)+ ", " + stringize(v4) + ", " +
			     stringize(v5)+ ", " + stringize(v6) + ", " +
			     stringize(v7)+ ", " + stringize(v8));
      ArgBehavior<T1>::remove(v1);
      ArgBehavior<T2>::remove(v2);
      ArgBehavior<T3>::remove(v3);
      ArgBehavior<T4>::remove(v4);
      ArgBehavior<T5>::remove(v5);
      ArgBehavior<T6>::remove(v6);
      ArgBehavior<T7>::remove(v7);
      ArgBehavior<T8>::remove(v8);
    }
    
    template<typename T1, typename T2, typename T3, typename T4,
    typename T5, typename T6, typename T7, typename T8,
    typename T9, typename T10, typename T11, typename T12,
    typename T13, typename T14, typename T15, typename T16>
    Array(const T1& v1, const T2& v2, const T3& v3, const T4& v4,
	  const T5& v5, const T6& v6, const T7& v7, const T8& v8,
	  const T9& v9, const T10& v10, const T11& v11, const T12& v12,
	  const T13& v13, const T14& v14, const T15& v15, const T16& v16)
    : InternalScalar(), v_(T())
    { 
      static_assert(N == 16, "Constructor for vectors of length 16");
      assert(HPL_STATE(BuildingFBody)); 
      print_from_initializer(stringize(v1)+ ", " + stringize(v2) + ", " +
			     stringize(v3)+ ", " + stringize(v4) + ", " +
			     stringize(v5)+ ", " + stringize(v6) + ", " +
			     stringize(v7)+ ", " + stringize(v8) + ", " +
			     stringize(v9)+ ", " + stringize(v10) + ", " +
			     stringize(v11)+ ", " + stringize(v12) + ", " +
			     stringize(v13)+ ", " + stringize(v14) + ", " +
			     stringize(v15)+ ", " + stringize(v16));
      ArgBehavior<T1>::remove(v1);
      ArgBehavior<T2>::remove(v2);
      ArgBehavior<T3>::remove(v3);
      ArgBehavior<T4>::remove(v4);
      ArgBehavior<T5>::remove(v5);
      ArgBehavior<T6>::remove(v6);
      ArgBehavior<T7>::remove(v7);
      ArgBehavior<T8>::remove(v8);
      ArgBehavior<T9>::remove(v9);
      ArgBehavior<T10>::remove(v10);
      ArgBehavior<T11>::remove(v11);
      ArgBehavior<T12>::remove(v12);
      ArgBehavior<T13>::remove(v13);
      ArgBehavior<T14>::remove(v14);
      ArgBehavior<T15>::remove(v15);
      ArgBehavior<T16>::remove(v16);
    }
    
    /**
     *  @brief Allows indexing of vector type
     */
    IndexedArray<T, 0> operator[](int i)
    {
      assert((i>=0) && (i < N));
      
      return IndexedArray<T, 0>(name(), i);
    }
    
    /**
     * @name Automatically generated assignment operations
     */
    ///@{
#include "Array_T_extraOps.h"
    ///@}
    
    void *getData(AccessType at = HPL_RDWR) const { return (void *)(&v_); }

    std::size_t getDataItemSize() const { return sizeof(T); }

    /// Get the value of this scalar as a modifiable value
    T &value() { return v_; }
    
    /// Get the value of this scalar as a constant
    T value() const { return v_; }

  protected:
    
    void print_from_initializer(const String_t& value) const {
      print('(' + String_t(TypeInfo<T>::String) + ")(" + value + ')');
    }
    
    void print(const String_t& value) const {
      InternalScalar::print(TypeInfo<T>::String, "=" + value);
    }
    
    T v_;    ///< Value of the scalar vector
    //const String_t custom_representation_; ///< For scalars initialized with several values
  };

  
  /** @name Scalar type synonyms.
   *  They improve the programmability when using scalars.
   */
  ///@{
  typedef Array<int, 0> Int;
  typedef Array<unsigned int, 0> UInt;
  typedef Array<std::size_t, 0> Size_t;
  typedef Array<float, 0> Float;
  typedef Array<double, 0> Double;
  typedef Array<float16, 0> Float16;
  typedef Array<float8, 0> Float8;
  typedef Array<float4, 0> Float4;
  typedef Array<float2, 0> Float2;
  typedef Array<int16, 0> Int16;
  typedef Array<int8, 0> Int8;
  typedef Array<int4, 0> Int4;
  typedef Array<int2, 0> Int2;
  ///@}
  
  
  /** @name Automatic names identified by the Codifier
   * They provide automatic thread identification without having to invoke explicitly any function
   */
  ///@{
  
  extern Size_t idx,  ///< global id for dimension 0
  idy,  ///< global id for dimension 1
  idz,  ///< global id for dimension 2
  lidx, ///< local id for dimension 0 
  lidy, ///< local id for dimension 1
  lidz, ///< local id for dimension 2
  gidx, ///< group id for dimension 0
  gidy, ///< group id for dimension 1
  gidz; ///< group id for dimension 2
  
  extern Size_t szx,  ///< size of global dimension 0
  szy,  ///< size of global dimension 1
  szz,  ///< size of global dimension 2
  lszx, ///< size of local dimension 0
  lszy, ///< size of local dimension 1
  lszz, ///< size of local dimension 2
  ngroupsx, ///< number of groups at dimension 0
  ngroupsy, ///< number of groups at dimension 1
  ngroupsz; ///< number of groups at dimension 2
  
  ///@} 

  
  
  /// Class with common points for all arrays
  class InternalMultiDimArray : public BaseArray {
  public:
    /**
	 * @brief Default constructor.
	 * @param[in] flags Memory space where this array is allocated in the device
	 * @param[in] p     Pointer for memory provided
	 * @param[in] size  Number of elements of the array.
	 */
    InternalMultiDimArray(ArrayFlags flags, void *p, unsigned int size);
    
    /// Copy constructor
    InternalMultiDimArray(const InternalMultiDimArray& another);

    /**
     * @brief Return the number of elements of the array
     */
    unsigned int getSize() const { return size_; }

    virtual int getXYZDim(int d) const = 0;
    //virtual bool isConsecutive() const = 0;
    
    ///////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////
    /**
     * @brief Obtains the definition tuple of the first dimension.
     */
    const Tuple& getTX() const { return _tX; };
    
    /**
     * @brief Obtains the definition tuple of the second dimension.
     */
     const Tuple& getTY() const { return _tY; };
    
    /**
     * @brief Obtains the definition tuple of the third dimension.
     */
     const Tuple& getTZ() const { return _tZ; };
    
    void setTX(Tuple tX) { _tX = tX; }
    void setTY(Tuple tY) { _tY = tY; }
    void setTZ(Tuple tZ) { _tZ = tZ; }
    
    /**
     * @brief Obtains the father Array of this Array.
     */
    InternalMultiDimArray* getFather() const { return this->father; }

    /**
     * @brief Checks if all the sons are updated in the host.
     */
    //    virtual bool areSyncFatherChildren() const = 0;
    
    /**
     * @brief Removes the buffer of a son for a device.
     */
    //    virtual void invalidSon( Platform_t platform_id, Device_t device_type, int device_n) const = 0;
    //    virtual void writeFather() const = 0;
    virtual void *getDataWithoutHDM() const = 0;
    virtual void onlyRead(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq) = 0;
    virtual void onlyWrite(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq) = 0;
    virtual void readWrite(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq) = 0;
    ///////////////////////////////CHILDREN_CODE_END/////////////////////////////////
    
  protected:
    
    /**
     *  @brief Return HPL::String_t representing this object as an array argument of \c nd dimensions
     *  @param[in] nd Number of dimensions of the array.
     */
    String_t string(int nd) const;
    
    void print(const char* typestr, const int *dims, int nd) const;
    
    const bool owned_;      ///< Whether the memory pointed to by v_ is owned by this object or not
    const bool fastMem_;    ///< Allocated in fast memory?
    unsigned int size_;     ///< Number of elements of the array
    
    ///////////////////////////////////////////////////////CHILDREN_CODE_BEGIN///////////////////////////////////////
    Tuple _tX; ///< Subregion selected in the less significant dimension (along rows)
    Tuple _tY; ///< Subregion selected in the next most significant dimension (along columns)
    Tuple _tZ; ///< Subregion selected in the last most significant dimension (along slices)
    InternalMultiDimArray * father; ///< If this is a subarray, array it comes from
    //////////////////////////////////////////////////////CHILDREN_CODE_END//////////////////////////////////////////
    
    //////////////////////////////////////////////////COHERENCY MEMORY BEGIN/////////////////////////////////////
    
    bool updated_;
    bool childrenDisabled_;
    bool childrenTotallyCover_;
    
    BufferType buf_[MAX_PLATFORMS+1][MAX_DEVICE_TYPES][HPL_MAX_DEVICE_COUNT];
    //////////////////////////////////////////////////COHERENCY MEMORY END/////////////////////////////////////

  };
  
  /// Underlying BaseArray specialization for arrays (>= 1 dimensions).
  /** This is the class that stores the data and does the actual work for non-scalars.
      @tparam T     type of the elements
      @tparam NDIM number of dimensions */
  template<typename T, int NDIM>
  class MultiDimArray: public InternalMultiDimArray {
    
  public:

    /// 1-D constructor
    MultiDimArray(int i, T* p, ArrayFlags flags) :
    InternalMultiDimArray(flags, static_cast<void *>(p), i),
    v_(!HPL_STATE(NotBuilding) ? NULL : ((owned_ && size_) ? static_cast<T*>(TheGlobalState().clbinding().allocMem(size_ * sizeof(T), fastMem_)) : p))
    {
      static_assert(NDIM == 1, "Constructor for 1D Arrays ");
      
      dims_[0] = i;
      distrib = D_NONE;
      distribSize = 0;
      cumulSize_[0] = 1;
      
      overlapping = (MultiDimArray<T,NDIM>**)malloc(2*sizeof(MultiDimArray<T,NDIM>*));
      overlapping[0] = NULL;
      overlapping[1] = NULL;

      buf_[MAX_PLATFORMS][0][0] = OwnBuffer;
      if(i >= 0) print(); //i should be -1 only when this is an AliasArray
    }

    /// 2-D constructor
    MultiDimArray(int i, int j, T* p, ArrayFlags flags) :
    InternalMultiDimArray(flags, static_cast<void *>(p), i*j), 
    v_(!HPL_STATE(NotBuilding) ? NULL : ((owned_ && size_) ? static_cast<T*>(TheGlobalState().clbinding().allocMem(size_ * sizeof(T), fastMem_)) : p))
    {
      static_assert(NDIM == 2, "Constructor for 2D Arrays ");

      dims_[0] = i;
      dims_[1] = j;
      distrib = D_NONE;
      distribSize = 0;
      cumulSize_[0] = j;
      cumulSize_[1] = 1;
      
//      overlapping_0 = NULL;
//      overlapping_1 = NULL;
      overlapping = (MultiDimArray<T,NDIM>**)malloc(2*sizeof(MultiDimArray<T,NDIM>*));
      overlapping[0] = NULL;
      overlapping[1] = NULL;

      print();
    }
    
    /// 3-D constructor
    MultiDimArray(int i, int j, int k, T* p, ArrayFlags flags) :
    InternalMultiDimArray(flags, static_cast<void *>(p), i*j*k),
    v_(!HPL_STATE(NotBuilding) ? NULL : ((owned_ &&size_) ? static_cast<T*>(TheGlobalState().clbinding().allocMem(size_ * sizeof(T), fastMem_)) : p))
    {
      static_assert(NDIM == 3, "Constructor for 3D Arrays ");
      
      dims_[0] = i;
      dims_[1] = j;
      dims_[2] = k;
      distrib = D_NONE;
      distribSize = 0;
      cumulSize_[0] = j*k;
      cumulSize_[1] = k;
      cumulSize_[2] = 1;

      overlapping = (MultiDimArray<T,NDIM>**)malloc(2*sizeof(MultiDimArray<T,NDIM>*));
      overlapping[0] = NULL;
      overlapping[1] = NULL;

      print();
    }
    
    /// Copy constructor
    MultiDimArray(const MultiDimArray<T, NDIM>& another)
    : InternalMultiDimArray(another), v_(another.v_)
    {
      //std::cerr << owned_ << " NDIM=" << NDIM << " size= " << size_ << " v=" << v_ << std::endl;
      
      assert(!owned_ || (v_==NULL)); //You should now make copies of Arrays with owned data
      
      for(int i=0; i < NDIM; i++) {
	dims_[i] = another.dims_[i];
	cumulSize_[i] = another.cumulSize_[i];
      }

      //These ones are silent; they are not printed to code
    }

    template<typename integer, class Tuple, class... Args>
    void check(integer i, Tuple t, Args... args)
    {
    	HPLExceptionIf(t.origin<0 || t.getSize()>this->getDimension(i), "Invalid subArray size");
    	if(i>0)
    		check(i-1,args...);
    }

    template<typename integer, class Tuple>
    void check(integer i, Tuple t)
    {
    	HPLExceptionIf(t.origin<0 || t.getSize()>this->getDimension(i), "Invalid subArray size");
    }

    /// Return String_t representing this object as an argument
    String_t string() const { return InternalMultiDimArray::string(NDIM); }

    /** @name Data access functions for parallel code. 
     *  They must be used to express accesses to the array in parallel code.
     *  They MUST NOT be used in host code.
     */
    ///@{
    
    /// Indexing with an Array scalar in parallel code
    template<typename SCALARTYPE>
    IndexedArray<T, NDIM> operator[](const Array<SCALARTYPE, 0>& i) const
    {
      assert(HPL_STATE(BuildingFBody));
      TheGlobalState().getArrayInfo(this)->addAccess(); //Increments in one the number of accesses to the array.
      return IndexedArray<T, NDIM>(this, i.name());
    }
    
    /// Indexing with an integer in parallel code
    IndexedArray<T, NDIM> operator[](int i) const
    {
      assert(HPL_STATE(BuildingFBody));
      TheGlobalState().getArrayInfo(this)->addAccess(); //Increments in one the number of accesses to the array.
      return IndexedArray<T, NDIM>(this, stringize(i));
    }
    
    /// Indexing with an arbitrary PETE expression template in parallel code
    template<class RHS>
    IndexedArray<T, NDIM> operator[](const Expression<RHS> &i) const
    {
      assert(HPL_STATE(BuildingFBody));
      String_t tmp = i.string();
      i.remove();
      TheGlobalState().getArrayInfo(this)->addAccess(); //Increments in one the number of accesses to the array.
      return IndexedArray<T, NDIM>(this, tmp);
    }
    ///@}
    
    int getNumberOfDimensions() const final { return NDIM; }

    int getDimension(int d) const final {
	  return (d < NDIM) ? dims_[d] : 1;
    }

    int getXYZDim(int d) const final {
    	return (d < NDIM) ? dims_[NDIM-1-d] : 1;
    }

    bool isConsecutive() const
    {
        if(NDIM == 1) return true;
        else
        {
                if(getFather()!=NULL)
                {
                       	if( NDIM==2 && getTX().end == getXYZDim(0) ) return true;
                        if( NDIM==3 && (getTX().end == getXYZDim(0) && (getTY().end == getXYZDim(1))) ) return true;
                       	return false;
               	}
                else return true;
        }
    }

    const int * getDimensions() const { return dims_; }

    /// This is another form of read
    /// If the array has sons, these will be updated to the host immediately after the father.
    void *getData(AccessType at = HPL_RDWR) const {
        validate(at);
//////////////////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////////
//       	if(this->getNumberOfChildren()>0){
//               	updateWithChildren(at);
//       	}
//////////////////////////////////////////CHILDREN_CODE_END///////////////////////////////////////////////
        return (void *)(v_); 
    }

    T *data(AccessType at = HPL_RDWR) const {
      return (T *)getData(at);
    }
    
    /// Obtain a pointer to host data without interacting with HDM.
    void *getDataWithoutHDM() const final {
    	return (void*)(v_);
    }

    std::size_t getDataItemSize() const final { return sizeof(T); }
    
    const int *getCumulSize() const {
    		return cumulSize_;
    }
    
    /// Return the reduction of all the values in the array using operation \c op
    template<typename OP>
    T reduce(OP op) {
    	const T * const p = (const T *)getData(HPL_RD);
    	const unsigned int sz = getSize();
    	T d = *p;
    	////////////////////////////////////////CHILDREN_CODE_BEGIN//////////////////////////////////////////////////
    	if((this->getFather()!=NULL) && (NDIM > 1))
    	{
    		d = 0;
    		if(NDIM==2)
    		{
    			for(unsigned int i = 0; i < this->getXYZDim(1); i++)
    				for(unsigned int j = 0; j < this->getXYZDim(0); j++)
    					d = op(d, p[i*this->getTX().end + j]);
    		}
    		if(NDIM==3)
    		{
    			for(unsigned int i = 0; i < this->getXYZDim(2); i++)
    				for(unsigned int j = 0; j < this->getXYZDim(1); j++)
    					for(unsigned int k = 0; k < this->getXYZDim(0); k++)
    						d = op(d, p[i * this->getTY().end * this->getTX().end + j*this->getTX().end + k]);
    		}
    	}
    	else
    		///////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////////////////////
    		{
    		for(unsigned int i = 1; i < sz; ++i)
    			d = op(d, p[i]);
    		}
    	return d;
    }

    void get_backend_subarrays(std::vector<Tuple>& sizes, int ndev, int size_stencil = 0, int dimension_order = 0)
    {
     float work;
     int accum_work, last, t_begin, t_end, current_work;
     accum_work = 0;
     int nn = this->getXYZDim(dimension_order);
     last = ndev-1;
     for(int i = 0; i < ndev; i++)
      {
        work = 1.0f / ndev;
        if(i == last)
          current_work = (nn) - accum_work;
        else
          current_work = (nn) * work;

        t_begin = (accum_work-((size_stencil-1)/2)*(i!=0));
        t_end = (i == (ndev-1) ? nn - 1 : accum_work + (current_work +((size_stencil-1)/2)-1));
        sizes.push_back(Tuple(t_begin,t_end));
        accum_work += current_work;

      }
    }

    void get_subarray_sizes(std::vector<Tuple>& sizes)
    {
    	typename std::map<KeyChildrenMap, Array<T,NDIM>* >::const_iterator it(children.begin());

    	int out_ori, out_end;
    	for(; it != children.end(); it++)
    	{
    		MultiDimArray<T,NDIM>* ar = (*it).second();
    		if(NDIM==1)
    		{
    			out_ori = (*it).first.tXO;
    			out_end = (*it).first.tXE;
    		}
    		else
    		{
    			if(NDIM == 2)
    			{
    				if(((*it).first.tXO != 0 || (*it).first.tXE != (this->getDimension(1)-1)))
    				{
    					out_ori = (*it).first.tXO;
    					out_end = (*it).first.tXE;
    				} else {
    					out_ori = (*it).first.tYO;
    					out_end = (*it).first.tYE;
    				}
    			}
    			else {
    				if(((*it).first.tXO != 0 || (*it).first.tXE != (this->getDimension(2)-1)))
    				{
    					out_ori = (*it).first.tXO;
    					out_end = (*it).first.tXE;
    				}
    				else
    				{
    					if(((*it).first.tYO != 0 || (*it).first.tYE != (this->getDimension(1)-1))) {
    						out_ori = (*it).first.tYO;
    						out_end = (*it).first.tYE;
    					}
    					else
    					{
    						out_ori = (*it).first.tZO;
    						out_end = (*it).first.tZE;
    					}
    				}
    			}
    		}
    		sizes.push_back(Tuple(out_ori, out_end));
    	}
    }

    MultiDimArray<T,NDIM>& operator=(const T& scalar)
    {
    	if(getFather()!=NULL)
    	{
    		int data_size = this->getSize();
                //T dataLinear[data_size];
                HPL_STACK_STORAGE(T, dataLinear, data_size);
          
    		std::fill(dataLinear, dataLinear+data_size, scalar);

    		if((this->hdm.get_device_platform()!=MAX_PLATFORMS) && this->hdm.get_deviceValid(this->hdm.get_device_platform(),
    				this->hdm.get_device_type(),this->hdm.get_device_number()))
    		{
    			T* data = (T*)this->getDataWithoutHDM();
    			std::fill(data, data+(this->getXYZDim(0)* this->getXYZDim(1)*
    					this->getXYZDim(2)), scalar);
    			this->hdm.invalidate_dbuffers();
    		}
    		else
    		{
    			if((this->getFather()->hdm.get_device_platform()!=MAX_PLATFORMS)  && this->getFather()->hdm.get_deviceValid(this->getFather()->hdm.get_device_platform(),
    					this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number()))
    			{
    				TheGlobalState().clbinding().copy_to_buffer_from_basearray_range(this->getFather()->hdm.get_device_platform(),
    						this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(), this,
    						this->_tX.origin, this->_tY.origin, this->_tZ.origin, this->getXYZDim(0), this->getXYZDim(1),
    						this->getXYZDim(2), this->getTX().end*sizeof(T),  this->getTX().end*this->getTY().end*sizeof(T),
    						this->getTX().end*sizeof(T),  this->getTX().end*this->getTY().end*sizeof(T),dataLinear);
    			}

    			if(this->hdm.get_hostValid()!=0)
    				setDataRect(dataLinear, this->getTY().end, this->getTX().end);
    		}
    	}
    	else
    	{
    		T* data = (T*)this->getDataWithoutHDM();
    		std::fill(data, data+(this->getSize()), scalar);
    		this->hdm.invalidate_dbuffers();
    	}

    	return *this;
    }

    MultiDimArray<T,NDIM>& operator=(const MultiDimArray<T,NDIM>& other)
    {
    	if(this->getFather()!=NULL && other.getFather()!=NULL)
    	{
    		//////TODO: THIS FIRST CLAUSE WOULD WORK IN ANY CASES, INSTEAD OF THE ELSE
    		////// THAT ONLY WOULD WORK IN CASE OF CONSECUTIVE MEMORY ADDRESSES.
    		if(NDIM==2)
    		{
    			if(other.hdm.get_hostValid()==0)
    				other.getData(HPL_RD);

    			if(other.hdm.get_device_platform()==MAX_PLATFORMS)
    			{
    				if((other.getFather()->hdm.get_device_platform()!=MAX_PLATFORMS) && other.getFather()->hdm.get_deviceValid(other.getFather()->hdm.get_device_platform(), other.getFather()->hdm.get_device_type(), other.getFather()->hdm.get_device_number()))
    				{

    					TheGlobalState().clbinding().copy_from_buffer_to_basearray_range(other.getFather()->hdm.get_device_platform(),
    							other.getFather()->hdm.get_device_type(),other.getFather()->hdm.get_device_number(), &other, other.getTX().origin, other.getTY().origin,
    							other.getTZ().origin, other.getXYZDim(0), other.getXYZDim(1), other.getXYZDim(2), other.getTX().end*sizeof(T), other.getTX().end * other.getTY().end*sizeof(T),
    							other.getFather()->getDataWithoutHDM());

    				}
    			}

                        //T dataLinear[other.getXYZDim(0)*other.getXYZDim(1)*other.getXYZDim(2)];
    			HPL_STACK_STORAGE(T, dataLinear, other.getXYZDim(0)*other.getXYZDim(1)*other.getXYZDim(2));

                        other.getDataLinear(dataLinear, other.getTY().end, other.getTX().end);
    			if((this->hdm.get_device_platform()!=MAX_PLATFORMS) && this->hdm.get_deviceValid(this->hdm.get_device_platform(),
    					this->hdm.get_device_type(),this->hdm.get_device_number()))
    			{
    				//IF NDIM > 1 getData returns a non consecutive memory space. This is an incorrect assignment.
    				memcpy(this->getData(HPL_RD), dataLinear, other.getXYZDim(0)*other.getXYZDim(1)*other.getXYZDim(2)*sizeof(T));
    				this->hdm.invalidate_dbuffers();
    			}
    			else
    			{
    				if((this->getFather()->hdm.get_device_platform()!=MAX_PLATFORMS) && this->getFather()->hdm.get_deviceValid(this->getFather()->hdm.get_device_platform(),
    						this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number()))
    					TheGlobalState().clbinding().copy_to_buffer_from_basearray_range(this->getFather()->hdm.get_device_platform(),
    							this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(), this,
    							this->getTX().origin, this->getTY().origin, this->getTZ().origin, this->getXYZDim(0), this->getXYZDim(1),
    							this->getXYZDim(2), this->getTX().end*sizeof(T), this->getTX().end*this->getTY().end*sizeof(T), this->getXYZDim(0)*sizeof(T),
    							this->getXYZDim(0) * this->getXYZDim(1) * sizeof(T), dataLinear);

    				if(this->hdm.get_hostValid()!=0)
    					setDataRect(dataLinear, this->getTY().end, this->getTX().end);
    			}
    		}
    		else
    		{
    			if(this->getFather()->hdm.get_device_type()==ACCELERATOR && other.getFather()->hdm.get_device_type()==ACCELERATOR)
    			{
    				//#ifdef ENQUEUE_COPY
    				TheGlobalState().clbinding().enqueue_copy(this->getFather()->hdm.get_device_platform(), this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(),
    						other.getFather()->hdm.get_device_platform(), other.getFather()->hdm.get_device_type(), other.getFather()->hdm.get_device_number(),
    						(MultiDimArray<T,NDIM>*)this->getFather(), (MultiDimArray<T,NDIM>*)other.getFather(),
    						this->getTZ().origin *this->getXYZDim(0)*this->getXYZDim(1) + this->getTY().origin*this->getXYZDim(0) + this->getTX().origin,
    						other.getTZ().origin *other.getXYZDim(0)*other.getXYZDim(1) + other.getTY().origin*other.getXYZDim(0) + other.getTX().origin,
    						other.getXYZDim(0)*other.getXYZDim(1)*other.getXYZDim(2)*sizeof(T));
    				//#else
    			}
    			else
    			{
    				if(this->getFather()->hdm.get_device_type() ==  other.getFather()->hdm.get_device_type() &&
    						this->getFather()->hdm.get_device_platform() ==  other.getFather()->hdm.get_device_platform() &&
    						this->getFather()->hdm.get_device_number() == other.getFather()->hdm.get_device_number())
    					TheGlobalState().clbinding().enqueue_copy(this->getFather()->hdm.get_device_platform(), this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(),
    							other.getFather()->hdm.get_device_platform(), other.getFather()->hdm.get_device_type(), other.getFather()->hdm.get_device_number(), this, &other, this->getTX().origin, other.getTX().origin,
    							other.getXYZDim(0)*sizeof(T));
    				else
    				{

    					if(other.hdm.get_hostValid()==0)
    						other.getData(HPL_RD);

    					if(other.hdm.get_device_platform()==MAX_PLATFORMS)
    					{
    						if((other.getFather()->hdm.get_device_platform()!=MAX_PLATFORMS) && other.getFather()->hdm.get_deviceValid(other.getFather()->hdm.get_device_platform(), other.getFather()->hdm.get_device_type(), other.getFather()->hdm.get_device_number()))
    						{

    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range(other.getFather()->hdm.get_device_platform(),
    									other.getFather()->hdm.get_device_type(),other.getFather()->hdm.get_device_number(), &other, other.getTX().origin, other.getTY().origin,
    									other.getTZ().origin, other.getXYZDim(0), other.getXYZDim(1), other.getXYZDim(2), other.getTX().end*sizeof(T), other.getTX().end * other.getTY().end*sizeof(T),
    									other.getFather()->getDataWithoutHDM());

    						}
    					}
    					if((this->hdm.get_device_platform()!=MAX_PLATFORMS) && this->hdm.get_deviceValid(this->hdm.get_device_platform(),
    							this->hdm.get_device_type(),this->hdm.get_device_number()))
    					{
    						//IF NDIM > 1 getData returns a non consecutive memory space. This is an incorrect assignment.
    						memcpy(this->getData(HPL_RD), other.getDataWithoutHDM(), other.getXYZDim(0)*other.getXYZDim(1)*other.getXYZDim(2)*sizeof(T));
    						this->hdm.invalidate_dbuffers();
    					}
    					else
    					{
    						if((this->getFather()->hdm.get_device_platform()!=MAX_PLATFORMS) && this->getFather()->hdm.get_deviceValid(this->getFather()->hdm.get_device_platform(),
    								this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number()))
    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range(this->getFather()->hdm.get_device_platform(),
    									this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(), this,
    									this->getTX().origin, this->getTY().origin, this->getTZ().origin, this->getXYZDim(0), this->getXYZDim(1),
    									this->getXYZDim(2), this->getTX().end*sizeof(T), this->getTX().end*this->getTY().end*sizeof(T), this->getXYZDim(0)*sizeof(T),
    									this->getXYZDim(0) * this->getXYZDim(1) * sizeof(T), ((T*)other.getDataWithoutHDM()));


    					}
    				}
    			}
    			//#endif
    		}
    	}
    	if(getFather()==NULL && other.getFather()==NULL)
    	{
    		memcpy(this->getData(HPL_RD), other.getData(HPL_RD), other.getSize()*sizeof(T));
    		this->hdm.invalidate_dbuffers();
    	}
    	if(getFather()==NULL && other.getFather()!=NULL)
    	{

    		//Tuple tX = other.getTX();
    		//Tuple tY = other.getTY();
    		//Tuple tZ = other.getTZ();
    		//HPLExceptionIf(other.getDimension(1)!=1 || other.getDimension(2)!=1, "You cannot do this assignment!");
    		this->validate(HPL_RD);

    		if(other.hdm.get_hostValid()==0)
    			other.getData(HPL_RD);
    		if(other.hdm.get_device_platform()==MAX_PLATFORMS)
    		{
    			if((other.getFather()->hdm.get_device_platform()!=MAX_PLATFORMS) && other.getFather()->hdm.get_deviceValid(other.getFather()->hdm.get_device_platform(), other.getFather()->hdm.get_device_type(), other.getFather()->hdm.get_device_number()))
    			{

    				TheGlobalState().clbinding().copy_from_buffer_to_basearray_range(other.getFather()->hdm.get_device_platform(),
    						other.getFather()->hdm.get_device_type(),other.getFather()->hdm.get_device_number(), &other, other.getTX().origin, other.getTY().origin,
    						other.getTZ().origin, other.getXYZDim(0), other.getXYZDim(1), other.getXYZDim(2), other.getTX().end*sizeof(T), other.getTX().end * other.getTY().end*sizeof(T),
    						other.getFather()->getDataWithoutHDM());
    			}
    		}

    		//T dataLinear[other.getXYZDim(0) * other.getXYZDim(1) * other.getXYZDim(2)];
                HPL_STACK_STORAGE(T, dataLinear, other.getXYZDim(0) * other.getXYZDim(1) * other.getXYZDim(2));
          
    		other.getDataLinear(dataLinear, 0,0);
    		setDataRect(dataLinear,this->getXYZDim(1), this->getXYZDim(0));

    		this->hdm.invalidate_dbuffers();

    	}
    	if(getFather()!=NULL && other.getFather()==NULL)
    	{
    		if((this->hdm.get_device_platform()!=MAX_PLATFORMS) && this->hdm.get_deviceValid(this->hdm.get_device_platform(),
    				this->hdm.get_device_type(),this->hdm.get_device_number()))
    		{
    			memcpy(this->getData(HPL_RD), (T*)other.getData(HPL_RD), other.getXYZDim(0)*other.getXYZDim(1)*other.getXYZDim(2)*sizeof(T));
    			this->hdm.invalidate_dbuffers();
    		}
    		else
    		{
    			if((this->getFather()->hdm.get_device_platform()!=MAX_PLATFORMS) && this->getFather()->hdm.get_deviceValid(this->getFather()->hdm.get_device_platform(),
    					this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number()))
    				TheGlobalState().clbinding().copy_to_buffer_from_basearray_range(this->getFather()->hdm.get_device_platform(),
    						this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(), this,
    						this->getTX().origin, this->getTY().origin, this->getTZ().origin, this->getXYZDim(0), this->getXYZDim(1),
    						this->getXYZDim(2), this->getTX().end*sizeof(T), this->getTX().end*this->getTY().end*sizeof(T),
						other.getXYZDim(0) * sizeof(T), other.getXYZDim(1) * other.getXYZDim(0) * sizeof(T), (T*)other.getData(HPL_RD));
    			if(this->hdm.get_hostValid()!=0)
    				setDataRect((T*)other.getData(HPL_RD), this->getTY().end, this->getTX().end);
    		}

    	}

    	return *this;
    }

    template<int NDIM2>
    MultiDimArray<T, NDIM>& operator=(const MultiDimArray<T, NDIM2>& other) {
    	if (this->getFather() != NULL && other.getFather() != NULL) {
    		if (other.hdm.get_hostValid() == 0)
    			other.getData(HPL_RD);
    		if (other.hdm.get_device_platform() == MAX_PLATFORMS) {
    			if ((other.getFather()->hdm.get_device_platform()
    					!= MAX_PLATFORMS)
    					&& other.getFather()->hdm.get_deviceValid(
    							other.getFather()->hdm.get_device_platform(),
    							other.getFather()->hdm.get_device_type(),
    							other.getFather()->hdm.get_device_number())) {

    				TheGlobalState().clbinding().copy_from_buffer_to_basearray_range(other.getFather()->hdm.get_device_platform(),
    						other.getFather()->hdm.get_device_type(),other.getFather()->hdm.get_device_number(), &other, other.getTX().origin, other.getTY().origin,
    						other.getTZ().origin, other.getXYZDim(0), other.getXYZDim(1), other.getXYZDim(2), other.getTX().end*sizeof(T), other.getTX().end * other.getTY().end*sizeof(T),
    						other.getFather()->getDataWithoutHDM());

    			}
    		}
    		//T dataLinear[other.getXYZDim(0) * other.getXYZDim(1) * other.getXYZDim(2)];
                HPL_STACK_STORAGE(T, dataLinear, other.getXYZDim(0) * other.getXYZDim(1) * other.getXYZDim(2));
          
    		other.getDataLinear(dataLinear, other.getTY().end, other.getTX().end);
    		if ((this->hdm.get_device_platform() != MAX_PLATFORMS)
    				&& this->hdm.get_deviceValid(
    						this->hdm.get_device_platform(),
    						this->hdm.get_device_type(),
    						this->hdm.get_device_number())) {
    			memcpy(
    					this->getData(HPL_RD),
    					dataLinear,
    					other.getXYZDim(0) * other.getXYZDim(1)
    					* other.getXYZDim(2) * sizeof(T));
    			this->hdm.invalidate_dbuffers();
    		} else {
    			if ((this->getFather()->hdm.get_device_platform()
    					!= MAX_PLATFORMS)
    					&& this->getFather()->hdm.get_deviceValid(
    							this->getFather()->hdm.get_device_platform(),
    							this->getFather()->hdm.get_device_type(),
    							this->getFather()->hdm.get_device_number()))
    				TheGlobalState().clbinding().copy_to_buffer_from_basearray_range(this->getFather()->hdm.get_device_platform(),
    						this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(), this,
    						this->getTX().origin, this->getTY().origin, this->getTZ().origin, this->getXYZDim(0), this->getXYZDim(1),
    						this->getXYZDim(2), this->getTX().end*sizeof(T), this->getTX().end*this->getTY().end*sizeof(T), this->getXYZDim(0)*sizeof(T),
                                                this->getXYZDim(0) * this->getXYZDim(1) * sizeof(T), dataLinear);

    			if (this->hdm.get_hostValid() != 0)
    				setDataRect(dataLinear, this->getTY().end,
    						this->getTX().end);
    		}

    	}
    	if (getFather() == NULL && other.getFather() != NULL) {

    		//Tuple tX = other.getTX();
    		//Tuple tY = other.getTY();
    		//Tuple tZ = other.getTZ();
    		//HPLExceptionIf(other.getDimension(1)!=1 || other.getDimension(2)!=1,"You cannot do this assignment!");
    		this->validate(HPL_RD);

    		if (other.hdm.get_hostValid() == 0)
    			other.getData(HPL_RD);
    		if (/*other.hdm.get_device_number()==-1*/other.hdm.get_device_platform()
    				== MAX_PLATFORMS) {
    			if ((other.getFather()->hdm.get_device_platform()
    					!= MAX_PLATFORMS)
    					&& other.getFather()->hdm.get_deviceValid(
    							other.getFather()->hdm.get_device_platform(),
    							other.getFather()->hdm.get_device_type(),
    							other.getFather()->hdm.get_device_number())) {

    				TheGlobalState().clbinding().copy_from_buffer_to_basearray_range(other.getFather()->hdm.get_device_platform(),
    						other.getFather()->hdm.get_device_type(),other.getFather()->hdm.get_device_number(), &other, other.getTX().origin, other.getTY().origin,
    						other.getTZ().origin, other.getXYZDim(0), other.getXYZDim(1), other.getXYZDim(2), other.getTX().end*sizeof(T), other.getTX().end * other.getTY().end*sizeof(T),
    						other.getFather()->getDataWithoutHDM());
    			}
    		}

    		//T dataLinear[other.getXYZDim(0) * other.getXYZDim(1) * other.getXYZDim(2)];
                HPL_STACK_STORAGE(T, dataLinear, other.getXYZDim(0) * other.getXYZDim(1) * other.getXYZDim(2));
          
    		other.getDataLinear(dataLinear, 0, 0);
    		setDataRect(dataLinear, /*other.getTY().end, other.getTX().end*/this->getXYZDim(1), this->getXYZDim(0));

    		this->hdm.invalidate_dbuffers();

    	}
    	if (getFather() != NULL && other.getFather() == NULL) {
    		if ((this->hdm.get_device_platform() != MAX_PLATFORMS)
    				&& this->hdm.get_deviceValid(
    						this->hdm.get_device_platform(),
    						this->hdm.get_device_type(),
    						this->hdm.get_device_number())) {
    			memcpy(
    					this->getData(HPL_RD),
    					(T*) other.getData(HPL_RD),
    					other.getXYZDim(0) * other.getXYZDim(1)
    					* other.getXYZDim(2) * sizeof(T));
    			this->hdm.invalidate_dbuffers();
    		} else {
    			if ((this->getFather()->hdm.get_device_platform()
    					!= MAX_PLATFORMS)
    					&& this->getFather()->hdm.get_deviceValid(
    							this->getFather()->hdm.get_device_platform(),
    							this->getFather()->hdm.get_device_type(),
    							this->getFather()->hdm.get_device_number()))
    				TheGlobalState().clbinding().copy_to_buffer_from_basearray_range(this->getFather()->hdm.get_device_platform(),
    						this->getFather()->hdm.get_device_type(),this->getFather()->hdm.get_device_number(), this,
    						this->getTX().origin, this->getTY().origin, this->getTZ().origin, this->getXYZDim(0), this->getXYZDim(1),
    						this->getXYZDim(2), this->getTX().end*sizeof(T), this->getTX().end*this->getTY().end*sizeof(T),
						0,0, (T*)other.getData(HPL_RD));

    			if (this->hdm.get_hostValid() != 0)
    				setDataRect((T*) other.getData(HPL_RD), this->getTY().end,
    						this->getTX().end);
    		}

    	}

    	return *this;

    }

    /*
     * Return an array (dataLinear) with the data of the rectangle pointed by data.
     */
    void getDataLinear(T* dataLinear, int dim2, int dim1) const
    {
    	T* dataRect = (T*)this->getDataWithoutHDM();
    	for(int k = 0; k < getXYZDim(2); k++)
    	{
    		for(int j = 0; j < getXYZDim(1); j++)
    		{
    			for(int i = 0; i < getXYZDim(0); i++)
    			{
    				dataLinear[k*getXYZDim(1)*getXYZDim(0) + j*getXYZDim(0) + i] = dataRect[(k/*+_tZ.origin*/)*dim2*dim1 + (j/*+_tY.origin*/)*dim1 + (i/*+_tX.origin*/)];
    			}
    		}
    	}
    }

    /*
     * Return the data overlapped in this array taking into account the regions determined by the arguments
     */
    void getDataLinearOverlapp(T* dataLinear, int s_dim2, int s_dim1, int t_dim2, int t_dim1, int offset, int num_slices = 0) const
    {
    	T* dataRect = (T*)this->getDataWithoutHDM() + offset;
        for(int k = 0; k < num_slices; k++)
                for(int j = 0; j < t_dim2; j++)
                {
                        for(int i = 0; i < t_dim1; i++)
                        {
                                dataLinear[k*t_dim2*t_dim1 + j*t_dim1 + i] = dataRect[k* s_dim2 * s_dim1 + (j)*s_dim1 + (i)];
                        }
                }
    }

    /*
     * Store in the delegated array pointed by dataRect the linear data stored in dataLinear
     */
    void setDataRect(T* dataLinear, int dim2, int dim1)
    {
    	T* dataRect = (T*)this->getDataWithoutHDM();
    	for(int k = 0; k < getXYZDim(2); k++)
    	{
    		for(int j = 0; j < getXYZDim(1); j++)
    		{
    			for(int i = 0; i < getXYZDim(0); i++)
    			{
    				dataRect[(k/*+_tZ.origin*/)*dim2*dim1 + (j/*+_tY.origin*/)*dim1 + (i/*+_tX.origin*/)] =
    						dataLinear[k*getXYZDim(1)*getXYZDim(0) + j*getXYZDim(0) + i];
    			}
    		}
    	}
    }

    /** @name Data access functions for host code. 
     *   They allow to access the contents of the array in host code using the operator() 
     *  with the number of ints required for the indexing.
     *   They MUST NOT be used in parallel functions, as they do not send strings to the Codifier.
     */
    ///@{
    
#ifndef HPL_NO_AUTOSYNC

    /// Access element of 1D Array in host code tracking whether the Array is modified 
    Proxy<T> operator()(int i) { 
    		static_assert(NDIM == 1, "Indexing does not agree with the number of dimensions of the Array");
    		return Proxy<T>(v_[i], this);
    }
    
    /// Access element of 2D Array in host code tracking whether the Array is modified
    Proxy<T> operator()(int i, int j) {
    		static_assert(NDIM == 2, "Indexing does not agree with the number of dimensions of the Array");
    	    if(( this->getTX().end != 0) || (this->getTY().end != 0))
    	      return Proxy<T>(v_[i * this->getTX().end +j], this);
    	    else
    	      return Proxy<T>(v_[i * cumulSize_[0] + j], this);
    }
    
    /// Access element of 3D Array in host code tracking whether the Array is modified
    Proxy<T> operator()(int i, int j, int k) {
        if(( this->getTX().end != 0) || (this->getTY().end != 0) || (this->getTZ().end != 0))
          return Proxy<T>(v_[i * this->getTX().end * this->getTY().end +j * this->getTY().end + k], this);
        else
        	return Proxy<T>(v_[i * cumulSize_[0] + j * cumulSize_[1] + k], this);
    }
    
    /// Access element of const 1D Array in host code marking the Array as read
    const T& operator()(int i) const { 
    		static_assert(NDIM == 1, "Indexing does not agree with the number of dimensions of the Array");
    		validate(HPL_RD);
    		return v_[i];
    }
    
    /// Access element of const 2D Array in host code marking the Array as read
    const T& operator()(int i, int j) const { 
    		static_assert(NDIM == 2, "Indexing does not agree with the number of dimensions of the Array");
    		validate(HPL_RD);
    	      if(( this->getTX().end != 0) || (this->getTY().end != 0))
    	        return v_[i * this->getTX().end +j];
    	      else
    		return v_[i * cumulSize_[0] + j];
    }
    
    /// Access element of const 3D Array in host code marking the Array as read
    const T& operator()(int i, int j, int k) const {
    		static_assert(NDIM == 3, "Indexing does not agree with the number of dimensions of the Array");
    		validate(HPL_RD);
    	      if(( this->getTX().end != 0) || (this->getTY().end != 0) || (this->getTZ().end != 0))
    	        return v_[i * this->getTX().end * this->getTY().end +j * this->getTY().end + k];
    	      else
    		return v_[i * cumulSize_[0] + j * cumulSize_[1] + k];
    }

/** @brief Accessor for structs.
    @todo optimize with auto/decltype to get copy of px instead of building it twice */ 
#define field(px, fld) proxy_child((px), self_contents(px).fld)

#else
    
    /// Access element of 1D Array in host code without any status tracking
    T& operator()(int i) { 
      static_assert(NDIM == 1, "Indexing does not agree with the number of dimensions of the Array"); 
      return v_[i];
    }
    
    /// Access element of 2D Array in host code without any status tracking
    T& operator()(int i, int j)  { 
      static_assert(NDIM == 2, "Indexing does not agree with the number of dimensions of the Array"); 
      //////////////////////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////
      if(( this->getTX().end != 0) || (this->getTY().end != 0))
    	  return v_[i * this->getTX().end +j];
      else
      //////////////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////
    	  return v_[i * cumulSize_[0] + j];
    }
    
    /// Access element of 3D Array in host code without any status tracking
    T& operator()(int i, int j, int k)  { 
      static_assert(NDIM == 3, "Indexing does not agree with the number of dimensions of the Array"); 
      //////////////////////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////
      if(( this->getTX().end != 0) || (this->getTY().end != 0) || (this->getTZ().end != 0))
    	  return v_[i * this->getTX().end * this->getTY().end +j * this->getTX().end + k];
      else
      //////////////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////
    	  return v_[i * cumulSize_[0] + j * cumulSize_[1] + k];
    }
    
    /// Access element of const 1D Array in host code without any status tracking
    const T& operator()(int i) const { 
      static_assert(NDIM == 1, "Indexing does not agree with the number of dimensions of the Array"); 
      return v_[i]; 
    }
    
    /// Access element of const 2D Array in host code without any status tracking
    const T& operator()(int i, int j) const { 
      static_assert(NDIM == 2, "Indexing does not agree with the number of dimensions of the Array"); 
      //////////////////////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////
      if(( this->getTX().end != 0) || (this->getTY().end != 0))
    	  return v_[i * this->getTX().end +j];
      else
      //////////////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////
    	  return v_[i * cumulSize_[0] + j];
    }
    
    /// Access element of const 3D Array in host code without any status tracking
    const T& operator()(int i, int j, int k) const { 
      static_assert(NDIM == 3, "Indexing does not agree with the number of dimensions of the Array"); 
      //////////////////////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////
      if(( this->getTX().end != 0) || (this->getTY().end != 0) || (this->getTZ().end != 0))
    	  return v_[i * this->getTX().end +j * this->getTY().end + k];
      else
      //////////////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////
    	  return v_[i * cumulSize_[0] + j * cumulSize_[1] + k];
    }

#define field(px, fld) (px).fld

#endif
    
    ///@}

    /// Join two overlapped arrays
    void addOverlapp(MultiDimArray<T,NDIM>* bar)
    {
    	if(this->overlapping[0]!=NULL)
    	{
//		HPLExceptionIf(this->overlapping[1] != NULL, "Overlapping is not possible!");
    		this->overlapping[1] = bar;
    	}
    	else
    		this->overlapping[0] = bar;
    }

    //////////////////////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////
    /// The child array is stored in the Father.
    void registerChild(Array<T,NDIM>* child, Tuple tX, Tuple tY, Tuple tZ)
    {
    	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::iterator  it(children.begin());
    	for(; it != children.end(); it++)
    	{
    		Array<T,NDIM>* ar = (*it).second;
    		if((ar->getTX().origin + ar->getXYZDim(0) <= tX.origin)) continue;
    		if((ar->getTY().origin + ar->getXYZDim(1) <= tY.origin)) continue;
    		if((ar->getTZ().origin + ar->getXYZDim(2) <= tZ.origin)) continue;
    		//////The tuples have the end of the child dimension
    		if((ar->getTX().origin >= tX.origin + child->getXYZDim(0))) continue;
    		if((ar->getTY().origin >= tY.origin + child->getXYZDim(1))) continue;
    		if((ar->getTZ().origin >= tZ.origin + child->getXYZDim(2))) continue;

    		child->addOverlapp(ar);
    		ar->addOverlapp(child);
    	}
    	children.insert(std::pair<KeyChildrenMap,Array<T,NDIM>*>(KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end), child));
    }

    /**
     * @brief Obtains the number of sons of this Array.
     */
    size_t getNumberOfChildren() const
    {
    	return children.size();
    }

//    bool areSyncFatherChildren() const
//    {
//    	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
//    	for(; it != children.end(); it++)
//    	{
//    		Array<T,NDIM>* ar = (*it).second;
//    		if(!ar->hdm.get_hostValid()) return false;
//    	}
//    	return true;
//    }
//
//    /// The children are updated in the host side.
//    void updateWithChildren(AccessType at) const
//    {
//    	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
//    	for(; it != children.end(); it++)
//    	{
//    		Array<T,NDIM>* ar = (*it).second;
//    		ar->getData(at);
//    	}
//    }



//    void syncGhost() const
//    {
//    	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
//    	int value[3];
//    	for(; it != children.end(); it++)
//    	{
//    		if(((*it).second)->overlapping_0!=NULL)
//    		{
//    			(*it).second->getOverlappedArea((*it).second->overlapping_0, value);
//    			if((*it).second->getTZ().origin==(*it).second->overlapping_0->getTZ().origin)
//    			{
//    				if((*it).second->getTY().origin==(*it).second->overlapping_0->getTY().origin)
//    				{
//    					if((*it).second->getTX().origin<(*it).second->overlapping_0->getTX().origin)
//    					{
//    						if((*it).second->overlapping_0->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_0->hdm.get_device_platform(),
//    									(*it).second->overlapping_0->hdm.get_device_type(),(*it).second->overlapping_0->hdm.get_device_number(), (*it).second->overlapping_0, value[0], 0,
//    									0, value[0], value[1], value[2], (*it).second->overlapping_0->getXYZDim(0) *sizeof(T),
//    									(*it).second->overlapping_0->getXYZDim(0) * (*it).second->overlapping_0->getTY().end*sizeof(T),
//    									(*it).second->overlapping_0->getTX().end*sizeof(T), (*it).second->overlapping_0->getTX().end*(*it).second->overlapping_0->getTY().end*sizeof(T),
//    									(*it).second->overlapping_0->getDataWithoutHDM());
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//
//    						(*it).second->overlapping_0->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_0->getXYZDim(0), value[1], value[0], value[0], value[2]);
//
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    									(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    									(*it).second->overlapping_0->getTX().origin+value[0], (*it).second->overlapping_0->getTY().origin, (*it).second->overlapping_0->getTZ().origin,
//    									value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T),  (*it).second->getTX().end*(*it).second->getTY().end *sizeof(T), dataLinear);
//    						}
//
//    					}
//    					else
//    					{
//    						if((*it).second->overlapping_0->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_0->hdm.get_device_platform(),
//    									(*it).second->overlapping_0->hdm.get_device_type(),(*it).second->overlapping_0->hdm.get_device_number(), (*it).second->overlapping_0,
//    									(*it).second->overlapping_0->getXYZDim(0) - 2*value[0], 0, 0, value[0], value[1], value[2], (*it).second->overlapping_0->getXYZDim(0)*sizeof(T),
//    									(*it).second->overlapping_0->getXYZDim(0)* (*it).second->overlapping_0->getXYZDim(1)*sizeof(T),
//    									(*it).second->overlapping_0->getTX().end*sizeof(T), (*it).second->overlapping_0->getTX().end*(*it).second->overlapping_0->getTY().end*sizeof(T),
//    									((T*)(*it).second->overlapping_0->getDataWithoutHDM()));
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//    						(*it).second->overlapping_0->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_0->getXYZDim(0), value[1], value[0],
//    								(*it).second->overlapping_0->getXYZDim(0) - 2*value[0], value[2]);
//
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    									(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    									0, 0, 0,
//    									value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//    						}
//
//    					}
//    				}
//    				else
//    				{
//    					if((*it).second->getTY().origin<(*it).second->overlapping_0->getTY().origin)
//    					{
//    						if((*it).second->overlapping_0->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_0->hdm.get_device_platform(),
//    									(*it).second->overlapping_0->hdm.get_device_type(),(*it).second->overlapping_0->hdm.get_device_number(), (*it).second->overlapping_0, 0, value[1],
//    									0, value[0], value[1], value[2], (*it).second->overlapping_0->getXYZDim(0) *sizeof(T),
//    									(*it).second->overlapping_0->getXYZDim(0) * (*it).second->overlapping_0->getTY().end*sizeof(T),
//    									(*it).second->getTX().end*sizeof(T), (*it).second->getTX().end* (*it).second->overlapping_0->getTY().end *sizeof(T),
//    									(*it).second->overlapping_0->getDataWithoutHDM());
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//    						(*it).second->overlapping_0->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_0->getXYZDim(0), value[1], value[0], value[0]*value[1], value[2]);
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    									(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    									(*it).second->overlapping_0->getTX().origin, (*it).second->overlapping_0->getTY().origin+value[1], (*it).second->overlapping_0->getTZ().origin,
//    									value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//    						}
//
//    					}
//    					else
//    					{
//    						if((*it).second->overlapping_0->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_0->hdm.get_device_platform(),
//    									(*it).second->overlapping_0->hdm.get_device_type(),(*it).second->overlapping_0->hdm.get_device_number(), (*it).second->overlapping_0, 0,
//    									(*it).second->overlapping_0->getXYZDim(1) - 2 * value[1], 0, value[0], value[1], value[2], (*it).second->overlapping_0->getXYZDim(0)*sizeof(T),
//    									(*it).second->overlapping_0->getXYZDim(0)*(*it).second->overlapping_0->getTY().end*sizeof(T), (*it).second->overlapping_0->getXYZDim(0)*sizeof(T),
//    									(*it).second->overlapping_0->getXYZDim(0)*(*it).second->overlapping_0->getTY().end*sizeof(T), ((T*)(*it).second->overlapping_0->getDataWithoutHDM()));
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//    						(*it).second->overlapping_0->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_0->getXYZDim(0), value[1], value[0],
//    								((*it).second->overlapping_0->getXYZDim(1) - 2*value[1])*(*it).second->overlapping_0->getXYZDim(0), value[2]);
//
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    									(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second, 0, 0, 0, value[0], value[1], value[2],
//    									(*it).second->getTX().end*sizeof(T), (*it).second->getTX().end* (*it).second->getTY().end*sizeof(T), dataLinear);
//    						}
//
//    					}
//    				}
//    			}
//    			else
//    			{
//    				////AQUITRABAJAREMOS PARA LOS ORIGIN DE Z DISTINTOS
//    				if((*it).second->getTZ().origin<(*it).second->overlapping_0->getTZ().origin)
//    				{
//    					if((*it).second->overlapping_0->hdm.get_hostValid()==0)
//    						TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_0->hdm.get_device_platform(),
//    								(*it).second->overlapping_0->hdm.get_device_type(),(*it).second->overlapping_0->hdm.get_device_number(), (*it).second->overlapping_0,
//    								0, 0, value[2], value[0], value[1], value[2], (*it).second->overlapping_0->getXYZDim(0)*sizeof(T),
//    								(*it).second->overlapping_0->getXYZDim(0)* (*it).second->overlapping_0->getTY().end*sizeof(T),
//    								(*it).second->overlapping_0->getTX().end*sizeof(T), (*it).second->overlapping_0->getTX().end *
//    								(*it).second->overlapping_0->getTY().end*sizeof(T),
//    								((T*)(*it).second->overlapping_0->getDataWithoutHDM()));
//    					T dataLinear[value[0] * value[1] * value[2]];
//    					(*it).second->overlapping_0->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_0->getXYZDim(0), value[1], value[0],
//    							(value[2])*(*it).second->overlapping_0->getXYZDim(1)*(*it).second->overlapping_0->getXYZDim(0), value[2]);
//    					if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    							(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    					{
//    						TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    								(*it).second->overlapping_0->getTX().origin, (*it).second->overlapping_0->getTY().origin, (*it).second->overlapping_0->getXYZDim(2) - value[2],
//    								value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//    					}
//
//    				}
//    				else
//    				{
//    					if((*it).second->overlapping_0->hdm.get_hostValid()==0)
//    						TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_0->hdm.get_device_platform(),
//    								(*it).second->overlapping_0->hdm.get_device_type(),(*it).second->overlapping_0->hdm.get_device_number(), (*it).second->overlapping_0,
//    								0, 0, (*it).second->overlapping_0->getXYZDim(2) - 2*value[2], value[0], value[1], value[2], (*it).second->overlapping_0->getXYZDim(0)*sizeof(T),
//    								(*it).second->overlapping_0->getXYZDim(0)* (*it).second->overlapping_0->getTY().end*sizeof(T),
//    								(*it).second->overlapping_0->getTX().end*sizeof(T), (*it).second->overlapping_0->getTX().end*(*it).second->overlapping_0->getTY().end*sizeof(T),
//    								((T*)(*it).second->overlapping_0->getDataWithoutHDM()));
//    					T dataLinear[value[0] * value[1] * value[2]];
//    					(*it).second->overlapping_0->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_0->getXYZDim(0), value[1], value[0],
//    							((*it).second->overlapping_0->getXYZDim(2) - 2*value[2])* (*it).second->overlapping_0->getTY().end*(*it).second->overlapping_0->getXYZDim(0),
//    							value[2]);
//    					if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    							(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    					{
//    						TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    								0, 0, 0,
//    								value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//    					}
//
//    				}
//
//    			}
//    		}
//    		if(((*it).second)->overlapping_1!=NULL)
//    		{
//    			(*it).second->getOverlappedArea((*it).second->overlapping_1, value);
//    			if((*it).second->getTZ().origin==(*it).second->overlapping_0->getTZ().origin)
//    			{
//
//    				if((*it).second->getTY().origin==(*it).second->overlapping_1->getTY().origin)
//    				{
//
//    					if((*it).second->getTX().origin<(*it).second->overlapping_1->getTX().origin)
//    					{
//
//    						if((*it).second->overlapping_1->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_1->hdm.get_device_platform(),
//    									(*it).second->overlapping_1->hdm.get_device_type(),(*it).second->overlapping_1->hdm.get_device_number(), (*it).second->overlapping_1, value[0], 0,
//    									0, value[0], value[1], value[2], (*it).second->overlapping_1->getXYZDim(0) *sizeof(T),
//    									(*it).second->overlapping_1->getXYZDim(0) * (*it).second->overlapping_1->getTY().end*sizeof(T), (*it).second->overlapping_1->getTX().end*sizeof(T),
//    									(*it).second->overlapping_1->getTX().end*(*it).second->overlapping_1->getTY().end*sizeof(T), (*it).second->overlapping_1->getDataWithoutHDM());
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//    						(*it).second->overlapping_1->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_1->getXYZDim(0), value[1], value[0], value[0], value[2]);
//
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    									(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    									(*it).second->getXYZDim(0)-value[0], (*it).second->overlapping_1->getTY().origin, (*it).second->overlapping_1->getTZ().origin,
//    									value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//    						}
//
//    					}
//    					else
//    					{
//    						if((*it).second->overlapping_1->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_1->hdm.get_device_platform(),
//    									(*it).second->overlapping_1->hdm.get_device_type(),(*it).second->overlapping_1->hdm.get_device_number(), (*it).second->overlapping_1,
//    									(*it).second->overlapping_1->getXYZDim(0) - 2*value[0], 0, 0, value[0], value[1], value[2], (*it).second->overlapping_1->getXYZDim(0)*sizeof(T),
//    									(*it).second->overlapping_1->getXYZDim(0)* (*it).second->overlapping_1->getTY().end*sizeof(T), (*it).second->overlapping_1->getTX().end*sizeof(T),
//    									(*it).second->overlapping_1->getTX().end*(*it).second->overlapping_1->getTY().end*sizeof(T), ((T*)(*it).second->overlapping_1->getDataWithoutHDM()));
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//    						(*it).second->overlapping_1->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_1->getXYZDim(0), value[1], value[0],
//    								(*it).second->overlapping_1->getXYZDim(0) - 2*value[0], value[2]);
//
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    									(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second, 0, 0, 0, value[0], value[1], value[2],
//    									(*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//    						}
//
//
//    					}
//    				}
//    				else
//    				{
//    					if((*it).second->getTY().origin<(*it).second->overlapping_1->getTY().origin)
//    					{
//    						if((*it).second->overlapping_1->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_1->hdm.get_device_platform(),
//    									(*it).second->overlapping_1->hdm.get_device_type(),(*it).second->overlapping_1->hdm.get_device_number(), (*it).second->overlapping_1, 0,
//    									value[1], 0, value[0], value[1], value[2], (*it).second->overlapping_1->getXYZDim(0) *sizeof(T) ,
//    									(*it).second->overlapping_1->getXYZDim(0) * (*it).second->overlapping_1->getTY().end *sizeof(T),
//    									(*it).second->getTX().end*sizeof(T), (*it).second->getTX().end* (*it).second->overlapping_1->getTY().end*sizeof(T),
//    									(*it).second->overlapping_1->getDataWithoutHDM());
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//    						(*it).second->overlapping_1->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_1->getXYZDim(0), value[1], value[0],
//    								value[0]*value[1], value[2]);
//
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    									(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second, (*it).second->overlapping_1->getTX().origin,
//    									(*it).second->getXYZDim(1)-value[1], (*it).second->overlapping_1->getTZ().origin, value[0], value[1], value[2],
//    									(*it).second->getTX().end*sizeof(T), (*it).second->getTX().end* /*(*it).second->getXYZDim(1)*/ (*it).second->getTY().end*sizeof(T), dataLinear);
//    						}
//
//    					}
//    					else
//    					{
//    						if((*it).second->overlapping_1->hdm.get_hostValid()==0)
//    							TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_1->hdm.get_device_platform(),
//    									(*it).second->overlapping_1->hdm.get_device_type(),(*it).second->overlapping_1->hdm.get_device_number(), (*it).second->overlapping_1, 0,
//    									(*it).second->overlapping_1->getXYZDim(1) - 2 * value[1], 0, value[0], value[1], value[2], (*it).second->overlapping_1->getXYZDim(0)*sizeof(T),
//    									(*it).second->overlapping_1->getXYZDim(0)*(*it).second->overlapping_1->getXYZDim(1)*sizeof(T), (*it).second->getTX().end*sizeof(T),
//    									(*it).second->getTX().end*(*it).second->getXYZDim(1)*sizeof(T), ((T*)(*it).second->overlapping_1->getDataWithoutHDM()));
//
//    						T dataLinear[value[0] * value[1] * value[2]];
//    						(*it).second->overlapping_1->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_1->getXYZDim(0), value[1], value[0],
//    								((*it).second->overlapping_1->getXYZDim(1) - 2*value[1])*(*it).second->overlapping_1->getXYZDim(0), value[2]);
//
//    						if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    						{
//    							TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(), (*it).second->hdm.get_device_type(),
//    									(*it).second->hdm.get_device_number(), (*it).second, 0, 0, 0, value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T),
//    									(*it).second->getTX().end*(*it).second->getXYZDim(1)*sizeof(T), dataLinear);
//    						}
//
//    					}
//
//    				}
//    			}
//    			else
//    			{
//    				if((*it).second->getTZ().origin<(*it).second->overlapping_1->getTZ().origin)
//    				{
//
//    					if((*it).second->overlapping_1->hdm.get_hostValid()==0)
//    						TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_1->hdm.get_device_platform(),
//    								(*it).second->overlapping_1->hdm.get_device_type(),(*it).second->overlapping_1->hdm.get_device_number(), (*it).second->overlapping_1,
//    								0, 0, value[2], value[0], value[1], value[2], (*it).second->overlapping_1->getXYZDim(0)*sizeof(T),
//    								(*it).second->overlapping_1->getXYZDim(0)* (*it).second->overlapping_1->getTY().end*sizeof(T),
//    								(*it).second->overlapping_1->getTX().end*sizeof(T), (*it).second->overlapping_1->getTX().end *
//    								(*it).second->overlapping_1->getTY().end*sizeof(T),
//    								((T*)(*it).second->overlapping_1->getDataWithoutHDM()));
//    					T dataLinear[value[0] * value[1] * value[2]];
//    					(*it).second->overlapping_1->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_1->getXYZDim(0), value[1], value[0],
//    							(value[2])*(*it).second->overlapping_1->getXYZDim(1)*(*it).second->overlapping_1->getXYZDim(0), value[2]);
//    					if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    							(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    					{
//    						TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    								(*it).second->getTX().origin, (*it).second->getTY().origin, (*it).second->getXYZDim(2) - value[2],
//    								value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//
//    					}
//    				}
//    				else
//    				{
//    					if((*it).second->overlapping_1->hdm.get_hostValid()==0)
//    						TheGlobalState().clbinding().copy_from_buffer_to_basearray_range_offset((*it).second->overlapping_1->hdm.get_device_platform(),
//    								(*it).second->overlapping_1->hdm.get_device_type(),(*it).second->overlapping_1->hdm.get_device_number(), (*it).second->overlapping_1,
//    								0, 0, (*it).second->overlapping_1->getXYZDim(2) - 2*value[2], value[0], value[1], value[2], (*it).second->overlapping_1->getXYZDim(0)*sizeof(T),
//    								(*it).second->overlapping_1->getXYZDim(0)* (*it).second->overlapping_1->getTY().end*sizeof(T),
//    								(*it).second->overlapping_1->getTX().end*sizeof(T), (*it).second->overlapping_1->getTX().end*(*it).second->overlapping_1->getTY().end*sizeof(T),
//    								((T*)(*it).second->overlapping_1->getDataWithoutHDM()));
//
//    					T dataLinear[value[0] * value[1] * value[2]];
//    					(*it).second->overlapping_1->getDataLinearOverlapp(dataLinear, value[1], (*it).second->overlapping_1->getXYZDim(0), value[1], value[0],
//    							((*it).second->overlapping_1->getXYZDim(2) - 2*value[2])* (*it).second->overlapping_1->getTY().end*(*it).second->overlapping_1->getXYZDim(0),
//    							value[2]);
//    					if((*it).second->hdm.get_deviceValid((*it).second->hdm.get_device_platform(),
//    							(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number()))
//    					{
//    						TheGlobalState().clbinding().copy_to_buffer_from_basearray_range_offset((*it).second->hdm.get_device_platform(),
//    								(*it).second->hdm.get_device_type(),(*it).second->hdm.get_device_number(), (*it).second,
//    								0, 0, 0,
//    								value[0], value[1], value[2], (*it).second->getTX().end*sizeof(T), (*it).second->getTX().end*(*it).second->getTY().end*sizeof(T), dataLinear);
//    					}
//    				}
//
//    			}
//    		}
//    	}
//    }

    void syncGhost() const
    {
    	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
    	int value[3]; //Overlap area size in rows, then columns, then slices
        CLbinding& clb = TheGlobalState().clbinding();

    	for(; it != children.end(); ++it)
    	{
                Array<T,NDIM>* const first_arr = (*it).second;
          
    		for(int i = 0; i < 2; i++)
    		{
                  
                  MultiDimArray<T,NDIM>* const second_arr = first_arr->overlapping[i];
                  
    			if(second_arr != NULL)
    			{
    				first_arr->getOverlappedArea(second_arr, value);
                          
                          const Platform_t first_arr_platform   = first_arr->hdm.get_device_platform();
                          const Device_t first_arr_device_type  = first_arr->hdm.get_device_type();
                          const int first_arr_device_num        = first_arr->hdm.get_device_number();
                          const Platform_t second_arr_platform  = second_arr->hdm.get_device_platform();
                          const Device_t second_arr_device_type = second_arr->hdm.get_device_type();
                          const int second_arr_device_num       = second_arr->hdm.get_device_number();
                          

    				if(first_arr->getTZ().origin == second_arr->getTZ().origin)
    				{
    					if(first_arr->getTY().origin == second_arr->getTY().origin)
    					{
    						if(first_arr->getTX().origin < second_arr->getTX().origin)
    						{
    							if(second_arr_device_type==ACCELERATOR && first_arr_device_type == ACCELERATOR)
    							{
    								clb.enqueue_copy(first_arr_platform, first_arr_device_type, first_arr_device_num,
    										second_arr_platform, second_arr_device_type, second_arr_device_num,
    										first_arr, second_arr, first_arr->getXYZDim(0)-value[0], value[0], value[0]*sizeof(T));
    							}
    							else {

    								if(second_arr->hdm.get_hostValid()==0)
    									clb.copy_from_buffer_to_basearray_range_offset(second_arr_platform,
    											second_arr_device_type,second_arr_device_num, second_arr,
                                                                                        value[0], 0, 0, value[0], value[1], value[2],
                                                                                        second_arr->getTX().end *sizeof(T),                          //host_row_bytes
    											second_arr->getTX().end * second_arr->getTY().end*sizeof(T), //host_slice_bytes
    											second_arr->getXYZDim(0)*sizeof(T),                          //row_bytes
    											second_arr->getXYZDim(0)*second_arr->getXYZDim(1)*sizeof(T), //slice_bytes
    											second_arr->getDataWithoutHDM());

    								//T dataLinear[value[0] * value[1] * value[2]];
                                                                HPL_STACK_STORAGE(T, dataLinear, value[0] * value[1] * value[2]);
                                                          
    								second_arr->getDataLinearOverlapp(dataLinear, value[1], second_arr->getTX().end, value[1], value[0], value[0], value[2]);

    								if((first_arr_platform != MAX_PLATFORMS) && first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
    								{
    									clb.copy_to_buffer_from_basearray_range_offset(first_arr_platform,
    											first_arr_device_type,first_arr_device_num, first_arr,
    											first_arr->getXYZDim(0)-value[0], second_arr->getTY().origin, second_arr->getTZ().origin,
    											value[0], value[1], value[2],
                                                                                        first_arr->getXYZDim(0)*sizeof(T),                            //row_bytes
                                                                                        first_arr->getXYZDim(0)* first_arr->getXYZDim(1) * sizeof(T), //slice_bytes
                                                                                        dataLinear);
    								}
    							}

    						}
    						else
    						{
    	                        if(second_arr_device_type==ACCELERATOR && first_arr_device_type==ACCELERATOR)
    	                        {
    	                                clb.enqueue_copy(first_arr_platform, first_arr_device_type, first_arr_device_num,
    	                                                 second_arr_platform, second_arr_device_type, second_arr_device_num,
    	                                                 first_arr, second_arr, 0, second_arr->getXYZDim(0) - 2*value[0], value[0]*sizeof(T));
    	                        }
    	                        else {
    	                        	if(second_arr->hdm.get_hostValid()==0)
    	                        		clb.copy_from_buffer_to_basearray_range_offset(second_arr_platform,
    	                        				second_arr_device_type,second_arr_device_num, second_arr,
    	                        				second_arr->getXYZDim(0) - 2*value[0], 0, 0, value[0], value[1], value[2],
                                                                second_arr->getTX().end*sizeof(T),                            //host_row_bytes
    	                        				second_arr->getTX().end * second_arr->getTY().end*sizeof(T),  //host_slice_bytes
    	                        				second_arr->getXYZDim(0)*sizeof(T),                           //row_bytes
                                                                second_arr->getXYZDim(0)*second_arr->getXYZDim(1)*sizeof(T),  //slice_bytes
    	                        				((T*)second_arr->getDataWithoutHDM()));

    	                        	//T dataLinear[value[0] * value[1] * value[2]];
                                        HPL_STACK_STORAGE(T, dataLinear, value[0] * value[1] * value[2]);
                                  
    	                        	second_arr->getDataLinearOverlapp(dataLinear, value[1],second_arr->getTX().end, value[1], value[0],
    	                        			second_arr->getXYZDim(0) - 2*value[0], value[2]);

    	                        	if((first_arr_platform != MAX_PLATFORMS) && first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
    	                        	{
    	                        		clb.copy_to_buffer_from_basearray_range_offset(first_arr_platform,
    	                        				first_arr_device_type,first_arr_device_num, first_arr,
    	                        				0, 0, 0,
    	                        				value[0], value[1], value[2],
                                                                first_arr->getXYZDim(0)*sizeof(T),                          //row_bytes
                                                                first_arr->getXYZDim(0)*first_arr->getXYZDim(1)*sizeof(T),  //slice_bytes
                                                                dataLinear);
    	                        	}
    	                        }
    						}
    					}
    					else
    					{
                                          
    						if(first_arr->getTY().origin < second_arr->getTY().origin)
    						{
    							if(second_arr->hdm.get_hostValid()==0)
    								clb.copy_from_buffer_to_basearray_range_offset(second_arr_platform,
    										second_arr_device_type,second_arr_device_num, second_arr,
                                                                                0, value[1], 0, value[0], value[1], value[2],
                                                                                second_arr->getTX().end *sizeof(T),                            //host_row_bytes
    										second_arr->getTX().end * second_arr->getTY().end*sizeof(T),   //host_slice_bytes
    										second_arr->getXYZDim(0)*sizeof(T),                            //row_bytes
                                                                                second_arr->getXYZDim(0)* second_arr->getXYZDim(1) *sizeof(T), //slice_bytes
    										second_arr->getDataWithoutHDM());

    							//T dataLinear[value[0] * value[1] * value[2]];
                                                        HPL_STACK_STORAGE(T, dataLinear, value[0] * value[1] * value[2]);
                                                  
    							second_arr->getDataLinearOverlapp(dataLinear, second_arr->getTY().end, second_arr->getTX().end, value[1], value[0], value[0]*value[1], value[2]);

//    							if(first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
							if((first_arr_platform!=4) && first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
    							{
    								clb.copy_to_buffer_from_basearray_range_offset(first_arr_platform,
    										first_arr_device_type,first_arr_device_num, first_arr,
    										second_arr->getTX().origin, first_arr->getXYZDim(1)-value[1], second_arr->getTZ().origin,
    										value[0], value[1], value[2],
                                                                                first_arr->getXYZDim(0)*sizeof(T),                         //row_bytes
                                                                                first_arr->getXYZDim(0)*first_arr->getXYZDim(1)*sizeof(T), //slice_bytes
                                                                                dataLinear);
    							}
    						}
    						else
    						{
    							if(second_arr->hdm.get_hostValid()==0)
    								clb.copy_from_buffer_to_basearray_range_offset(second_arr_platform,
    										second_arr_device_type,second_arr_device_num, second_arr,
                                                                                0, second_arr->getXYZDim(1) - 2 * value[1], 0, value[0], value[1], value[2],
    										second_arr->getTX().end*sizeof(T),                           //host_row_bytes
    										second_arr->getTX().end*second_arr->getTY().end*sizeof(T),   //host_slice_bytes
                                                                                second_arr->getXYZDim(0)*sizeof(T),                          //row_bytes
    										second_arr->getXYZDim(0)*second_arr->getXYZDim(1)*sizeof(T), //slice_bytes
                                                                                ((T*)second_arr->getDataWithoutHDM()));

    							//T dataLinear[value[0] * value[1] * value[2]];
                                                        HPL_STACK_STORAGE(T, dataLinear, value[0] * value[1] * value[2]);
                                                  
    							second_arr->getDataLinearOverlapp(dataLinear, second_arr->getTY().end, second_arr->getTX().end, value[1], value[0],
    									(second_arr->getXYZDim(1) - 2*value[1])*second_arr->getXYZDim(0), value[2]);

//    							if(first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
							if((first_arr_platform!=4) && first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
    							{
    								clb.copy_to_buffer_from_basearray_range_offset(first_arr_platform,
    										first_arr_device_type,first_arr_device_num, first_arr,
                                                                                0, 0, 0,
                                                                                value[0], value[1], value[2],
    										first_arr->getXYZDim(0)*sizeof(T),                           //row_bytes
                                                                                first_arr->getXYZDim(0) * first_arr->getXYZDim(1)*sizeof(T), //slice_bytes
                                                                                dataLinear);
    							}

    						}
    					}
    				}
    				else
    				{
    					if(first_arr->getTZ().origin < second_arr->getTZ().origin)
    					{
    						if(second_arr->hdm.get_hostValid() == 0)
    							clb.copy_from_buffer_to_basearray_range_offset(second_arr_platform,
    									second_arr_device_type,second_arr_device_num, second_arr,
    									0, 0, value[2], value[0], value[1], value[2],
                                                                        second_arr->getTX().end*sizeof(T),                             //host_row_bytes
    									second_arr->getTX().end*second_arr->getTY().end*sizeof(T),     //host_slice_bytes
    									second_arr->getXYZDim(0)*sizeof(T),                            //row_bytes
                                                                        second_arr->getXYZDim(0) * second_arr->getXYZDim(1)*sizeof(T), //slice_bytes
    									((T*)second_arr->getDataWithoutHDM()));
                                          
    						//T dataLinear[value[0] * value[1] * value[2]];
                                                HPL_STACK_STORAGE(T, dataLinear, value[0] * value[1] * value[2]);
                                          
    						second_arr->getDataLinearOverlapp(dataLinear, value[1], second_arr->getTX().end, value[1], value[0],
    								(value[2])*second_arr->getXYZDim(1)*second_arr->getXYZDim(0), value[2]);

//    						if(first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
						if((first_arr_platform!=4) && first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type,first_arr_device_num))
    						{
    							clb.copy_to_buffer_from_basearray_range_offset(first_arr_platform,
    									first_arr_device_type,first_arr_device_num, first_arr,
    									second_arr->getTX().origin, second_arr->getTY().origin, first_arr->getXYZDim(2) - value[2],
    									value[0], value[1], value[2],
                                                                        first_arr->getXYZDim(0)*sizeof(T),                         //row_bytes
                                                                        first_arr->getXYZDim(0)*first_arr->getXYZDim(1)*sizeof(T), //slice_bytes
                                                                        dataLinear);
    						}

    					}
    					else
    					{
    						if(second_arr->hdm.get_hostValid() == 0)
    							clb.copy_from_buffer_to_basearray_range_offset(second_arr_platform,
    									second_arr_device_type,second_arr_device_num, second_arr,
    									0, 0, second_arr->getXYZDim(2) - 2*value[2], value[0], value[1], value[2],
                                                                        second_arr->getTX().end*sizeof(T),                           //host_row_bytes
    									second_arr->getTX().end* second_arr->getTY().end*sizeof(T),  //host_slice_bytes
    									second_arr->getXYZDim(0)*sizeof(T),                          //row_bytes
                                                                        second_arr->getXYZDim(0)*second_arr->getXYZDim(1)*sizeof(T), //slice_bytes
    									((T*)second_arr->getDataWithoutHDM()));

    						//T dataLinear[value[0] * value[1] * value[2]];
                                                HPL_STACK_STORAGE(T, dataLinear, value[0] * value[1] * value[2]);
                                          
    						second_arr->getDataLinearOverlapp(dataLinear, value[1], second_arr->getTX().end, value[1], value[0],
    								(second_arr->getXYZDim(2) - 2*value[2])* second_arr->getTY().end*second_arr->getXYZDim(0), value[2]);

//    						if(first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type, first_arr_device_num))
						if((first_arr_platform!=4) && first_arr->hdm.get_deviceValid(first_arr_platform, first_arr_device_type, first_arr_device_num))
    						{
    							clb.copy_to_buffer_from_basearray_range_offset(first_arr_platform,
    									first_arr_device_type,first_arr_device_num, first_arr,
    									0, 0, 0,
    									value[0], value[1], value[2],
                                                                        first_arr->getXYZDim(0)*sizeof(T),                         //row_bytes
                                                                        first_arr->getXYZDim(0)*first_arr->getXYZDim(1)*sizeof(T), //slice_bytes
                                                                        dataLinear);
    						}
    					}

    				}
    			}
                  //return; //BBF DEBUG
    		}
    	}
    }


//    void invalidSon( Platform_t platform_id, Device_t device_type, int device_n) const
//    {
//     	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
//         	for(; it != children.end(); it++)
//          {
//                 	Array<T,NDIM>* ar = (*it).second;
//                 	ar->getData(HPL_RD);
//                 	ar->hdm.set_deviceValid(platform_id, device_type, device_n, false);
//                 	ar->hdm.add_dbuffer(platform_id, device_type, device_n, NULL);
//          }
//    }

//    void writeFather() const
//    {
//    	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
//    	for(; it != children.end(); it++)
//    	{
//    		Array<T,NDIM>* ar = (*it).second;
//    		ar->getData(HPL_RDWR);
//    	}
//    }

    /// Object destructor
    /** the data v_ is only deallocated if it is owned_ */
    virtual ~MultiDimArray()
    {
        hdm.free_dbuffers();
        typename std::map<KeyChildrenMap, Array<T,NDIM>*>::iterator  it(children.begin());
        for(; it != children.end(); it++)
        {
                        delete((*it).second);
        }

      if (owned_ && v_) {
	if (fastMem_) TheGlobalState().clbinding().freeMem(static_cast<void *>(v_));
	else free (v_);
      }
    }
    
    //////////////////////////////////////////////////////COHERENCY MEMORY BEGIN/////////////////////////////////////

    void onlyRead(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq) final
    {
	refresh(platform_id, device_type, device_n, cq, this);
    }

    void readWrite(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq) final
    {
    	refresh(platform_id, device_type, device_n, cq, this);
    	markOnlyOwner(platform_id, device_type, device_n, cq, this);
    }

    void onlyWrite(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq) final
    {
      if(platform_id==MAX_PLATFORMS) {
        readWrite((Platform_t)MAX_PLATFORMS, (Device_t)0, 0, cq); //onlyWrite in the host behaves as readWrite
      } else {
        markOnlyOwner(platform_id, device_type, device_n, cq, this);
      }
    }

    void killChildren()
    {
        typename std::map<KeyChildrenMap, Array<T,NDIM>*>::iterator  it(this->children.begin());
        for(; it != this->children.end(); it++)
        {
                        delete((*it).second);
       	}
        this->children.clear();
    }

    //////////////////////////////////////////////////////COHERENCY MEMORY END //////////////////////////////////////

    ///////////////////////////////////////////////////////CHILDREN_CODE_BEGIN///////////////////////////////////////
      protected:
        std::map<KeyChildrenMap, Array<T,NDIM>*> children;
        DistribType distrib;
        int distribSize;
        //MultiDimArray<T,NDIM>* father;
        MultiDimArray<T,NDIM>** overlapping;
    //////////////////////////////////////////////////////CHILDREN_CODE_END//////////////////////////////////////////
        //////////////////////////////////////////////////COHERENCY MEMORY BEGIN/////////////////////////////////////


    /// @internal BBF: IT does not use "ba". ?
        void allocBuffer(Platform_t platform_id, Device_t device_type, int device_number,cl::CommandQueue *cq, BaseArray* ba)
        {
          if(buf_[platform_id][device_type][device_number] == None) {
        		if(!father) {
        			buf_[platform_id][device_type][device_number] = OwnBuffer;
        			//find children that already exist in the device, move them to buffers inside the parent
        			typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
        			for (; it != children.end(); it++) {
        				Array<T,NDIM>* subuffer = (*it).second;
        				if(subuffer->buf_[platform_id][device_type][device_number] != None) {
        					assert(subuffer->buf_[platform_id][device_type][device_number] == OwnBuffer);
        					if(subuffer->updated_ && subuffer->hdm.get_deviceValid(platform_id,device_type, device_number) && subuffer->hdm.isOnlyCopy(platform_id, device_type, device_number)) {
        						subuffer->hdm.readWrite((Platform_t)MAX_PLATFORMS, (Device_t)0,0,cq,subuffer);
        					}
        					subuffer->hdm.set_deviceValid(platform_id, device_type, device_number, false);
        					delete(subuffer->hdm.get_dbuffer(platform_id, device_type, device_number));
        					subuffer->buf_[platform_id][device_type][device_number] = SubBuffer;
        				}
        			}
        		} else {
        			buf_[platform_id][device_type][device_number] = (static_cast<MultiDimArray<T, NDIM>*>(father)->buf_[platform_id][device_type][device_number] == None) ? OwnBuffer : SubBuffer;
        		}
          }
        }

        /// @internal BBF: IT does not use "ba". ?
        void simple_rebuild_obsolete_parent_in_host(cl::CommandQueue *cq, BaseArray* ba)
        {
        	Platform_t platform_id;
        	Device_t device_type;
        	int device_n;
        	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
        	for (; it != children.end(); it++) {
        		Array<T,NDIM>* subuffer = (*it).second;
        		if (subuffer->updated_) {
        			subuffer->hdm.where(&platform_id, &device_type, &device_n);
        			if(platform_id!=MAX_PLATFORMS)
        				subuffer->hdm.onlyRead((Platform_t)MAX_PLATFORMS,(Device_t)0,0,cq,subuffer);
        		}
        	}
        }

        void complex_rebuild_obsolete_parent_in_host(cl::CommandQueue *cq, InternalMultiDimArray* ba)
    {
      Platform_t platform_id;
      Device_t device_type;
      int device_n;
      hdm.where(&platform_id, &device_type, &device_n);
      
      typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
      for (; it != children.end(); it++) {
        Array<T,NDIM>* child = (*it).second;
        if (child->updated_) {
          if(child->hdm.get_deviceValid(platform_id, device_type, device_n)) {
            child->hdm.set_deviceValid((Platform_t)MAX_PLATFORMS, (Device_t)0,0, true); //the hdm_.onlyRead(0); does the movement
          } else { //if only exists in 0, update it in d so it is not overwritten with wrong datayar
            if(child->hdm.get_deviceValid((Platform_t)MAX_PLATFORMS, (Device_t)0,0) && child->hdm.isOnlyCopy((Platform_t)MAX_PLATFORMS,(Device_t)0,0)) {
              child->refresh(platform_id, device_type, device_n,cq,child);
            }
          }
        }
      }
      
      hdm.onlyRead((Platform_t)MAX_PLATFORMS, (Device_t)0, 0,cq,ba); //bring array from d to 0
      
      simple_rebuild_obsolete_parent_in_host(cq,ba);
    }

    /// @internal BBF: IT does not use "ba". ?
        void copy_missing_children(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq, BaseArray* ba)
        {
        	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::const_iterator  it(children.begin());
        	for (; it != children.end(); it++) {
        		Array<T,NDIM>* child = (*it).second;
            if (child->updated_) {
             	if(child->hdm.get_deviceValid(platform_id, device_type, device_n)) {
              } else {
            	  child->hdm.onlyRead((Platform_t)MAX_PLATFORMS, (Device_t)0, 0,cq,child);
            	  child->hdm.onlyRead(platform_id, device_type, device_n,cq,child);
              }
            }
          }
        }

        void refresh(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq, MultiDimArray<T, NDIM>* ba)
        {
        	Platform_t w_platform_id;
        	Device_t w_device_type;
        	int w_device_n;

        	allocBuffer(platform_id, device_type, device_n, cq, ba);

        	if(updated_) {

        		if(!hdm.get_deviceValid(platform_id, device_type, device_n)) {

        			hdm.where(&w_platform_id, &w_device_type, &w_device_n);
if(!TheGlobalState().clbinding().get_copy_faster(w_platform_id, w_device_type))
{
        			if(w_platform_id!=MAX_PLATFORMS) {

        				hdm.onlyRead((Platform_t)MAX_PLATFORMS, (Device_t)0, 0, cq, ba);

        			}
        			if(platform_id!=MAX_PLATFORMS) {

        				hdm.onlyRead(platform_id, device_type, device_n, cq, ba);
        			}
}else{
        			//if the source and target devices are not the HOST and the
        			// the array to copy is consecutive in memory, we will do
        			// the D->D copy by means of enqueue_copy.
                    if((w_platform_id!=MAX_PLATFORMS && platform_id!=MAX_PLATFORMS) &&
                           	ba->isConsecutive())
                    {
                            hdm.enqueueCopy(platform_id, device_type, device_n, cq, ba);
                    }
                    else
                    {
                    	//The copy will be done by means of two transfers, one to the host and
                    	// other one from the host to the device.
                            if(w_platform_id!=MAX_PLATFORMS) {
                                    hdm.onlyRead((Platform_t)MAX_PLATFORMS, (Device_t)0, 0, cq, ba);
                           	}
                           	if(platform_id!=MAX_PLATFORMS) {
                                    hdm.onlyRead(platform_id, device_type, device_n, cq, ba);
                            }

                    }

}
        		}

        	} else {
        		if(!father) { //writes on one or more children had invalidated all the copies of this parent Array

        			if(platform_id==MAX_PLATFORMS) {
        				if(hdm.get_deviceValid((Platform_t)MAX_PLATFORMS, (Device_t)0, 0) || canBeRebuiltOnlyFromChildren()) {
        					simple_rebuild_obsolete_parent_in_host(cq,ba);
        				} else {

        					complex_rebuild_obsolete_parent_in_host(cq,ba);
        				}
        				hdm.onlyWrite((HPL::Platform_t)MAX_PLATFORMS,(HPL::Device_t)0,0);    //sets this Array valid in host, invalid anywhere else
        			} else { //branches (1) and (3) of algorithm

        				if(canBeRebuiltOnlyFromChildren()) { //branch (1)-if + (3)(2)-if + (3)(0)-sometimes

        					simple_rebuild_obsolete_parent_in_host(cq,ba);
        					hdm.onlyWrite((HPL::Platform_t)MAX_PLATFORMS,(HPL::Device_t)0,0);    //sets this Array valid in host, invalid anywhere else
        					hdm.onlyRead(platform_id, device_type, device_n, cq, ba);

        				} else {

        					if(hdm.get_deviceValid(platform_id, device_type, device_n)) { //branch (1)-else

        						copy_missing_children(platform_id, device_type, device_n,cq,ba);
        						hdm.onlyWrite(platform_id, device_type, device_n);    //sets this Array valid in device, invalid anywhere else
        					} else { // branch (3)(2)-else + (3)(0)-sometimes
        						refresh((Platform_t)MAX_PLATFORMS,(Device_t)0,0,cq,ba);
        						hdm.onlyRead(platform_id, device_type, device_n,cq,ba);
        					}
        				}
        			}
        		} else { //writes on the parent had invalidated all the copies of this subarray
        			// This algorithm is good when parent_->updated_ is true
        			// If the parent is not updated, but neither is this subarray, that is because after its latest updated_ status,
        			//other subarrays, different from this one were written. Since we assume that subarrays do not overlap, the
        			//last valid version of this subarray is in the memories where the parent_->hdm is valid.
        			if (father->hdm.get_deviceValid(platform_id, device_type, device_n)==true) {
        				hdm.onlyWrite(platform_id, device_type, device_n); //this subbuffer is automatically the correct one, as it resides inside
        			} else {
        				//we move the whole Array to the host if needed, then update the subarray in device. We could also only update the subarray
        				//the reason for moving the whole array is that we think probably more subarrays will be used soon
        				if(!father->hdm.get_deviceValid((Platform_t)MAX_PLATFORMS, (Device_t)0, 0))
        					static_cast<MultiDimArray<T, NDIM>*>(father)->refresh((Platform_t)MAX_PLATFORMS,(Device_t)0,0,cq,ba);
        				hdm.onlyWrite((HPL::Platform_t)MAX_PLATFORMS,(HPL::Device_t)0,0);    //sets this Array valid in host, invalid anywhere else
        				if(platform_id!=MAX_PLATFORMS) {
        					hdm.onlyRead(platform_id, device_type, device_n,cq,ba);
        				}
        			}

        			static_cast<MultiDimArray<T, NDIM>*>(father)->childrenDisabled_ = false;

        		}
        		updated_ = true;

        	}

        	assert(updated_);
        	assert(hdm.get_deviceValid(platform_id, device_type, device_n));
        }

        void markOnlyOwner(Platform_t platform_id, Device_t device_type, int device_n, cl::CommandQueue *cq, InternalMultiDimArray* ba)
        {

        	allocBuffer(platform_id, device_type, device_n,cq,ba);

        	if(!father) { //This is a parent Array
        		if (!childrenDisabled_) {
        			typename std::map<KeyChildrenMap, Array<T,NDIM>*>::iterator  it(children.begin());
        			for(; it != children.end(); it++)
        			{
        				Array<T,NDIM>* ar = (*it).second;

        				ar->totallyInvalidate();
        			}
        			childrenDisabled_ = true;
        			// Notice that all the children copies are disabled, even those in the same device,
        			// But when we want to use them back, the process will be very fast if the array is updated there
        		}
        	} else { //This is a subarray

        		static_cast<MultiDimArray<T, NDIM>*>(father)->totallyInvalidate();
        		static_cast<MultiDimArray<T, NDIM>*>(father)->childrenDisabled_ = false;
        		// Notice that we disable the parent even if we are writing in the host or a device where it is fully updated
        		// But when we want to use it back, the recovery process will be fast if the array is updated there
        	}

        	updated_ = true;
        	hdm.onlyWrite(platform_id, device_type, device_n);
        	assert(updated_);

        }


        void totallyInvalidate()
        {
        	updated_ = false;
        }

        void childrenTotallyCover(bool b) { childrenTotallyCover_ = b; }

        bool canBeRebuiltOnlyFromChildren(){
        	typename std::map<KeyChildrenMap, Array<T,NDIM>*>::iterator  it(children.begin());
        	for(; it != children.end(); it++)
        	{
        		Array<T,NDIM>* ar = (*it).second;
        		if(!ar->updated_) return false;
        	}
        	return  childrenTotallyCover_;
        }

        //////////////////////////////////////////////////COHERENCY MEMORY END///////////////////////////////////////

  private:

        /// Obtain the size of the area overlapped between this array and the other one.
        void getOverlappedArea(MultiDimArray<T,NDIM>* const other, int arr[3]) const
        {
        	if(this->getTZ().origin == other->getTZ().origin)
        	{
        		if(this->getTY().origin == other->getTY().origin)
        		{
        			if(this->getTX().origin < other->getTX().origin)
        			{
        				arr[0] = abs((this->getTX().origin + this->getXYZDim(0)) - other->getTX().origin)/2;
        				arr[1] = abs((this->getTY().origin + this->getXYZDim(1)) - other->getTY().origin);
        				arr[2] = abs((this->getTZ().origin + this->getXYZDim(2)) - other->getTZ().origin);
        			}
        			else
        			{
        				arr[0] = abs(this->getTX().origin - (other->getTX().origin + other->getXYZDim(0)))/2;
        				arr[1] = abs(this->getTY().origin - (other->getTY().origin + other->getXYZDim(1)));
        				arr[2] = abs(this->getTZ().origin - (other->getTZ().origin + other->getXYZDim(2)));
        			}
        		}
        		else
        		{
        			/////////////////////////////////////SUPONDREMOS QUE AMBAS CAJAS SON DE IGUAL TAMANO EN X
        			if(this->getTY().origin < other->getTY().origin)
        			{
        				arr[0] = abs((this->getTX().origin + this->getXYZDim(0)) - other->getTX().origin);
        				arr[1] = abs((this->getTY().origin + this->getXYZDim(1)) - other->getTY().origin)/2;
        				arr[2] = abs((this->getTZ().origin + this->getXYZDim(2)) - other->getTZ().origin);
        			}
        			else
        			{
        				arr[0] = abs(this->getTX().origin - (other->getTX().origin + other->getXYZDim(0)));
        				arr[1] = abs(this->getTY().origin - (other->getTY().origin + other->getXYZDim(1)))/2;
        				arr[2] = abs(this->getTZ().origin - (other->getTZ().origin + other->getXYZDim(2)));
        			}

        		}
        	}
        	else
        	{

        		if(this->getTZ().origin < other->getTZ().origin)
        		{
        			arr[0] = abs((this->getTX().origin + this->getXYZDim(0)) - other->getTX().origin);
        			arr[1] = abs((this->getTY().origin + this->getXYZDim(1)) - other->getTY().origin);
        			arr[2] = abs((this->getTZ().origin + this->getXYZDim(2)) - other->getTZ().origin)/2;
        		}
        		else
        		{
        			arr[0] = abs(this->getTX().origin - (other->getTX().origin + other->getXYZDim(0)));
        			arr[1] = abs(this->getTY().origin - (other->getTY().origin + other->getXYZDim(1)));
        			arr[2] = abs(this->getTZ().origin - (other->getTZ().origin + other->getXYZDim(2)))/2;
        		}

        	}

        	if(arr[0]==0) arr[0] = 1;
        	if(arr[1]==0) arr[1] = 1;
        	if(arr[2]==0) arr[2] = 1;
        }


    /// Print the declaration (NOT the usage) of this variable to the Codifier
    /** Notice that the string generated depends on the state of the Codifier:
        - If its state is ::NotBuilding, nothing is done
        - If the state is ::BuildingFBody, the string is like "datatype arrayname[dim1][dim2]..."
        - If the state is ::BuildingFHeader, the string is like "__global datatype *arrayname"
          folowed by i=0,...,NDIM-1 strings of the shape ", int arrayname_i" to receive the cumulative
          sizes of the dimensions 0,...,NDIM-1 of the array required for its indexing
      */
    void print() const {
      if(!HPL_STATE(NotBuilding))
	InternalMultiDimArray::print(TypeInfo<T>::String, dims_, NDIM);
    }
    
    int dims_[NDIM];        ///< Array dimensions. 0 is the most significant dimension, 1 the next one, etc.
    int cumulSize_[NDIM];   ///< Cumulative size of each dimension (for indexing). Decreasing value up to 1.
    T * const v_;           ///< Pointer to the data of the array
  };


  /// MultiDimArray specialization for 1-D arrays. Only provides fancy constructors.
  /** @tparam T     type of the elements */
  template<typename T, ArrayFlags AF>
  class Array<T, 1, AF> : public MultiDimArray<T, 1> {
  public:
    
	/**
	 * @brief Usual constructor
	 * @param[in] i Elements of the array.
	 * @param[in] p Pointer to the elements of type T. This parameter is optional and if
	 * it is passed as argument it won't be owned by this array else it will be owned by
	 * this array.
	 */
    Array(int i , T* p =0)
    : MultiDimArray<T, 1>(i, p, AF)
    { }

    /**
     * @brief Constructor when the pointer to data has a different data type T2.
     * @tparam T2 Data type of the data array.
     * @param[in] i Dimensions of the array
     * @param[in] p Pointer to the elements of type T2.
     */
    template<typename T2>
    Array(int i, T2 *p =0)
    : MultiDimArray<T, 1>(i, reinterpret_cast<T*>(p), AF)
    { }

    /**
     * @brief Constructor only allowed when building a variable for a parallel function header.
     */
    Array()
    : MultiDimArray<T, 1>(0, 0, AF)
    { assert(HPL_STATE(BuildingFHeader)); }

    using MultiDimArray<T,1>::operator();

//    Array<T,1,AF>* operator()(Tuple tX) {
//    	assert(HPL_STATE(NotBuilding));
//    	Tuple tY(0,0);
//    	Tuple tZ(0,0);
//    	if(this->children.find(KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end))!=this->children.end())
//    	{
//    		return ( this->children[KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end)]);
//    	}
//    	else
//    	{
//    		////WITH getData we assure that the child starts with valid host data
//    		Array<T,1,AF>* child = new Array<T,1,AF>(tX.getSize(), ((T*)this->getData/*WithoutHDM*/(HPL_RD)+tX.origin));
//    		this->registerChild(child, tX, tY, tZ);
//    		tX.end = this->getXYZDim(0);
//    		tY.end = 1;
//    		tZ.end = 1;
//    		child->setTX(tX);
//    		child->setTY(tY);
//    		child->setTZ(tZ);
//
//    		child->father = this;
//    		return child;
//    	}
//    }

    Array<T,1,AF>& setDistrib(DistribType d, int size = 0)
    {
        MultiDimArray<T,1>::distrib = d;
        MultiDimArray<T,1>::distribSize = size;
        return *this;
    }
    
    DistribType getDistrib()
    {
        return MultiDimArray<T,1>::distrib;
    }
    
    int getDistribSize()
    {
    	return MultiDimArray<T,1>::distribSize;
    }

    Array<T,1,AF>& operator()(Tuple tX) {
    	assert(HPL_STATE(NotBuilding));
    	Tuple tY(0,0);
    	Tuple tZ(0,0);
    	if(this->children.find(KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end))!=this->children.end())
    	{
    		return *( this->children[KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end)]);
    	}
    	else
    	{
    		////WITH getData we assure that the child starts with valid host data
    		Array<T,1,AF>* child = new Array<T,1,AF>(tX.getSize(), ((T*)this->getData/*WithoutHDM*/()+tX.origin));
    		this->registerChild(child, tX, tY, tZ);
    		tX.end = this->getXYZDim(0);
    		tY.end = 1;
    		tZ.end = 1;
    		child->setTX(tX);
    		child->setTY(tY);
    		child->setTZ(tZ);
    		child->buf_[MAX_PLATFORMS][0][0] = BufferType::SubBuffer;
    		child->father = this;
    		this->childrenDisabled_=false;

    		this->refresh((Platform_t)MAX_PLATFORMS,(Device_t)0,0,NULL,NULL);
    		return *child;
    	}
    }

    Array<T,1,AF>& get(int pos)
	{
    	typename std::map<KeyChildrenMap, Array<T,1,AF>*>::const_iterator it(this->children.begin());
    	int j = 0;
    	for(; it != this->children.end(); it++)
    	{
    		Array<T,1,AF>* ar = (*it).second;
    		if(j == pos) return *ar;
    		j++;
    	}
    	HPLExceptionIf(pos>=this->children.size(), "get(int pos) called with an out of bounds value!");
    	return *this;
	}

//    void killChildren()
//    {
//    	typename std::map<KeyChildrenMap, Array<T,1>*>::iterator  it(this->children.begin());
//    	for(; it != this->children.end(); it++)
//    	{
//    		delete((*it).second);
//    	}
//    	this->children.clear();
//    }

    //////////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////////////

    using MultiDimArray<T,1>::operator=;
  };
  

  /// MultiDimArray specialization for 2-D arrays. Only provides fancy constructors.
  /** @tparam T     type of the elements */
  template<typename T, ArrayFlags AF>
  class Array<T, 2, AF> : public MultiDimArray<T, 2> {
  public:
    
	/**
	 * @brief Usual constructor
	 * @param[in] i Elements of the first dimension of the array.
	 * @param[in] j Elements of the second dimension of the array.
	 * @param[in] p Pointer to the elements of type T. This parameter is optional and if
	 * it is passed as argument it won't be owned by this array else it will be owned by
	 * this array.
	 */
    Array(int i, int j, T* p =0)
    : MultiDimArray<T, 2>(i, j, p, AF)
    { }
    
    /**
     * @brief Constructor when the pointer to data has a different data type T2.
     * @tparam T2 Data type of the data array.
     * @param[in] i Elements of the first dimension of the array
     * @param[in] j Elements of the second dimension of the array
     * @param[in] p Pointer to the elements of type T2.
     */
    template<typename T2>
    Array(int i, int j, T2 *p =0)
    : MultiDimArray<T, 2>(i, j, reinterpret_cast<T*>(p), AF)
    { }
    
    /**
     * @brief Constructor only allowed when building a variable for a parallel function header.
     */
    Array()
    : MultiDimArray<T, 2>(0, 0, 0, AF)
    { assert(HPL_STATE(BuildingFHeader)); }

    using MultiDimArray<T,2>::operator();

    ////////////////////////////////////////////CHILDREN_CODE_BEGIN////////////////////////////////////////////

//    Array<T,2,AF>* operator()(Tuple tY, Tuple tX) {
//    	assert(HPL_STATE(NotBuilding));
//    	Tuple tZ(0,0);
//
//    	if(this->children.find(KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end))!=this->children.end())
//    	{
//    		return ( this->children[KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end)]);
//    	}
//    	else
//    	{
//    		Array<T,2,AF>* child = new Array<T,2,AF>(tY.getSize(), tX.getSize(), ((T*)this->getData/*WithoutHDM*/(HPL_RD)+ (tY.origin* this->getXYZDim(0)) + tX.origin));
//    		this->registerChild(child, tX, tY, tZ);
//    		tX.end = this->getXYZDim(0);
//    		tY.end = this->getXYZDim(1);
//    		tZ.end = 1;
//    		child->setTX(tX);
//    		child->setTY(tY);
//    		child->setTZ(tZ);
//    		child->father = this;
//    		return child;
//    	}
//    }

    Array<T,2,AF>& setDistrib(DistribType d, int size = 0)
    {
        MultiDimArray<T,2>::distrib = d;
        MultiDimArray<T,2>::distribSize = size;
        return *this;
    }
    DistribType getDistrib()
    {
        return MultiDimArray<T,2>::distrib;
    }
    int getDistribSize()
    {
    	return MultiDimArray<T,2>::distribSize;
    }

    Array<T,2,AF>& operator()(Tuple tY, Tuple tX) {
        assert(HPL_STATE(NotBuilding));
        Tuple tZ(0,0);

        if(this->children.find(KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end))!=this->children.end())
        {
                return *( this->children[KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end)]);
        }
        else
        {
		if(this->father!=NULL) ((Array<T,2,AF>*)this->father)->syncGhost();
 		else this->syncGhost();

                Array<T,2,AF>* child = new Array<T,2,AF>(tY.getSize(), tX.getSize(), ((T*)this->getData/*WithoutHDM*/(HPL_RD)+ (tY.origin* this->getXYZDim(0)) + tX.origin));
                this->registerChild(child, tX, tY, tZ);
               	tX.end = this->getXYZDim(0);
                tY.end = this->getXYZDim(1);
                tZ.end = 1;
                child->setTX(tX);
               	child->setTY(tY);
               	child->setTZ(tZ);
               	child->father = this;
               	child->buf_[MAX_PLATFORMS][0][0] = SubBuffer;
               	this->childrenDisabled_=false;
               	this->refresh((Platform_t)MAX_PLATFORMS,(Device_t)0,0,NULL,NULL);
                return *child;
       	}
    }

    Array<T,2,AF>& get(int pos)
	{
    	typename std::map<KeyChildrenMap, Array<T,2,AF>*>::const_iterator it(this->children.begin());
    	int j = 0;
    	for(; it != this->children.end(); it++)
    	{
    		Array<T,2,AF>* ar = (*it).second;
    		if(j == pos) return *ar;
    		j++;
    	}
    	HPLExceptionIf(pos>=this->children.size(), "get(int pos) called with an out of bounds value!");
    	return *this;
	}

//    void killChildren()
//    {
//    	typename std::map<KeyChildrenMap, Array<T,2>*>::iterator  it(this->children.begin());
//    	for(; it != this->children.end(); it++)
//    	{
//    		delete((*it).second);
//    	}
//    	this->children.clear();
//    }

    //////////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////////////

    using MultiDimArray<T,2>::operator=;

  };
  
  /// MultiDimArray specialization for 3-D arrays. Only provides fancy constructors.
  /** @tparam T     type of the elements */
  template<typename T, ArrayFlags AF>
  class Array<T, 3, AF> : public MultiDimArray<T, 3> {
  public:
    
	/**
	 * @brief Usual constructor
	 * @param[in] i Elements of the first dimension of the array.
	 * @param[in] j Elements of the second dimension of the array.
	 * @param[in] k Elements of the third dimension of the array.
	 * @param[in] p Pointer to the elements of type T. This parameter is optional and if
	 * it is passed as argument it won't be owned by this array else it will be owned by
	 * this array.
	 */
    Array(int i, int j, int k, T* p =0)
    : MultiDimArray<T, 3>(i, j, k, p, AF)
    { }
    
    /**
     * @brief Constructor when the pointer to data has a different data type \c T2.
     * @tparam T2 Data type of the data array.
     * @param[in] i Elements of the first dimension of the array
     * @param[in] j Elements of the second dimension of the array
     * @param[in] k Elements of the third dimension of the array
     * @param[in] p Pointer to the elements of type T2.
     */
    template<typename T2>
    Array(int i, int j, int k, T2 *p =0)
    : MultiDimArray<T, 3>(i, j, k, reinterpret_cast<T*>(p), AF)
    { }
    
    /**
     * @brief Constructor only allowed when building a variable for a parallel function header.
     */
    Array()
    : MultiDimArray<T, 3>(0, 0, 0,0 , AF)
    { assert(HPL_STATE(BuildingFHeader)); }

    using MultiDimArray<T,3>::operator();

//    Array<T,3,AF>* operator()(Tuple tZ, Tuple tY, Tuple tX) {
//    	assert(HPL_STATE(NotBuilding));
//    	if(this->children.find(KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end))!=this->children.end())
//    	{
//    		return ( this->children[KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end)]);
//    	}
//    	else
//    	{
//    		Array<T,3,AF>* child = new Array<T,3,AF>(tZ.getSize(), tY.getSize(), tX.getSize(), ((T*)this->getData/*WithoutHDM*/(HPL_RD)+ (tZ.origin * this->getXYZDim(1) * this->getXYZDim(0)) + (tY.origin* this->getXYZDim(0)) + tX.origin));
//    		this->registerChild(child, tX, tY, tZ);
//    		tX.end = this->getXYZDim(0);
//    		tY.end = this->getXYZDim(1);
//    		tZ.end = this->getXYZDim(2);
//    		child->setTX(tX);
//    		child->setTY(tY);
//    		child->setTZ(tZ);
//    		child->father = this;
//    		return child;
//    	}
//    }
    Array<T,3,AF>& setDistrib(DistribType d, int size = 0)
    {
     	MultiDimArray<T, 3>::distrib = d;
     	MultiDimArray<T, 3>::distribSize = size;
        return *this;
    }
    DistribType getDistrib()
    {
     	return MultiDimArray<T,3>::distrib;
    }
    int getDistribSize()
    {
    	return MultiDimArray<T,3>::distribSize;
    }

    Array<T,3,AF>& operator()(Tuple tZ, Tuple tY, Tuple tX) {
    	assert(HPL_STATE(NotBuilding));
    	if(this->children.find(KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end))!=this->children.end())
    	{
    		return *( this->children[KeyChildrenMap(tX.origin, tX.end, tY.origin, tY.end, tZ.origin, tZ.end)]);
    	}
    	else
    	{
		if(this->father!=NULL) ((Array<T,3,AF>*)this->father)->syncGhost();
                else this->syncGhost();

    		Array<T,3,AF>* child = new Array<T,3,AF>(tZ.getSize(), tY.getSize(), tX.getSize(), ((T*)this->getData/*WithoutHDM*/(HPL_RD)+ (tZ.origin * this->getXYZDim(1) * this->getXYZDim(0)) + (tY.origin* this->getXYZDim(0)) + tX.origin));
    		this->registerChild(child, tX, tY, tZ);
    		tX.end = this->getXYZDim(0);
    		tY.end = this->getXYZDim(1);
    		tZ.end = this->getXYZDim(2);
    		child->setTX(tX);
    		child->setTY(tY);
    		child->setTZ(tZ);
    		child->father = this;
    		child->buf_[MAX_PLATFORMS][0][0] = SubBuffer;
    		this->childrenDisabled_=false;
    		this->refresh((Platform_t)MAX_PLATFORMS,(Device_t)0,0,NULL,NULL);
    		return *child;
    	}
    }

    Array<T,3,AF>& get(int pos)
	{
    	typename std::map<KeyChildrenMap, Array<T,3,AF>*>::const_iterator it(this->children.begin());
    	int j = 0;
    	for(; it != this->children.end(); it++)
    	{
    		Array<T,3,AF>* ar = (*it).second;
    		if(j == pos) return *ar;
    		j++;
    	}
    	HPLExceptionIf(pos>=this->children.size(), "get(int pos) called with an out of bounds value!");
    	return *this;
	}

//    void killChildren()
//    {
//    	typename std::map<KeyChildrenMap, Array<T,3>*>::iterator  it(this->children.begin());
//    	for(; it != this->children.end(); it++)
//    	{
//    		delete((*it).second);
//    	}
//    	this->children.clear();
//    }

    //////////////////////////////////////////CHILDREN_CODE_END/////////////////////////////////////////////////

    using MultiDimArray<T,3>::operator=;

  };
  
  
  /// Dumps any Array to a std::ostream
  template<typename T, int NDIM>
  std::ostream &operator<<(std::ostream &os, const Array<T, NDIM> &a)
  {
    String_t tmp = a.name() + '(';
    for(int i = 0; i < NDIM; i++) {
      if(i) tmp += ", ";
      tmp += stringize(a.getDimension(i));
    }
    tmp += ')';
    
    os << '[' << tmp << ']';
    return os;
  }

  
  ///Array that points to another array to enable T *p=(T*)q
  template<typename T>
  class AliasArray : public MultiDimArray<T, 1> {
    
    MultiDimArray<T, 1> * const p;
    
  public:
    
    template<typename T2, int N, ArrayFlags AF>
    AliasArray(Array<T2, N, AF>& ar) :
    MultiDimArray<T, 1>(-1, 0, Private),
    p(reinterpret_cast<MultiDimArray<T, 1> *>(&ar))
    {
      assert(HPL_STATE(BuildingFBody));
      
      String_t type_str(ar.storageName() + TypeInfo<T>::String + " *");
      String_t s(type_str + " const " + this->name() + " = (" + type_str + ")" + ar.name());
      TheGlobalState().getCodifier().add(s, true);
    }
    
    template<typename T2, int N>
    AliasArray(const IndexedArray<T2, N>& ia) :
    MultiDimArray<T, 1>(-1, 0, Private),
    p(const_cast<MultiDimArray<T, 1> *>(reinterpret_cast<const MultiDimArray<T, 1> *>(ia.getMultiDimArray())))
    {
      assert(HPL_STATE(BuildingFBody));
      
      String_t type_str(ia.getBaseArray()->storageName() + TypeInfo<T>::String + " *");
      String_t s(type_str + " const " + this->name() + " = (" + type_str + ")(&" + ia.string() +")");
      TheGlobalState().getCodifier().add(s, true);
    }
    
    BaseArray * getUnderlyingBaseArray() const { return p; }

    /** @name Data access functions for parallel code. 
     *  They must be used to express accesses to the array in parallel code.
     *  They MUST NOT be used in host code.
     */
    ///@{
    
    /// Indexing with an Array scalar in parallel code
    template<typename SCALARTYPE>
    IndexedArray<T, 1> operator[](const Array<SCALARTYPE, 0>& i) const
    {
      //assert(HPL_STATE(BuildingFBody));
      TheGlobalState().getArrayInfo(p)->addAccess(); //Increments in one the number of accesses to the referenced array.
      return IndexedArray<T, 1>(p, i.name(), this);
    }
    
    /// Indexing with an integer in parallel code
    IndexedArray<T, 1> operator[](int i) const
    {
      //assert(HPL_STATE(BuildingFBody));
      TheGlobalState().getArrayInfo(p)->addAccess(); //Increments in one the number of accesses to the referenced array.
      return IndexedArray<T, 1>(p, stringize(i), this);
    }
    
    /// Indexing with an arbitrary PETE expression template in parallel code
    template<class RHS>
    IndexedArray<T, 1> operator[](const Expression<RHS> &i) const
    {
      //assert(HPL_STATE(BuildingFBody));
      String_t tmp = i.string();
      i.remove();
      TheGlobalState().getArrayInfo(p)->addAccess(); //Increments in one the number of accesses to the referenced array.
      return IndexedArray<T, 1>(p, tmp, this);
    }
    ///@}
    
  };
  
}


#endif
