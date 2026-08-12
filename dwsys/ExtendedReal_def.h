/* ExtendedReal_def.h
 *
 * Copyright (C) 2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
	An ExtendedReal has two parts (real,extension)
	The number in the extension part represents: power (epsilon, power (2, extension)),
	where epsilon is a number larger than zero but smaller than any real number r.
	r.epsilon < epsilon^2.
*/

#define ooSTRUCT ExtendedReal
oo_DEFINE_STRUCT (ExtendedReal)

	oo_DOUBLE (real)
	oo_INTEGER (extension)

	#if oo_DECLARING

		constexpr void operator+= (const structExtendedReal& other) {
			real += other.real;
			extension = std::max (extension, other.extension);
		}
		
		constexpr void operator-= (const structExtendedReal& other) {
			this -> real -= other.real;
			this -> extension = this -> extension <= other.extension ? this -> extension : - other.extension;
		}
		
		friend inline bool operator<  (const structExtendedReal& lhs, const structExtendedReal& rhs) {
			return std::tie (lhs.real, rhs.extension) < std::tie (rhs.real, lhs.extension);
		}
		
		friend inline bool operator>  (const structExtendedReal& lhs, const structExtendedReal& rhs) {
			return rhs < lhs;
		}
		friend inline bool operator<= (const structExtendedReal& lhs, const structExtendedReal& rhs) {
			return ! (lhs > rhs);
		}
		friend inline bool operator>= (const structExtendedReal& lhs, const structExtendedReal& rhs) {
			return ! (lhs < rhs);
		}
		friend inline bool operator== (const structExtendedReal& lhs, const structExtendedReal& rhs) {
			return std::tie (lhs.real, lhs.extension) == std::tie (rhs.real, rhs.extension);
		}
		friend inline bool operator!= (const structExtendedReal& lhs, const structExtendedReal& rhs) {
			return ! (lhs == rhs);
		}
	#endif

oo_END_STRUCT (ExtendedReal)
#undef ooSTRUCT

/* End of file ExtendedReal_def.h */
