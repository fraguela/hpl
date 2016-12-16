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
/// \file     AddtlIndexedArrayOperators.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

/** @file
    @brief Expression template operators developed by hand because they were not (properly) supported by PETE
*/

#ifndef POSTFIXINDEXEDARRAYOPS_H
#define POSTFIXINDEXEDARRAYOPS_H

struct OpPrefixPlusPlus
{
  PETE_EMPTY_CONSTRUCTORS(OpPrefixPlusPlus)
  template<class T>
  inline typename UnaryReturn<T, OpPrefixPlusPlus >::Type_t
  operator()(const T &a) const
  {
    return "(++" + a + ')';
  }
};

struct OpPrefixMinusMinus
{
  PETE_EMPTY_CONSTRUCTORS(OpPrefixMinusMinus)
  template<class T>
  inline typename UnaryReturn<T, OpPrefixMinusMinus >::Type_t
  operator()(const T &a) const
  {
    return "(--" + a + ')';
  }
};

struct OpPostfixPlusPlus
{
  PETE_EMPTY_CONSTRUCTORS(OpPostfixPlusPlus)
  template<class T>
  inline typename UnaryReturn<T, OpPostfixPlusPlus >::Type_t
  operator()(const T &a) const
  {
    return '(' + a + "++)";
  }
};

struct OpPostfixMinusMinus
{
  PETE_EMPTY_CONSTRUCTORS(OpPostfixMinusMinus)
  template<class T>
  inline typename UnaryReturn<T, OpPostfixMinusMinus >::Type_t
  operator()(const T &a) const
  {
    return '(' + a + "--)";
  }
};

struct OpAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a +  '=' + b + ')';
  }
};


template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpAssign > {
  typedef HPL::String_t Type_t;
};

/* 
  PETE generated the operators below mostly right from this specification in StringOps.in,
  but the return type was &Type_t instead of Type_t for the string :
 
 UnaryAssignOps
 -----
  TAG = "OpPrefixPlusPlus"
  FUNCTION = "operator++"
  EXPR = "return \"(++\" + a + ')';"
-----
  TAG = "OpPrefixMinusMinus"
  FUNCTION = "operator--"
  EXPR = "return \"(--\" + a + ')';"
-----
  TAG = "OpPostPlusPlus"
  FUNCTION = "operator++"
  EXPR = "return '(' + a + \"++)\";"
-----
  TAG = "OpPostMinusMinus"
  FUNCTION = "operator--"
  EXPR = "return '(' + a + \"--)\";"

 binaryAssignOps
 -----
 TAG = "OpAddAssign"
 FUNCTION = "operator+="
 EXPR = "return '(' + a + '+' + '=' + b + ')';"
 -----
 TAG = "OpSubtractAssign"
 FUNCTION = "operator-="
 EXPR = "return '(' + a + '-' + '=' + b + ')';"
 -----
 TAG = "OpMultiplyAssign"
 FUNCTION = "operator*="
 EXPR = "return '(' + a + '*' + '=' + b + ')';"
 -----
 TAG = "OpDivideAssign"
 FUNCTION = "operator/="
 EXPR = "return '(' + a + '/' + '=' + b + ')';"
 -----
 TAG = "OpModAssign"
 FUNCTION = "operator%="
 EXPR = "return '(' + a + '%' + '=' + b + ')';"
 -----
 TAG = "OpBitwiseOrAssign"
 FUNCTION = "operator|="
 EXPR = "return '(' + a + '|' + '=' + b + ')';"
 -----
 TAG = "OpBitwiseAndAssign"
 FUNCTION = "operator&="
 EXPR = "return '(' + a + '&' + '=' + b + ')';"
 -----
 TAG = "OpBitwiseXorAssign"
 FUNCTION = "operator^="
 EXPR = "return '(' + a + '^' + '=' + b + ')';"
 -----
 TAG = "OpLeftShiftAssign"
 FUNCTION = "operator<<="
 EXPR = "return '(' + a + '<' + '<' + '=' + b + ')';"
 -----
 TAG = "OpRightShiftAssign"
 FUNCTION = "operator>>="
 EXPR = "return '(' + a + '>' + '>' + '=' + b + ')';"
 
 
 */

struct OpAddAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpAddAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpAddAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '+' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpAddAssign > {
  typedef T1 Type_t;
};

struct OpSubtractAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpSubtractAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpSubtractAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '-' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpSubtractAssign > {
  typedef T1 Type_t;
};

struct OpMultiplyAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpMultiplyAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpMultiplyAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '*' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpMultiplyAssign > {
  typedef T1 Type_t;
};

struct OpDivideAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpDivideAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpDivideAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '/' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpDivideAssign > {
  typedef T1 Type_t;
};

