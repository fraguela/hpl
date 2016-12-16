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
/// \file     generate_extraOps.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

/** @file
 *  Creates the custom methods to perform assignments to Array<T, 0>
 * and IndexedArray<T, NDIM> in kernel codes. These methods are customized for two reasons:
 * -# Besides building the corresponding string, they invoke methods to
 * gather information on whether these variables are written or not, and whether
 * the first operation on them is a write or a read operations.
 * -# As explained in file AddtlIndexedArrayOperators.h, the return type for these methods
 *  in PETE was &Type_t instead of Type_t for the string
 */

class IOManager {
  ofstream stream_;	///< File where the code will be stored
  bool isOpen_;		///< True if it is already open. False otherwise
  
public:
  
  /**
   * @brief Default constructor. Initializes the flag isOpen_ to false.
   */
  IOManager()
  : isOpen_(false)
  {}
  
  /**
   * @brief Open the file where will be the management expressions code.
   * @param[in] name File name.
   */
  void open(const char *name) {
    
    if(isOpen_)
      close();
    
    stream_.open(name);
    
    if(stream_.is_open() == false ) {
      cerr << "Unable to open file " << name << " for writing\n";
      exit(EXIT_FAILURE);
    }
    
    isOpen_ = true;
  }
  
  /**
   * @brief Close the file.
   */
  void close() { 
    if(isOpen_)
      stream_.close();
    isOpen_ = false;
  }
  
  /**
   * @brief Operator that tries to write a string in the file stream_.
   * @param[in] s string that will be written.
   */
  IOManager& operator<<(const string& s) {
    if(isOpen_)
      stream_ << s << endl;
    else {
      cerr << "Attempt to write to closed IOManager\n";
      exit(EXIT_FAILURE);
    }
    return *this;
  }
  
};

struct ltstr
{
  bool operator()(const string& s1, const string& s2) const
  {
    return s1.compare(s2) < 0;
  }
};


///Maps from the tag of the operator to the name of the C++ operator
map<string, string, ltstr> Tag2OpMap;
map<string, string, ltstr> TagUOpMap;

/// Operate with T2-dependent thing. Args: LEFT,RIGHT,OPERATOR,TAG
const string WITH_T2 = 
"template<class T2>                                              \n \
typename MakeReturn<BinaryNode<TAG,                              \n \
typename CreateLeaf<HPL::LEFT >::Leaf_t,                     \n \
typename CreateLeaf<RIGHT >::Leaf_t> >::Expression_t             \n \
OPERATOR(const RIGHT & rhs)                                      \n \
{                                                                \n \
  typedef BinaryNode<TAG,                                        \n \
  typename CreateLeaf<HPL::LEFT >::Leaf_t,                   \n \
  typename CreateLeaf<RIGHT >::Leaf_t> Tree_t;                   \n \
  if(this->bArr_!=NULL) TheGlobalState().getArrayInfo(this->bArr_)->set_isReadandWritten();     \n \
  return MakeReturn<Tree_t>::make(Tree_t(                        \n \
              CreateLeaf<HPL::LEFT >::make(*this),           \n \
              CreateLeaf<RIGHT >::make(rhs)));                   \n \
}\n";

// Operate with <T2,NDIM2>-dependent thing. Args: LEFT,RIGHT,OPERATOR,TAG
const string WITH_T2_NDIM2 = 
"template<class T2, int NDIM2>                                   \n \
typename MakeReturn<BinaryNode<TAG,                              \n \
typename CreateLeaf<HPL::LEFT >::Leaf_t,                     \n \
typename CreateLeaf<HPL::RIGHT >::Leaf_t> >::Expression_t    \n \
OPERATOR(const HPL::RIGHT & rhs)                             \n \
{                                                                \n \
  typedef BinaryNode<TAG,                                        \n \
  typename CreateLeaf<HPL::LEFT >::Leaf_t,                   \n \
  typename CreateLeaf<HPL::RIGHT >::Leaf_t> Tree_t;          \n \
  if(this->bArr_!=NULL) TheGlobalState().getArrayInfo(this->bArr_)->set_isReadandWritten();     \n \
  return MakeReturn<Tree_t>::make(Tree_t(                        \n \
              CreateLeaf<HPL::LEFT >::make(*this),           \n \
              CreateLeaf<HPL::RIGHT >::make(rhs)));          \n \
}\n";

