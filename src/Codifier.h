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
/// \file     Codifier.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef CODIFIER_H
#define CODIFIER_H

#include "HPL_utils.h"

namespace HPL {

  /// Defines the state of the codifier, and thus the library
  enum State_t { NotBuilding,     ///< In host code, not in a function to be compiled
                 BuildingFHeader, ///< Building the header of a function
                 BuildingFBody    ///< Building the body of a function
               };
  
  /// Stores code as it is being built
  /** It is a singleton kept in the GlobalState. The cycle of usage of a Codifier is like this:
      -# first it is built in ::NotBuilding state. In this state, the calls to the library 
         are made in host code, that is, not in functions to the built by the library.
      -# when the user wants to run a parallel function \c f, he invokes ::eval 
	 on \c f. This checks whether the function has already been compiled for parallel form
         or not. If it has not, the Codifier is set to ::BuildingFHeader by 
        GlobalState::FHandle::fillIn()
      -# In ::BuildingFHeader state variables for the inputs to the function are created. During 
         this creation, they will send appropiate strings for their declaration in the function
         header to the Codifier.
      -# Once the inputs have been created, GlobalState::FHandle::fillIn() sets the
         Codifier state to ::BuildingFBody and invokes the function \c f with the arguments created.
         In this state the expression templates and control macros send the appropriate string to
         the Codifier to encode the body of the function
      -# When the function \c f finishes, GlobalState::FHandle::fillIn() invokes
         GlobalState::FHandle::endDefinition(), which stores the code generated in the
         GlobalState::FHandle and resets the Codifier by invoking its endDefinition() method.
   */
  class Codifier {

    static const char *Meaning[18];
    
  public:
    
    static const char *PreVars[18];
    
    Codifier();
    
    /// Adds new code
    /** @param[in]  s        code to append
        @param[in]  newline  whether preceding indentation and an ending new line must be added.
                             If \p s does not finish in a bracket, it also appends a semicolon before the new line character
       */
    void add(const String_t& s, bool newline = false);

    /// Push \c if statement with condition \p s in the code and begins code block
    void if_(const String_t& s);

    /// Push \c else if statement with condition \p s in the code and begins code block
    void else_if_(const String_t& s);


    /// Push \c while statement with condition \p s in the code and begins code block
    void while_(const String_t& s);
    
    /// Push \c for statement with initialization \p init, condition \p cond and update \p update and and begins code block
    void for_(const String_t& init, const String_t& cond, const String_t& update);
    
    /// Return the value of the function
    void return_(const String_t& s);
    
    /// Push an \c else in the code
    void else_();
    
    /// Finishes a block of code
    void endif_() { endBlock(); }
    
    /// Allows to remove the last \p n characters in the code stored
    void removeLastChars(String_t::size_type n);
    
    /// Resets the code and nesting level, but does not modify the state
    void clear();
    
    /// Increases the nesting level
    void beginBlock();
    
    /// Closes the block with a bracket and decreases the nesting level
    void endBlock();
    
    ///Push and \c break in the code
    void break_();

    /// Retrieve the current code stored
    const String_t& getCode() const { return code_; }
    
    /// Retrieve the current state of the Codifier
    State_t state() const { return state_; }
    
     /// Set the state of the Codifier
    /** Some changes also modify the code (e.g. parenthesis or brackets are automatically added) */
    void state(State_t s);
    
    /// Indicates the end of the construction of a function, returns its code and resets the Codifier (including the code)
    String_t endDefinition();
    
    /// Store the value for nScalarsByValue_
    void setnScalarsByValue(unsigned short int i) { nScalarsByValue_ = i; }
    
    /// Checks whether there are not yet copied input scalars passed by value
    /** The check is only made by Array(const Array<T, 0>& another), so it implies a new
        scalar argument is being passed by value, and as a result the number is decreased */
    bool pendingScalarsByValue() { return nScalarsByValue_ ? (nScalarsByValue_--) : false; }
    
  protected:
    
    void automaticProcess();

    int nestLevel_; ///< current nestling level in the generated code
    
    std::size_t bodyBegin_; ///< point of the \p code_ where the body of the function begins
    
    unsigned short int nScalarsByValue_; ///< Number of Array<T,0> passed by value to the function
    
    String_t code_; ///< code currently stored in the Codifier
    
    State_t state_; ///< Codifier state
  };
  
}

#endif
