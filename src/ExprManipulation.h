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
/// \file     ExprManipulation.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef EXPRMANIPULATION_H
#define EXPRMANIPULATION_H

#include <string>
#include <vector>
#include "Stringizer.h"

/// Avoids reload of the PETE operator tags
#define PETE_PETE_OPERATORTAGS_H
/// Avoids the usage of the Expression class/CreateLeaf struct provided by default by PETE
#define PETE_USER_DEFINED_EXPRESSION
#include "PETE/PETE.h"

/// Tag to request the stringization of an expression
struct StringizeExpr { };

/// Common ancestor to all the expression templates
class BaseExpression {
  
  /// Stores expressions which are not part of another expression (yet), i.e., top-level Expressions
  /**  Whenever an Expression is built, a pointer to it is pushed back in this vector.
      When the Expression is used to conform another larger Expression (e.g. it is one
      of the two operands in a binary operator), it is removed from this vector.
   
       The purpose of the vector is to keep the top-level Expressions built by the user
      so that they are printed to a string in the HPL::Codifier when liveCheckPrint() is invokated.
    */
  static std::vector<BaseExpression *> LiveExpressions_;
  
  /** @internal LiveExpressions_ has to be pushed here before a call evaluation builds a function
      and retrieved later because if call(f)(arg1, arg2, ...) the args are built before the call
      invocation, so if they are Expressions, they are already in LiveExpressions_ */ 
  static std::vector<std::vector<BaseExpression *> > HistoricLiveExpressions_;
  
protected:
  
  /// Sends strings of the Expressions #LiveExpressions_ to the global HPL::Codifier if in code generating mode
  /** Clears #LiveExpressions_ */
  static void liveCheckPrint();
  
public:
  
  /// Constructor, responsible from pushing the new Expression in #LiveExpressions_
  BaseExpression();
  
  /// Removes the Expression from #LiveExpressions_
  /** The Expression must be removed if it is part of some unary/binary operation, 
      or may be an index of an array. Therefore this function verifies that this
      Expression is the last or last but one in #LiveExpressions_
   */
  void remove() const;
  
  /// Simply clears the vector #LiveExpressions_ of live Expressions
  static void clearPendingExpressions();
  
  /// Removes Expression from #LiveExpressions_
  static void removeLast();
  
  /// Saves current LiveExpressions_ with arguments for a call to be retrieved after the functions construction
  static void pushLiveExpressions();
  
  /// Retrieves last saved LiveExpressions_
  static void popLiveExpressions();
  
  /// Get a string representation of the expression
  virtual std::string string() const  = 0;
  
  /// Virtual destructor
  virtual ~BaseExpression() { }
  
};


/// Wraps the contents of an expression in PETE expression templates
/** Thanks to this class we do not need to define operators for all the tree types.
    @tparam T Type encoding the expression
 */
template<class T>
class Expression : public BaseExpression
{
public:
  
  /// Type of the expression.
  typedef T Expression_t;
  
  /// Construct from an expression.
  Expression(const T& expr) : expr_m(expr)
  {
    //printf("Adding <<%s>>\n", string().c_str());
  }
  
  /// Accessor that returns the expression.
  const Expression_t& expression() const
  {
    return expr_m;
  }
  
  /// Get a string representation of the expression
  std::string string() const {
    return forEach(expr_m, StringizeExpr(), OpCombine());
  }
  

  ~Expression() {
    //printf("Removing <<%s>>\n", string().c_str());
    liveCheckPrint();
  }
  
private:
  /// Store the expression by value, since it is a temporary produced by operator functions.
  T expr_m;
};

/// Builds a leaf for a PETE expression template from another Expression
/** The leaf is simply built by removing the Expression template around \p T.
    @tparam T Type encoding the expression
*/
template<class T>
struct CreateLeaf<Expression<T> >
{
  typedef typename Expression<T>::Expression_t Leaf_t;
  
  inline static
  const Leaf_t &make(const Expression<T> &a)
  {
    a.remove();
    return a.expression();
  }
};


template<typename T1>
struct ArgBehavior {
  static void remove(const T1& v) { }
};

template<typename T1>
struct ArgBehavior<Expression<T1> > {
  static void remove(const Expression<T1>& v) { BaseExpression::removeLast(); }
};

//-----------------------------------------------------------------------------
// PETE support functions
//-----------------------------------------------------------------------------

