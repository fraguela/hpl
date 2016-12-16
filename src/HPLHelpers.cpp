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
/// \file     HPLHelpers.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///


#include "HPL.h"

namespace {
  
#ifdef HPL_WINDOWS
  char * stpcpy(char * to, const char * from)
  {
    for (; (*to = *from); ++from, ++to);
    return(to);
  }
#endif

  /// Returns the rounded UP log2 of \p v
  std::size_t ilog2(std::size_t v)
  { std::size_t result;
    
    assert(v);
    
    result = 0;
    v--;
    while (v) {
      result++;
      v = v >> 1;
    }
    return result;
  }
  
  /// Returns the rounded DOWN log2 of \p v
  std::size_t ilog2down(std::size_t v)
  {
    std::size_t result = ilog2(v);
    return result - ((1 << result) > v); 
  }

  bool ispow2(std::size_t v)
  {
    return v == (1 << ilog2(v));
  }
  
  char *str_replace_from(const char *orig, const char *rep, const char *with, int from) {
    char *result; // the return string
    const char *ins;    // the next insert point
    char *tmp;
    size_t len_rep;   // length of rep
    size_t len_with;  // length of with
    size_t len_front; // distance between rep and end of last rep
    int count;       // number of replacements
    
    if (!orig)
      return NULL;
    if (!rep || !(len_rep = strlen(rep)))
      return NULL;
    if (!(ins = strstr(orig + from, rep))) 
      return NULL;
    if (!with)
      with = "";
    len_with = strlen(with);
    
    for (count = 0; (tmp = (char *)strstr(ins, rep)) != NULL; ++count) {
      ins = tmp + len_rep;
    }
    
    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = (char *)malloc(strlen(orig) + (len_with - len_rep) * count + 1);
    
    if (!result)
      return NULL;
    
    tmp = strncpy(tmp, orig, from) + from;
    orig += from;
    
    while (count--) {
      ins = strstr(orig, rep);
      len_front = ins - orig;
      tmp = strncpy(tmp, orig, len_front) + len_front;
      tmp = strcpy(tmp, with) + len_with;
      orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
  }

  /// @internal Helper class for reductions
  struct Reducer {
    char reduc_template[2][64], reduc[64];
    const std::size_t nelems_;
    
    Reducer(const char *op, std::size_t nelems);
    
    char *adaptto(const char *lhs_index, const char *rhs_expr, bool external);
  };
  
  Reducer::Reducer(const char *op, std::size_t nelems)
  : nelems_(nelems)
  {
    bool is_short_op = (*op == '+') || (*op == '*');
    if(nelems) {
      if(is_short_op) {
	sprintf(reduc_template[0], "tmp_[%%s][j_]%c%%s[j_]", *op);
	sprintf(reduc_template[1], "tmp_[%%s][j_]%c(%%s)", *op);
      } else {
	sprintf(reduc_template[0], "%s(tmp_[%%s][j_],%%s[j_])", op);
	sprintf(reduc_template[1], "%s(tmp_[%%s][j_],%%s);", op);
      }
    } else {
      if(is_short_op) {
	sprintf(reduc_template[0], "tmp_[%%s]%c%%s", *op);
      } else {
	sprintf(reduc_template[0], "%s(tmp_[%%s],%%s)", op);
      }
    }
    
  }
  
  char *Reducer::adaptto(const char *lhs_index, const char *rhs_expr, bool external) {
    int i = nelems_ && external;
    sprintf(reduc, reduc_template[i], lhs_index, rhs_expr);
    return reduc;
  }
  
}


namespace HPL {
  
  void reduce::default_initialization() {
    grpSz_ = 0;
    ndims_ = 3; 
    localMem_ = 0;
    nTeams_ = 0;
    nelems_ = 0;
    syncReq_ = 2; // when >=2 threads involved => barrier required
    isMinGrpSz_ = false;
    inBinaryTree_ = false;
    toAll_ = false;
    unroll_ = true;
  }
  
  void reduce::dobarrier() {
    pos = static_cast<int>(stpcpy(buf + pos, "  barrier(CLK_LOCAL_MEM_FENCE);\n") - buf);
  }
  
