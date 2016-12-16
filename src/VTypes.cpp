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
/// \file     VTypes.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include "VTypes.h"
#include <cstdio>

namespace {
  
  int print_typename(char *p, const char *type_string, int n) {
    
    char * const porix = p;
    
    *p++='(';
    
    while (*type_string) {
      *p++ = *type_string++;
    }
    
    sprintf(p,"%d)(", n);
    while (*p) {
      p++;
    }
    
    return static_cast<int>(p - porix);
  }
  
}

namespace HPL {
  
  const String_t& VectorType::getFieldText(int i) {
    static String_t v[] = {
      ".s0", ".s1", ".s2", ".s3", ".s4", ".s5", ".s6", ".s7",
      ".s8", ".s9", ".sa", ".sb", ".sc", ".sd", ".se", ".sf"
    };
    return v[i];
  }

  String_t VectorType::string(const int* v_, int n) {
    char tmp[128];
    int pos = print_typename(tmp, "int", n); 
    while(n) {
      n--;
      pos += sprintf(tmp+pos, "%d,", *v_++);
    }
    tmp[pos-1]=')';
    tmp[pos] = 0;
    return String_t(tmp);
  }
  
  String_t VectorType::string(const float* v_, int n) {
    char tmp[128];
    int pos = print_typename(tmp, "float", n); 
    while(n) {
      n--;
      pos += sprintf(tmp+pos, "%ff,", *v_++);
    }
    tmp[pos-1]=')';
    tmp[pos] = 0;
    return String_t(tmp);
  }
 
  String_t VectorType::string(const double* v_, int n) {
    char tmp[128];
    int pos = print_typename(tmp, "double", n); 
    while(n) {
      n--;
      pos += sprintf(tmp+pos, "%25.15E,", *v_++);
    }
    tmp[pos-1]=')';
    tmp[pos] = 0;
    return String_t(tmp);
  }
}