#undef max
#include "AddtlIndexedArrayOperators.h"
#include "IndexedArrayOperators.h"

namespace HPL { class InternalKernelCaller; }

//-----------------------------------------------------------------------------
// We need to specialize CreateLeaf<T> for our class, so that operators
// know what to stick in the leaves of the expression tree.
//-----------------------------------------------------------------------------

/// Builds a leaf for a PETE expression template from an IndexedArray
template<typename T, int NDIM>
struct CreateLeaf<HPL::IndexedArray<T, NDIM> >
{
  typedef Reference<HPL::IndexedArray<T, NDIM> > Leaf_t;
  inline static
  Leaf_t make(const HPL::IndexedArray<T, NDIM> &a) { return Leaf_t(a); }
};

/// Builds a leaf for a PETE expression template from an Array<T, 0>
/** @tparam T   data type of the Array */
template<typename T>
struct CreateLeaf<HPL::Array<T, 0> >
{
  typedef Reference<HPL::Array<T, 0> > Leaf_t;
  inline static
  Leaf_t make(const HPL::Array<T, 0> &a) { return Leaf_t(a); }
};

template<>
struct CreateLeaf<HPL::InternalKernelCaller>
{
  typedef Reference<HPL::InternalKernelCaller> Leaf_t;
  inline static
  Leaf_t make(const HPL::InternalKernelCaller &a) { return Leaf_t(a); }
};


//-----------------------------------------------------------------------------
// Specialization of LeafFunctor class for applying the StringizeExpr
// tag to the object we can find in the tree. 
//-----------------------------------------------------------------------------

/// Processes an IndexedArray leaf in a PETE expression template to return a string representing it
template<typename T, int NDIM>
struct LeafFunctor<HPL::IndexedArray<T, NDIM>, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(const HPL::IndexedArray<T, NDIM> &v, const StringizeExpr &)
  { return v.string(); }
};

/// Processes an Array<T, 0> leaf in a PETE expression template to return a string representing it
/** @tparam T   data type of the Array */
template<typename T>
struct LeafFunctor<HPL::Array<T, 0>, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(const HPL::Array<T, 0> &v, const StringizeExpr &)
  { return v.string(); }
};




/// Processes an int leaf in a PETE expression template to return a string representing it
template<>
struct LeafFunctor<int, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(int a, const StringizeExpr &)
  {
    return HPL::stringize(a);
  }
};

/// Processes an unsigned int leaf in a PETE expression template to return a string representing it
template<>
struct LeafFunctor<unsigned int, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(unsigned int a, const StringizeExpr &)
  {
    return HPL::stringize(a);
  }
};

#ifdef HPL_64BIT_MACHINE
/// Processes a size_t leaf in a PETE expression template to return a string representing it
template<>
struct LeafFunctor<std::size_t, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(std::size_t a, const StringizeExpr &)
  {
    return HPL::stringize(a);
  }
};
#endif

/// Processes a float leaf in a PETE expression template to return a string representing it
template<>
struct LeafFunctor<float, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(float a, const StringizeExpr &)
  {
    return HPL::stringize(a);
  }
};

/// Processes a double leaf in a PETE expression template to return a string representing it
template<>
struct LeafFunctor<double, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(double a, const StringizeExpr &)
  {
    return HPL::stringize(a);
  }
};

/// Processes a InternalVector<T,N> leaf in a PETE expression template to return a string representing it
/** @tparam T   data type of the InternalVector in the leaf 
 @tparam N   length of the InternalVector in the leaf
 */
template<typename T, int N>
struct LeafFunctor<HPL::InternalVector<T, N>, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(const HPL::InternalVector<T, N> &a, const StringizeExpr &s)
  { return HPL::stringize(a); }
};

/// Processes a Scalar<T> leaf in a PETE expression template to return a string representing it
/** @tparam T   data type of the Scalar in the leaf */
template<class T>
struct LeafFunctor<Scalar<T>, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(const Scalar<T> &a, const StringizeExpr &s)
  { return LeafFunctor<T, StringizeExpr>::apply(a.value(), s); }
};

/// Processes a leaf of unknown type \c T in a PETE expression template to return a string representing it
/** It simply returns "Unknown type". May be it could throw an exception or some other behavior.
    @tparam T   data type of the PETE expression template leaf 
  */
template<class T>
struct LeafFunctor<T, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(const T &a, const StringizeExpr &)
  { return  HPL::String_t("Unknown type"); }
};


#endif