struct OpModAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpModAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpModAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '%' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpModAssign > {
  typedef T1 Type_t;
};

struct OpBitwiseOrAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpBitwiseOrAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpBitwiseOrAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '|' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpBitwiseOrAssign > {
  typedef T1 Type_t;
};

struct OpBitwiseAndAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpBitwiseAndAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpBitwiseAndAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '&' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpBitwiseAndAssign > {
  typedef T1 Type_t;
};

struct OpBitwiseXorAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpBitwiseXorAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpBitwiseXorAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '^' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpBitwiseXorAssign > {
  typedef T1 Type_t;
};

struct OpLeftShiftAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpLeftShiftAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpLeftShiftAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '<' + '<' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpLeftShiftAssign > {
  typedef T1 Type_t;
};

struct OpRightShiftAssign
{
  PETE_EMPTY_CONSTRUCTORS(OpRightShiftAssign)
  template<class T1, class T2>
  inline typename BinaryReturn<T1, T2, OpRightShiftAssign >::Type_t
  operator()(const T1 &a, const T2 &b) const
  {
    return '(' + a + '>' + '>' + '=' + b + ')';
  }
};

template<class T1, class T2 >
struct BinaryReturn<T1, T2, OpRightShiftAssign > {
  typedef T1 Type_t;
};

/* OpPostfixPlusPlus*/
/*
template<class T1, int D1>
inline typename MakeReturn<UnaryNode<OpPostfixPlusPlus,
typename CreateLeaf<HPL::IndexedArray<T1,D1> >::Leaf_t> >::Expression_t
operator++(const HPL::IndexedArray<T1,D1> & l, int)
{
  typedef UnaryNode<OpPostfixPlusPlus,
  typename CreateLeaf<HPL::IndexedArray<T1,D1> >::Leaf_t> Tree_t;
  return MakeReturn<Tree_t>::make(Tree_t(
    CreateLeaf<HPL::IndexedArray<T1,D1> >::make(l)));
}

template<class T1>
inline typename MakeReturn<UnaryNode<OpPostfixPlusPlus,
typename CreateLeaf<HPL::Array<T1, 0> >::Leaf_t> >::Expression_t
operator++(const HPL::Array<T1, 0> & l, int)
{
  typedef UnaryNode<OpPostfixPlusPlus,
  typename CreateLeaf<HPL::Array<T1, 0> >::Leaf_t> Tree_t;
  return MakeReturn<Tree_t>::make(Tree_t(
    CreateLeaf<HPL::Array<T1, 0> >::make(l)));
}

template<class T1>
inline typename MakeReturn<UnaryNode<OpPostfixPlusPlus,
typename CreateLeaf<Expression<T1> >::Leaf_t> >::Expression_t
operator++(const Expression<T1> & l, int)
{
  typedef UnaryNode<OpPostfixPlusPlus,
  typename CreateLeaf<Expression<T1> >::Leaf_t> Tree_t;
  return MakeReturn<Tree_t>::make(Tree_t(
    CreateLeaf<Expression<T1> >::make(l)));
}
*/
/* OpPostfixMinusMinus */
/*
template<class T1, int D1>
inline typename MakeReturn<UnaryNode<OpPostfixMinusMinus,
typename CreateLeaf<HPL::IndexedArray<T1,D1> >::Leaf_t> >::Expression_t
operator--(const HPL::IndexedArray<T1,D1> & l, int)
{
  typedef UnaryNode<OpPostfixMinusMinus,
  typename CreateLeaf<HPL::IndexedArray<T1,D1> >::Leaf_t> Tree_t;
  return MakeReturn<Tree_t>::make(Tree_t(
    CreateLeaf<HPL::IndexedArray<T1,D1> >::make(l)));
}

template<class T1>
inline typename MakeReturn<UnaryNode<OpPostfixMinusMinus,
typename CreateLeaf<HPL::Array<T1, 0> >::Leaf_t> >::Expression_t
operator--(const HPL::Array<T1, 0> & l, int)
{
  typedef UnaryNode<OpPostfixMinusMinus,
  typename CreateLeaf<HPL::Array<T1, 0> >::Leaf_t> Tree_t;
  return MakeReturn<Tree_t>::make(Tree_t(
    CreateLeaf<HPL::Array<T1, 0> >::make(l)));
}

template<class T1>
inline typename MakeReturn<UnaryNode<OpPostfixMinusMinus,
typename CreateLeaf<Expression<T1> >::Leaf_t> >::Expression_t
operator--(const Expression<T1> & l, int)
{
  typedef UnaryNode<OpPostfixPlusPlus,
  typename CreateLeaf<Expression<T1> >::Leaf_t> Tree_t;
  return MakeReturn<Tree_t>::make(Tree_t(
    CreateLeaf<Expression<T1> >::make(l)));
}
*/
#endif