  void reduce::docopy(const char *indexdest, const char *value) {
    if(nelems_) {
      pos += sprintf(buf + pos, "  for(int j_=0; j_<%zu; ++j_) tmp_[%s][j_]=%s", nelems_, indexdest, value);
    } else {
      pos += sprintf(buf + pos, "  tmp_[%s]=%s", indexdest, value);
    }
    buf[pos++] = ';';
    buf[pos++] = '\n';
  }
  
  void reduce::dolocalarraydef(const char * name, std::size_t nrows, const char *source) {
    
    if (source) {
      pos += sprintf(buf + pos, "  volatile __local %s ", typeName_);
      if(nelems_)
	pos += sprintf(buf + pos, "(* const %s)", name);
      else
	pos += sprintf(buf + pos, "* const %s", name);
    } else {
      pos += sprintf(buf + pos, "  __local %s %s[%zu]", typeName_, name, nrows);
    }
    
    if(nelems_) {
      pos += sprintf(buf + pos, "[%zu]", nelems_);
    }
    
    if (source) {
      pos += sprintf(buf + pos, " = %s", source);
    }
    
    buf[pos++] = ';';
    buf[pos++] = '\n';
  }
  
  reduce::~reduce()
  { static const char * Tid_String = "tid_", * Tsz_String = "tsz_";
    char exprptr[128];
    //const char * const exprptr = expression_.c_str();
    const char * lidxid = Tid_String;
    const char * szid = Tsz_String;
    const char * tmpptr, * reduc;
    std::size_t maxsize = 0;
    int volatile_from = 0;

    pos = 0;

    if(nelems_ < 1) {
      nelems_ = 0;
      strcpy(exprptr, expression_.c_str());
    } else
      sprintf(exprptr, "*(&(%s)+j_)", expression_.c_str());
    
#ifdef DEBUG
    pos = sprintf(buf, "/* code for op=%s ndims=%zu grpSz=%zu nelems=%zu, isMin=%u localMem=%zu syncReq=%zu binaryTree=%u*/\n", 
		  op_, ndims_, grpSz_, nelems_, isMinGrpSz_, localMem_, syncReq_, inBinaryTree_);
#endif
    
    buf[pos++] = '{';
      
    if(!isMinGrpSz_ && ((grpSz_ == 1) || (grpSz_ && (grpSz_ == nTeams_)))) {
      tmpptr = exprptr;
      toAll_ = true;
    } else {
      
      // Initialize tid_ and tsz_ 
      switch (ndims_) {
	case 1: 
	  lidxid = lidx.name().c_str();
	  szid = lszx.name().c_str();
	  break;
	case 2:
	  pos += sprintf(buf + pos, " const size_t tid_ = %s +  get_local_size(0) * %s;\n", lidx.name().c_str(), lidy.name().c_str());
	  if (isMinGrpSz_ || !grpSz_)
	    pos += sprintf(buf + pos, "  const size_t tsz_ = get_local_size(0) * get_local_size(1);\n");
	  break;
	case 3:
	  pos += sprintf(buf + pos, " const size_t tid_ = %s +  get_local_size(0) * (%s + get_local_size(1) * %s);\n", lidx.name().c_str(), lidy.name().c_str(), lidz.name().c_str());
	  if (isMinGrpSz_ || !grpSz_)
	    pos += sprintf(buf + pos, "  const size_t tsz_ = get_local_size(0) * get_local_size(1) * get_local_size(2);\n");
	  break;
	default: 
	  assert(0);
      }
      
      //adjustment for teams of threads within a work-group
      if(nTeams_) {
	if (nTeams_ == 1) {
	  nTeams_ = 0;
	} else {
	  assert(!(grpSz_ % nTeams_));
	  if(!grpSz_ || isMinGrpSz_ || (grpSz_ == nTeams_)) { // if there is a team size, it is the minimum group size
	    grpSz_ = nTeams_;
	    isMinGrpSz_ = true;
	    localMem_ = nTeams_; //dismiss user requirement : set one item per team
	  } else { //requires grpSz_ fixed and multiple of nTeams_ . Forcing powers of 2
	    assert(ispow2(nTeams_));
	    localMem_ = 0; //dismiss user requirements for localMem. Use maximum parallelism
	  }
	}
      }
      
      //Main body

      Reducer reducer(op_, nelems_);
      
      tmpptr = "tmp_[1]";
      if(!isMinGrpSz_ && grpSz_ == 2) { //teams not supported in this branch (looks ridiculous)
	dolocalarraydef("tmp_", 2);
	docopy(lidxid, exprptr);
	dobarrier();
      } else {

	// Adjust localMem_ size
	if(grpSz_) {
	  maxsize = 1 << (ilog2(grpSz_) - 1); //maximum local memory required for maximal binary-tree usage (or minimum if it isMinGrpSz_)
	  if(!localMem_) localMem_ = maxsize;
	  else if (!isMinGrpSz_) { //added for dangling else issue
	    if (localMem_ > grpSz_)
	      localMem_ = grpSz_;
	    else if (localMem_ > maxsize)
	      localMem_ = maxsize;
    } //added for dangling else issue
	} else {
	  if(!localMem_) localMem_ = reduce::DEFAULT_LOCALMEM;
	}
	
	if(inBinaryTree_ && !nTeams_)
	  localMem_ = 1 << ilog2down(localMem_);
	
	if(nTeams_) { 
	  char team_offset[64], team_lid[64];
	  dolocalarraydef("tp_", localMem_);
	  if(isMinGrpSz_) { // grpSz_ = localMem_ = nTeams_
	    const std::size_t tmp = ilog2(nTeams_);
	    const bool ispw2 = (nTeams_ == (1 << tmp));
	    pos += sprintf(buf + pos,  "  const size_t lsz_ = %s %s %zu;\n", szid, ispw2 ? ">>" : "/", ispw2 ? tmp : nTeams_);
	    szid = "lsz_";
	    sprintf(team_lid, "%s%%lsz_;\n", lidxid);
	    localMem_ = maxsize = 1;
	    sprintf(team_offset, "%s/lsz_", lidxid);
	  } else {
	    szid = 0;
	    sprintf(team_lid, "%s&%zu;\n", lidxid, (grpSz_ / nTeams_) - 1);
	    localMem_ = maxsize = (grpSz_ / nTeams_) >> 1;
	    if(localMem_ == 1)
	      sprintf(team_offset, "%s>>1", lidxid);
	    else
	      sprintf(team_offset, "(%s>>1)&(~%zu)", lidxid, (localMem_ - 1)); 
	  }
	  pos += sprintf(buf + pos, "  const size_t lid_ = %s\n", team_lid);
	  
	  if(nelems_)
	    pos += sprintf(buf + pos, "  __local %s (* const tmp_)[%zu] = tp_ + (%s);\n", typeName_, nelems_, team_offset);
	  else
	    pos += sprintf(buf + pos, "  __local %s * const tmp_ = tp_ + (%s);\n", typeName_, team_offset);
	  
	  grpSz_ = grpSz_ / nTeams_;
	  lidxid = "lid_";
	} else
	  dolocalarraydef("tmp_", localMem_);
	
	if(!isMinGrpSz_ && grpSz_ && (localMem_ >= maxsize)) {
	  
	  if(localMem_ == maxsize) {
	    char temp[32];
	    std::size_t tmp = grpSz_ - maxsize;
	    pos += sprintf(buf + pos, "  if(%s >= %zu)", lidxid, tmp);
	    sprintf(temp, "%s - %zu", lidxid, tmp);
	    docopy(temp, exprptr);
	    dobarrier();
	  }
	  else //for sure localMem_ == grpSz_
	    docopy(lidxid, exprptr); //no ending barrier

	} else {
	  char temp[16], temp2[16];
	  sprintf(temp, "%s-i_", lidxid);
	  reduc = reducer.adaptto(temp, exprptr, true);
	  pos += sprintf(buf + pos, "  if((%s >= %zu) && (%s < %zu))", lidxid, localMem_, lidxid, 2 * localMem_);
	  sprintf(temp2, "%s - %zu", lidxid, localMem_);
	  docopy(temp2, exprptr);
	  dobarrier();
	  if (isMinGrpSz_ || !grpSz_)
	    pos += sprintf(buf + pos, "  for(size_t i_ = %zu; i_ < %s; i_+=%zu) {\n", 2 * localMem_, szid, localMem_);
	  else 
	    pos += sprintf(buf + pos, "  for(size_t i_ = %zu; i_ < %zu; i_+=%zu) {\n", 2 * localMem_, grpSz_, localMem_);
	  pos += sprintf(buf + pos, "    if((i_ <= %s) && (%s < (i_ + %zu)))", lidxid, lidxid, localMem_);
	  docopy(temp, reduc);
	  dobarrier();
	  pos += sprintf(buf + pos, " }\n");
	}

	// Sets volatile_from if there is any chance a barrier may be removed
	unroll_ = unroll_ || (localMem_ <= 4);
	if ( inBinaryTree_ && (localMem_ > 1) && ( (localMem_ < syncReq_) || (unroll_ && (syncReq_ > 4)) ) ) {
	  dolocalarraydef("ptr_", 0, "tmp_");
	  volatile_from = pos;
	}
	
	if(isMinGrpSz_ || !grpSz_ || localMem_ <= maxsize) {
	  reduc = reducer.adaptto(lidxid, exprptr, true);
	  if(!isMinGrpSz_ && (localMem_ == maxsize)) { //implies grpSz_ != 0
	    pos += sprintf(buf + pos, "  if(%s < %zu)", lidxid, grpSz_ - maxsize);
	    docopy(lidxid, reduc);
	  } else {
	    
	    if(nelems_) {
	      pos += sprintf(buf + pos, "  if(%s < %zu)\n    for(int j_=0; j_<%zu; ++j_)\n      tmp_[%s][j_]=", lidxid, localMem_, nelems_, lidxid);
	    } else{
	      pos += sprintf(buf + pos, "  if(%s < %zu)\n    tmp_[%s]=", lidxid, localMem_, lidxid);
	    }
	    
	    if ((2 * localMem_) > grpSz_ ) //includes !grpSz_
	      pos += sprintf(buf + pos, "((%s + %zu) >= %s) ? %s : ",  lidxid, localMem_, szid, exprptr);
	    
	    pos += sprintf(buf + pos, "%s;\n",  reduc);
	    
	  }
	}

	//final reduction within the temporary array tmp_
	if (inBinaryTree_) {

	  if(localMem_ > 2) {
	    char temp[32];
	    
	    if(unroll_ ) { // UNROLLED BINARY TREE REDUCTION
	      char dif[4];
	      for (std::size_t lm_i =  localMem_ >> 1; lm_i >1; lm_i >>= 1) {
		sprintf(dif, "%zu", lm_i);
		if( (lm_i * 2) >= syncReq_ )
		  dobarrier();
		sprintf(temp, "tmp_[%s + %s]", lidxid, dif);
		reduc = reducer.adaptto(lidxid, temp, false);
		temp[0]=0;
		if(localMem_ > grpSz_) //includes !grpSz_
		  sprintf(temp, "&&((%s+%s)<%s)", lidxid, dif, szid);
		pos += sprintf(buf + pos, "    if((%s < %s)%s) ", lidxid, dif, temp);
		docopy(lidxid, reduc);
	      }
	    } else {  // NON-UNROLLED BINARY TREE REDUCTION
	      /// @todo barriers are now a none or all thing: either they are put in the loop or not.
	      ///        another possitibility would be to make a loop with barriers, while the number of
	      ///        threads involved is >= syncReq_; and another loop without barriers when fewer
	      ///        thrads work in the reduction
	      const bool put_barriers = (localMem_ >= syncReq_);
	      pos += sprintf(buf + pos, "  for(size_t i_ = %zu; i_ > 1 ; i_>>=1) {\n", localMem_ >> 1);
	      if(put_barriers)
		dobarrier();
	      sprintf(temp, "tmp_[%s + i_]", lidxid);
	      reduc = reducer.adaptto(lidxid, temp, false);
	      temp[0]=0;
	      if(localMem_ > grpSz_) //includes !grpSz_
		sprintf(temp, "&&((%s+i_)<%s)", lidxid, szid);
	      pos += sprintf(buf + pos, "    if((%s < i_)%s) ", lidxid, temp);
	      docopy(lidxid, reduc);
	      strcpy(buf + pos, "  }\n");
	      pos += 4;
	    }
	    
	  }
	  
	  if(toAll_) {
	    //if( (grpSz_ >= syncReq_) || !grpSz_ ) // was needed for nTeams with reduction on a single element (sets grpSz_=1)
	    dobarrier();
	  } else {
	    if( (2 >= syncReq_) && ((grpSz_ > 2) || !grpSz_) )
	      dobarrier();
	    else 
	      if (!volatile_from) { //e.g. trigger with reduce(s, v, "+").ndims(1).groupSize(128).nElems(12).unroll(false).inTree().syncReq(33);
		dolocalarraydef("ptr_", 0, "tmp_"); 
		volatile_from = pos;
	      }
	  }
	  
	} else if ( !nTeams_ || (!isMinGrpSz_ && (maxsize>1)) ) {
	  
	  // The barrier is probably better than forcing a volatile pointer for just a single synchronization
	  //if(localMem_ >= syncReq_)
	  dobarrier();
	  
	  if (grpSz_ < localMem_) { //includes !grpSz_
	    maxsize = 0;
	    pos += sprintf(buf + pos, "  size_t i_ = ((%s > %zu) ? %zu : %s) - 1;\n", szid, localMem_, localMem_, szid);
	  } else
	    maxsize = localMem_ - 1;
	  
	  if (maxsize != 1) {
	    if(maxsize)
	      pos += sprintf(buf + pos, "  size_t i_ = %zu;\n", maxsize);
	
	    if(nelems_) {
	      reduc = reducer.adaptto("0", "tmp_[i_]", false);
	      pos += sprintf(buf + pos, "  if(!%s)\n", lidxid);
	      pos += sprintf(buf + pos, "    while(i_ > 1) {\n    ");
	      docopy("0", reduc);
	      pos += sprintf(buf + pos, "      i_--;\n    }\n");
	    } else {
	      pos += sprintf(buf + pos, "  %s x_=tmp_[i_];\n", typeName_);
	      tmpptr = "x_";
	      reduc = reducer.adaptto("i_", tmpptr, false);
	      pos += sprintf(buf + pos, "  while(--i_) { x_=%s; }\n", reduc);
	    }
	  }
	}
	
	/// no need of some barrier before assignment when toAll_ is true 
	/// because everyone runs the reduction

      }
      
      reduc = reducer.adaptto("0", tmpptr, false);
      
      
      if (nTeams_ && grpSz_ <= 2) {
	tmpptr = nelems_ ? "tmp_[0][j_]" : "tmp_[0]";
      } else
	if (grpSz_ < 2) {
	  sprintf(exprptr, "(%s==1)?tmp_[0]%s:(%s)", szid, nelems_ ? "[j_]" : "", reduc);
	  tmpptr = exprptr;
	} else
	  tmpptr = reduc;
      
    }
    
    if(!toAll_)
      pos += sprintf(buf + pos, "  if(!%s)", lidxid);
    
    if(nelems_) {
      sprintf(buf + pos, "\n    for(int j_=0; j_<%zu; ++j_)\n      *(&(%s)+j_)=%s;\n}\n", nelems_, dest_.c_str(), tmpptr);
    } else
      sprintf(buf + pos, "  %s=%s;\n}\n", dest_.c_str(), tmpptr);
    
    if (volatile_from) {
      char * tmp = str_replace_from(buf, "tmp_", "ptr_", volatile_from);
      strcpy(buf, tmp);
      free(tmp);
    }
    
    TheGlobalState().getCodifier().add(buf, false);
  }
  
  
}