/// Operate with itself. Args: LEFT,OPERATOR,TAG
const string WITH_ITSELF =
"typename MakeReturn<BinaryNode<TAG,                             \n \
typename CreateLeaf<HPL::LEFT >::Leaf_t,                     \n \
typename CreateLeaf<HPL::LEFT >::Leaf_t> >::Expression_t     \n \
OPERATOR(const HPL::LEFT & rhs)                              \n \
{                                                                \n \
  typedef BinaryNode<TAG,                                        \n \
  typename CreateLeaf<HPL::LEFT >::Leaf_t,                   \n \
  typename CreateLeaf<HPL::LEFT >::Leaf_t> Tree_t;           \n \
  if(this->bArr_!=NULL) TheGlobalState().getArrayInfo(this->bArr_)->set_isReadandWritten();     \n \
  return MakeReturn<Tree_t>::make(Tree_t(                        \n \
	      CreateLeaf<HPL::LEFT >::make(*this),           \n \
	      CreateLeaf<HPL::LEFT >::make(rhs)));           \n \
}\n";

const string PREFIXPOSTFIX =
"inline typename MakeReturn<UnaryNode<TAG, \n \
  typename CreateLeaf<HPL::LEFT >::Leaf_t> >::Expression_t \n \
OPERATOR(FIX) \n \
{ \n \
  typedef UnaryNode<TAG, \n \
    typename CreateLeaf<HPL::LEFT >::Leaf_t> Tree_t; \n \
	if(this->bArr_!=NULL) TheGlobalState().getArrayInfo(this->bArr_)->set_isReadandWritten();     \n \
	return MakeReturn<Tree_t>::make(Tree_t( \n \
    CreateLeaf<HPL::LEFT >::make(*this))); \n \
}\n";

const string LEFT("LEFT"), RIGHT("RIGHT"), OP("OPERATOR"), TAG("TAG"), FIX("FIX");

const string 
  OpAssign("OpAssign"), 
  OpAddAssign("OpAddAssign"),
  OpSubtractAssign("OpSubtractAssign"),
  OpMultiplyAssign("OpMultiplyAssign"),
  OpDivideAssign("OpDivideAssign"),
  OpModAssign("OpModAssign"),
  OpBitwiseOrAssign("OpBitwiseOrAssign"),
  OpBitwiseAndAssign("OpBitwiseAndAssign"),
  OpBitwiseXorAssign("OpBitwiseXorAssign"),
  OpLeftShiftAssign("OpLeftShiftAssign"),
  OpRightShiftAssign("OpRightShiftAssign"),
  OpPrefixPlusPlus("OpPrefixPlusPlus"),
  OpPrefixMinusMinus("OpPrefixMinusMinus"),
  OpPostfixPlusPlus("OpPostfixPlusPlus"),
  OpPostfixMinusMinus("OpPostfixMinusMinus");

/// Array with all the tags of the operators to overload
const string Ops [] = {
  OpAssign,           OpAddAssign,       OpSubtractAssign,   OpMultiplyAssign,
  OpDivideAssign,     OpModAssign,       OpBitwiseOrAssign, OpBitwiseAndAssign,
  OpBitwiseXorAssign, OpLeftShiftAssign, OpRightShiftAssign};

const string unaryOps [] = {
		OpPrefixPlusPlus, OpPrefixMinusMinus, OpPostfixPlusPlus, OpPostfixMinusMinus
};

const size_t num_Ops =  sizeof(Ops) / sizeof(Ops[0]);
//const size_t num_UnaryOps = sizeof(unaryOps) / sizeof(unaryOps[0]);

