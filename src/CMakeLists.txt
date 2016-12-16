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

# \file     src/CMakeLists.txt
# \author   Moisés Viñas        <moises.vinas@udc.es>
# \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
# \author   Basilio B. Fraguela <basilio.fraguela@udc.es>


cmake_minimum_required( VERSION 2.8 )

set(HPL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(PETE_DIR ${PROJECT_SOURCE_DIR}/lib/pete-2.1.0)
set(PETE_LINK ${HPL_SOURCE_DIR}/PETE)
set(MakeOps ${PETE_DIR}/src/Tools/MakeOperators)

add_custom_command(OUTPUT ${MakeOps}
                   COMMAND make --no-print-directory -C ${PETE_DIR})

#used to be a link: COMMAND ln -s ${PETE_DIR}/src/PETE ${PETE_LINK}
add_custom_command(OUTPUT ${PETE_LINK}
                   COMMAND cmake -E copy_directory ${PETE_DIR}/src/PETE ${PETE_LINK})

add_custom_command(OUTPUT ${HPL_SOURCE_DIR}/IndexedArray_T_NDIM_extraOps.h  ${HPL_SOURCE_DIR}/Array_T_extraOps.h
				   COMMAND generate_extraOps
				   DEPENDS generate_extraOps
				   WORKING_DIRECTORY ${HPL_SOURCE_DIR})

add_custom_command(OUTPUT ${HPL_SOURCE_DIR}/IndexedArrayOperators.h
                   COMMAND ${MakeOps} --classes IndexedArrayDefs.in --operators StringOps.in --guard INDEXEDARRAYOPS_H --op-tags > IndexedArrayOperators.h
                   DEPENDS ${MakeOps} IndexedArrayDefs.in StringOps.in
                   WORKING_DIRECTORY ${HPL_SOURCE_DIR})

set_source_files_properties( ${HPL_SOURCE_DIR}/IndexedArray_T_NDIM_extraOps.h
 			     			 ${HPL_SOURCE_DIR}/Array_T_extraOps.h
 			     			 ${HPL_SOURCE_DIR}/IndexedArrayOperators.h
                             PROPERTIES GENERATED TRUE)

add_executable( generate_extraOps generate_extraOps.cpp )

add_custom_target( initial_HPL_files DEPENDS ${PETE_LINK} ${HPL_SOURCE_DIR}/IndexedArray_T_NDIM_extraOps.h  ${HPL_SOURCE_DIR}/Array_T_extraOps.h ${HPL_SOURCE_DIR}/IndexedArrayOperators.h )

include_directories( ${HPL_SOURCE_DIR} )

# HPL library
set( HPL_SOURCES HPL.cpp Array.cpp ExprManipulation.cpp Stringizer.cpp GlobalState.cpp Codifier.cpp FRunner.cpp FHandle.cpp HPLHelpers.cpp FCall.cpp CLbinding.cpp Env.cpp Device.cpp VTypes.cpp exceptions/HPLException.cpp HDM.cpp HPL_utils.cpp )

if( CLBLAS_DIR )
	include_directories( ${CLBLAS_DIR}/include )
	set( HPL_SOURCES ${HPL_SOURCES} HPL_clBLAS.cpp exceptions/HPL_clBLAS_exceptions.cpp )
endif( CLBLAS_DIR )

add_library( HPL STATIC ${HPL_SOURCES} )
add_dependencies( HPL initial_HPL_files )

add_library( HPLprofile STATIC ${HPL_SOURCES} )
add_dependencies( HPLprofile initial_HPL_files )
target_compile_definitions( HPLprofile PUBLIC -DHPL_PROFILE )

# PerfAdapt

add_executable( PerfAdapt PerfAdapt.cpp )
target_link_libraries( PerfAdapt LINK_PUBLIC HPL ${OpenCL_LIBRARIES} )
# Remove HPL_config.txt as default target
#add_custom_command( TARGET PerfAdapt POST_BUILD COMMAND PerfAdapt > ${HPL_SOURCE_DIR}/HPL_config.txt )

#install

install( TARGETS HPL HPLprofile PerfAdapt
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib)

install( DIRECTORY ./
         DESTINATION include/hpl
         FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" )
