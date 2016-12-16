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
/// \file     Codifier.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cassert>
#include <cstdio>
#include "ExprManipulation.h"
#include "Codifier.h"

class BaseExpression;

namespace {
  
  /// Generates a String_t with \p n whitespaces
  HPL::String_t spaces(int n)
  { char tmp[n + 1];
    
    memset(tmp, ' ', n);
    tmp[n] = 0;
    
    return HPL::String_t(tmp);
  }

}

namespace HPL {
  
  const char *Codifier::PreVars[18] = 
   {0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  const char *Codifier::Meaning[18] = {
    "get_global_id(0)",
    "get_global_id(1)",
    "get_global_id(2)",
    "get_local_id(0)",
    "get_local_id(1)",
    "get_local_id(2)",
    "get_group_id(0)",
    "get_group_id(1)",
    "get_group_id(2)",
    "get_global_size(0)",
    "get_global_size(1)",
    "get_global_size(2)",
    "get_local_size(0)",
    "get_local_size(1)",
    "get_local_size(2)",
    "get_num_groups(0)",
    "get_num_groups(1)",
    "get_num_groups(2)"
  };
  
  Codifier::Codifier()
  : state_(NotBuilding)
  { 
    clear();
  }
  
  void Codifier::clear()
  {
    nestLevel_ = 0;
    bodyBegin_ = 0;
    nScalarsByValue_ = 0;
    code_ = "";
  }
  
  void Codifier::add (const String_t& s, bool newline)
  { static const String_t semicolon(";\n"), nosemicolon("\n");
    
    if(newline) {
      char c = s[s.length() - 1];
      /* BBF : I do not understand why this code (?).
      size_t pos = s.find("{");
      String_t str_end;

      if(pos != (s.length() - 1))
    	  str_end = ";\n"; //BBF: This branch leads to "if(){ }; else {};
      else
    	  str_end =  (c== '{' || c=='}') ? "\n" : ";\n";
       */
      const String_t& str_end = (c == '{' || c == '}') ? nosemicolon : semicolon;
      code_ += spaces(2 * nestLevel_) + s + str_end;
    } else
      code_ += s;
  }
  
  /*
  void Codifier::if_() {
    assert(state_ == BuildingFBody);
    add("if(");
    toNextBlock_ = 1;
  }
  
  void Codifier::while_() {
    assert(state_ == BuildingFBody);
    add("while(");
    toNextBlock_ = 1;
  }
  
  void Codifier::for_() {
    assert(state_ == BuildingFBody);
    add("for(");
    toNextBlock_ = 3;
  }
  */
  
  void Codifier::if_(const String_t& s) {
    assert(state_ == BuildingFBody);
    add("if(" + s + ") {", true);
    BaseExpression::clearPendingExpressions();
    beginBlock();
  }

  void Codifier::else_if_(const String_t& s) {
    assert(state_ == BuildingFBody);
    assert(nestLevel_);
    //nestLevel_--;
    add(" else if(" + s + ") {", true);
    BaseExpression::clearPendingExpressions();
    beginBlock();
  }
  
  void Codifier::while_(const String_t& s) {
    assert(state_ == BuildingFBody);
    add("while(" + s + ") {", true);
    BaseExpression::clearPendingExpressions();
    beginBlock();
  }
  
  void Codifier::for_(const String_t& init, const String_t& cond, const String_t& update) {
    assert(state_ == BuildingFBody);
    add("for(" + init + "; " + cond + "; " + update + ") {", true);
    BaseExpression::clearPendingExpressions();
    beginBlock();
  }

  void Codifier::return_(const String_t& s)
  {
    assert(state_ == BuildingFBody);
    add("return " + s, true);
    BaseExpression::clearPendingExpressions();
  }
  
  void Codifier::else_() {
    assert(state_ == BuildingFBody);
    assert(nestLevel_);
    //nestLevel_--;
    add(" else {", true);
    beginBlock();
  }
  
  void Codifier::beginBlock() {
    assert(state_ == BuildingFBody);
    nestLevel_++; 
  }
  
  void Codifier::endBlock() {
    assert(state_ == BuildingFBody);
    assert(nestLevel_);
    nestLevel_--; 
    add("}", true); 
  }
  
  void Codifier::break_()
  {
    assert(state_ == BuildingFBody);
    add("break", true);
    BaseExpression::clearPendingExpressions();
  }

  void Codifier::removeLastChars(String_t::size_type n)
  {
    const String_t::size_type s = code_.size();
    if (n > s) {
      n = s;
    }
    code_.erase(s - n, n);
  }
  
  void Codifier::state(State_t s)
  { 
    state_ = s;
    switch(state_) {
      case BuildingFHeader: 
	code_ += "("; 
	break;
      case BuildingFBody: 
	removeLastChars(2); 
	code_ += ") {\n";
	bodyBegin_ = code_.size();
	beginBlock(); 
	break;
      case NotBuilding: 
	break;
    }
  }
  
  void Codifier::automaticProcess()
  { char tmp[128];
    
    for(int i = 0; i < sizeof(PreVars)/sizeof(PreVars[0]); i++) {
      if(utils::findWord(code_, PreVars[i])) {
	sprintf(tmp, "  size_t %s=%s;\n", PreVars[i], Meaning[i]);
	code_.insert(bodyBegin_, tmp);
      }
    }
  }
  
  String_t Codifier::endDefinition()
  {
    automaticProcess();
    endBlock();
    state(NotBuilding);
    String_t ret = code_;
    clear();
    return ret;
  }
  
}
