//
// Created by Phillip Keldenich on 08.06.17.
//

#ifndef ALGUTIL_TEST_UPPER_TRIANGULAR_INDEXING_HPP
#define ALGUTIL_TEST_UPPER_TRIANGULAR_INDEXING_HPP

#include "../upper_triagular_indexing.hpp"

/** Test cases for indexing. */
namespace algutil {
	namespace test {
		ALGUTIL_TESTCASE("upper_no_diagonal_foreach", {
			static constexpr std::size_t TEST_N = 10;
			algutil::upper_no_diagonal::for_each(
				[] (std::size_t i, std::size_t j, std::size_t ind) -> void {
					ALGUTIL_TEST_EQUAL(algutil::upper_no_diagonal::index(TEST_N, i, j), ind);
				}, TEST_N
			);
		})

		ALGUTIL_TESTCASE("upper_diagonal_foreach", {
			static constexpr std::size_t TEST_N = 10;
			algutil::upper_diagonal::for_each(
				[](std::size_t i, std::size_t j, std::size_t ind) -> void {
					ALGUTIL_TEST_EQUAL(algutil::upper_diagonal::index(TEST_N, i, j), ind);
				}, TEST_N
			);
		})
	}
}

#endif //ALGUTIL_UPPER_TRIANGULAR_INDEXING_HPP