const string IndexedArray_T_NDIM("IndexedArray<T, NDIM>");
const string IndexedArray_T2_NDIM2("IndexedArray<T2, NDIM2>");
const string Array_T("Array<T, 0>");
const string HPL_Array_T2("HPL::Array<T2, 0>");
const string Expression_T2("Expression<T2>");
const string T2("T2");
const string POSTFIX("int");
const string PREFIX("");

IOManager ProgramIOManager;

void error_on_pattern(const string& pattern)
{
  cerr << "Requested pattern:\n\n" << pattern;
  cerr << "Wrong pattern or number of arguments for pattern\n";
  exit(EXIT_FAILURE);
}

/// when it is a scalar: 
//    - removes invokations to record the read/write state of the destination of the assignment
//    - makes actual assignment for T2, Array<T,0 > and Array<T2, 0> right operators for operator=
/// Also, when the tag is OpAssign, it changes set_isReadandWritten to set_isWrite
void scalarize(string& s, const string& left, const string& tag, const string& right)
{
  
  //If the destination is a scalar, do not record the write
  if(left.compare(IndexedArray_T_NDIM)) {
    
    if(left.compare(Array_T)) {
      cerr << "LEFT is neither " << IndexedArray_T_NDIM << " nor " << Array_T << endl;
      exit(EXIT_FAILURE);
    }
    
    size_t w   = s.find("if(this->bArr_!=NULL)", 0);
    size_t end = s.find(';', w);

    if(right.compare(Expression_T2) && right.compare(IndexedArray_T2_NDIM2)) {
      string assignment(Tag2OpMap[tag].c_str() + 8);
      s.replace(w, end - w, "if(HPL_STATE(NotBuilding)) v_" + assignment + (right.compare(T2) ? "(T)rhs.value()" :  "(T)rhs"));
    } else {
      s.replace(w, end - w, "assert(HPL_STATE(BuildingFBody))");
    }
    
  } else if(!tag.compare(OpAssign)) {
    s.erase(s.find("Readand", 0), 7); //In operator= the destination is not read before written
  }

}

/// Replaces all occurrences of string \c before by \c after in the string \c s
void replace(string& s, const string& before, const string& after)
{ 
  const size_t size_before = before.size();
  size_t w = 0;
  
  while( (w = s.find(before, w)) != string::npos) {
    s.replace(w, size_before, after);
  }
}

/// generate \c WITH_T2 and \c WITH_T2_NDIM2 based methods
void generate(const string& pattern, const string& left, const string& right, const string& tag) 
{ string s(pattern);
  
  if(pattern.compare(WITH_T2) && pattern.compare(WITH_T2_NDIM2))
    error_on_pattern(pattern);
  
  scalarize(s, left, tag, right);
  
  replace(s, LEFT, left);
  replace(s, RIGHT, right);
  replace(s, OP, Tag2OpMap[tag]);
  replace(s, TAG, tag);
  
  ProgramIOManager << s;
}

/// generate \c WITH_ITSELF based methods
void generate(const string& pattern, const string& left, const string& tag) 
{ string s(pattern);
  
  if(pattern.compare(WITH_ITSELF))
    error_on_pattern(pattern);
  
  scalarize(s, left, tag, HPL_Array_T2);
  
  replace(s, LEFT, left);
  replace(s, OP, Tag2OpMap[tag]);
  replace(s, TAG, tag);
  
  ProgramIOManager << s;
}

void scalarizeU(string& s, const string& left, const string& tag, const string& right)
{
  if(left.compare(IndexedArray_T_NDIM)) {

    size_t w   = s.find("if(this->bArr_!=NULL)", 0);
    size_t end = s.find(';', w);

    string assignment(TagUOpMap[tag].c_str() + 8);
    s.replace(w, end - w, "if(HPL_STATE(NotBuilding)) v_" + assignment);

  }
}

void generateU(const string& pattern, const string& left, const string& tag, const string& fix)
{ string s(pattern);

  scalarizeU(s, left, tag, HPL_Array_T2);

  replace(s, LEFT, left);
  replace(s, OP, TagUOpMap[tag]);
  replace(s, TAG, tag);
  replace(s, FIX, fix);

  ProgramIOManager << s;
}

