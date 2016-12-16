# HPL : Heterogeneous Programming Library
# Copyright (c) 2012-2016 The HPL Team as listed in CREDITS.txt
#
# This file is part of HPL
#
# HPL is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# HPL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with HPL.  If not, see <http://www.gnu.org/licenses/>.

# \file     CMakeLists.txt
# \author   Moisés Viñas        <moises.vinas@udc.es>
# \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
# \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
 
cmake_minimum_required( VERSION 2.8 )

project( HPL )

find_package( OpenCL REQUIRED MODULE )
 
#include_directories( ${OpenCL_INCLUDE_DIRS} )
#link_libraries( ${OpenCL_LIBRARIES} )
 
#if( OPENCL_HAS_CPP_BINDINGS )
#    message( "OpenCL has CPP bindings. Full include is: " ${OPENCL_INCLUDE_DIRS} )
#else( OPENCL_HAS_CPP_BINDINGS )
#    message( "No OpenCL CPP bindings found" )
#endif( OPENCL_HAS_CPP_BINDINGS )
 
#message( "OpenCL Found : " ${OpenCL_FOUND})
#message( "InclDir  : " ${OpenCL_INCLUDE_DIR})
#message( "InclDirs : " ${OpenCL_INCLUDE_DIRS})
#message( "Library  : " ${OpenCL_LIBRARY})
#message( "Libraries: " ${OpenCL_LIBRARIES})
#message( "Version  : " ${OpenCL_VERSION_STRING})
#message( "V        : " ${OpenCL_VERSION_MAJOR} ${OpenCL_VERSION_MINOR})
#message( "CMAKE_BINARY_DIR : " ${CMAKE_BINARY_DIR} )
#message( "CMAKE_SOURCE_DIR : " ${CMAKE_SOURCE_DIR} )

set(CLBLAS_DIR "" CACHE PATH "clBLAS home directory")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

#set( CMAKE_CXX_STANDARD          11 )
#set( CMAKE_CXX_STANDARD_REQUIRED ON )

add_subdirectory( src )
add_subdirectory( tests )
