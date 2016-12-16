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
/// \file     Tuple.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef TUPLE_H_
#define TUPLE_H_

namespace HPL {

class Tuple {
public:

	constexpr Tuple() noexcept :
		origin(0), end(0) {}

	constexpr Tuple(int origin) noexcept :
			origin(origin), end(origin){}

	constexpr Tuple(int origin, int end) noexcept :
		origin(origin), end(end) {}

	constexpr int getSize() const noexcept
	{
		return ((this->end+1) - this->origin);
	}

	int origin, end;
};
	typedef Tuple Range;
}

#endif /* TUPLE_H_ */