int main()
{ size_t i;

  Tag2OpMap[OpAssign]           = "operator=";
  Tag2OpMap[OpAddAssign]        = "operator+=";
  Tag2OpMap[OpSubtractAssign]   = "operator-=";
  Tag2OpMap[OpMultiplyAssign]   = "operator*=";
  Tag2OpMap[OpDivideAssign]     = "operator/=";
  Tag2OpMap[OpModAssign]        = "operator%=";
  Tag2OpMap[OpBitwiseOrAssign]  = "operator|=";
  Tag2OpMap[OpBitwiseAndAssign] = "operator&=";
  Tag2OpMap[OpBitwiseXorAssign] = "operator^=";
  Tag2OpMap[OpLeftShiftAssign]  = "operator<<=";
  Tag2OpMap[OpRightShiftAssign] = "operator>>=";
  TagUOpMap[OpPrefixPlusPlus]	= "operator++";
  TagUOpMap[OpPostfixPlusPlus]	= "operator++";
  TagUOpMap[OpPrefixMinusMinus]	= "operator--";
  TagUOpMap[OpPostfixMinusMinus]= "operator--";
  
  ProgramIOManager.open("IndexedArray_T_NDIM_extraOps.h");
  
  for(i = 0; i < num_Ops; i++) {
    /// Assign scalar variable
    generate(WITH_T2,       IndexedArray_T_NDIM, HPL_Array_T2,          Ops[i]);
  
    /// Assign IndexedArray with different T2 or NDIMS
    generate(WITH_T2_NDIM2, IndexedArray_T_NDIM, IndexedArray_T2_NDIM2, Ops[i]);
  
    /// Assign IndexedArray with same T and NDIM
    generate(WITH_ITSELF,   IndexedArray_T_NDIM,                        Ops[i]);
  
    /// Assign general PETE expression
    generate(WITH_T2,       IndexedArray_T_NDIM, Expression_T2,         Ops[i]);
  
    /// Assign any other thing
    generate(WITH_T2,       IndexedArray_T_NDIM, T2,                    Ops[i]);
  }

  // Generate ++ and -- pre and postfix por (indexed array) IndexedArray<T,NDIM>
  generateU(PREFIXPOSTFIX, IndexedArray_T_NDIM, unaryOps[0],PREFIX);
  generateU(PREFIXPOSTFIX, IndexedArray_T_NDIM, unaryOps[1],PREFIX);
  generateU(PREFIXPOSTFIX, IndexedArray_T_NDIM, unaryOps[2],POSTFIX);
  generateU(PREFIXPOSTFIX, IndexedArray_T_NDIM, unaryOps[3],POSTFIX);

  ///////////////////////////////
  
  ProgramIOManager.open("Array_T_extraOps.h");
  
  for(i = 0; i < num_Ops; i++) {
    //Special assignment Array<T,0> v = IndexedArray<T2, NDIM2> w that
    //avoids the v_ = (T)other.value() assigment problem
    generate(WITH_T2_NDIM2, Array_T, IndexedArray_T2_NDIM2, Ops[i]);
    
    /// Assign scalar variable of a different type T2
    generate(WITH_T2,       Array_T, HPL_Array_T2,  Ops[i]);
  
    /// Assign scalar variable of the same type T
    generate(WITH_ITSELF,   Array_T,                Ops[i]);
  
    /// Assign general PETE expression
    generate(WITH_T2,       Array_T, Expression_T2, Ops[i]);
  
    /// Assign any other thing
    generate(WITH_T2,       Array_T, T2,            Ops[i]);
  }
  
  // Generate ++ and -- pre and postfix por (scalar) Array<T,0>
  generateU(PREFIXPOSTFIX, Array_T, unaryOps[0], PREFIX);
  generateU(PREFIXPOSTFIX, Array_T, unaryOps[1], PREFIX);
  generateU(PREFIXPOSTFIX, Array_T, unaryOps[2], POSTFIX);
  generateU(PREFIXPOSTFIX, Array_T, unaryOps[3], POSTFIX);

  ProgramIOManager.close();
}
