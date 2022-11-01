#ifndef ALGUTIL_UPPER_TRIAGULAR_INDEXING_HPP
#define ALGUTIL_UPPER_TRIAGULAR_INDEXING_HPP

#include <cstddef>
#include "utility.hpp"

namespace algutil {
	/**
	 * Methods for indexing an upper triangular matrix without diagonal stored in a continuous vector.
	 */
	namespace upper_no_diagonal {
		/* 
		 * xx 00 01 02 03
		 * xx xx 04 05 06
		 * xx xx xx 07 08
		 * xx xx xx xx 09
		 * xx xx xx xx xx
		 */

		/* returns the index of a row, i.e. such that array[row_index(N,i) + j] for i < j refers to the pair (i,j) */
		static NDEBUG_CONSTEXPR inline std::size_t row_index(std::size_t n, std::size_t i) noexcept {
			UTIL_ASSERT(i < n)
			return (n * i) - 2*i - 1 - ((i-1)*i) / 2;
		}

		/* returns the index of an entry assuming that i < j */
		static NDEBUG_CONSTEXPR inline std::size_t index(std::size_t n, std::size_t i, std::size_t j) noexcept {
			UTIL_ASSERT(i < j)
			return row_index(n, i) + j;
		}

		/* returns the index of an entry; slower than index(n,i,j) */
		static NDEBUG_CONSTEXPR inline std::size_t index_unordered(std::size_t n, std::size_t i, std::size_t j) noexcept {
			return i < j ? index(n,i,j) : index(n,j,i);
		}

		/* return the number of elements */
		static constexpr inline std::size_t num_pairs(std::size_t n) {
			return (n * (n - 1)) / 2;
		}

		/* call the callable for every pair i < j with the index for each element */
		template<typename Callable> inline void for_each(Callable&& c, std::size_t n) {
			std::size_t ind = 0;
			for(std::size_t i = 0; i < n-1; ++i) {
				for(std::size_t j = i + 1; j < n; ++j, ++ind) {
					c(i, j, ind);
				}
			}
		}
	}

	/**
	 * Methods for indexing an upper triangular matrix with a diagonal stored in a continuous vector.
	 */
	namespace upper_diagonal {
		/* 
		 * 00 01 02 03 04
		 * xx 05 06 07 08
		 * xx xx 09 10 11
		 * xx xx xx 12 13
		 * xx xx xx xx 14
		 */
		
		/* returns the index of a row, i.e. such that array[row_index(N,i) + j] for i <= j refers to the pair (i,j) */
		static NDEBUG_CONSTEXPR inline std::size_t row_index(std::size_t n, std::size_t i) noexcept {
			UTIL_ASSERT(i < n)
			return n*i - (i*(i - 1)) / 2 - i;
		}

		/* returns the index of an entry assuming that i <= j */
		static NDEBUG_CONSTEXPR inline std::size_t index(std::size_t n, std::size_t i, std::size_t j) noexcept {
			UTIL_ASSERT(i <= j)
			return row_index(n, i) + j;
		}

		/* returns the index of an entry; slower than index(n,i,j) */
		static NDEBUG_CONSTEXPR inline std::size_t index_unordered(std::size_t n, std::size_t i, std::size_t j) noexcept {
			return i < j ? index(n, i, j) : index(n, j, i);
		}

		/* return the number of elements */
		static constexpr inline std::size_t num_pairs(std::size_t n) {
			return (n * (n + 1)) / 2;
		}

		/* call the callable for every pair i < j with the index for each element */
		template<typename Callable> inline void for_each(Callable&& c, std::size_t n) {
			std::size_t ind = 0;
			for(std::size_t i = 0; i < n; ++i) {
				for(std::size_t j = i; j < n; ++j, ++ind) {
					c(i, j, ind);
				}
			}
		}
	}
}

#endif //ALGUTIL_UPPER_TRIAGULAR_INDEXING_HPP
