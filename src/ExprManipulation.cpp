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
/// \file     ExprManipulation.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cassert>
#include "GlobalState.h"
#include "ExprManipulation.h"

std::vector<BaseExpression *> BaseExpression::LiveExpressions_;

std::vector<std::vector<BaseExpression *> > BaseExpression::HistoricLiveExpressions_;


BaseExpression::BaseExpression()
{
  LiveExpressions_.push_back(this);
}

void BaseExpression::remove() const
{
  const size_t i = LiveExpressions_.size();
  
  assert(i);
  
  BaseExpression *b = LiveExpressions_.back();
  if(b != this) {
    assert(i > 1);
    BaseExpression *& b2 = LiveExpressions_[i-2];
    assert(b2 == this);
    b2 = b;
  }
  
  LiveExpressions_.pop_back();
}

void BaseExpression::liveCheckPrint() {
  const size_t n = LiveExpressions_.size();
  for(size_t i = 0; i < n; i++) {
    LOG(LiveExpressions_[i]->string() << ";\n");
    if(!HPL_STATE(HPL::NotBuilding)) {
      //std::cerr << " Issuing and clearing " << LiveExpressions_[i]->string() << std::endl;
      HPL::TheGlobalState().getCodifier().add(LiveExpressions_[i]->string(), true);
    }
  }
  LiveExpressions_.clear();
}

void BaseExpression::removeLast()
{
  assert(LiveExpressions_.size());
  LiveExpressions_.pop_back();
}

void BaseExpression::clearPendingExpressions()
{
  LiveExpressions_.clear();
}

void BaseExpression::pushLiveExpressions()
{
  HistoricLiveExpressions_.push_back(LiveExpressions_);
  LiveExpressions_.clear();
}

void BaseExpression::popLiveExpressions()
{
  assert(!LiveExpressions_.size());
  LiveExpressions_.swap(HistoricLiveExpressions_.back());
  HistoricLiveExpressions_.pop_back();
}
