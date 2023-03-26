#include <catch2/catch_test_macros.hpp>

#include "../src/Matrix/Container.h"

TEST_CASE(" Testin correctness of det() function" , "[det]")
{
    REQUIRE(Matrix::Matrix<int>::Eye(5).det() == 1);
    REQUIRE(Matrix::Matrix<int>::Zeros(5).det() == 0);
    REQUIRE(Matrix::Matrix<int>({{1, 2, 3},
                                 {4, 5, 6},
                                 {7, 8, 9}}).det() == 0);
    REQUIRE(Matrix::Matrix<int>({{1, 2, 3},
                                 {4, 8, 6},
                                 {7, 8, 9}}).det() == -36);
    REQUIRE(Matrix::Matrix<int>({{1, 2, 3},
                                 {4, 8, 12},
                                 {7, 8, 9}}).det() == 0);
    REQUIRE(Matrix::Matrix<int>({{1, -2, 3},
                                 {-4, 5, -6},
                                 {7, -8, 9}}).det() == 0);
    REQUIRE(Matrix::Matrix<int>({{1, -2, 3},
                                 {0, 0, -6},
                                 {0, 0, 1}}).det() == 0);
    REQUIRE(Matrix::Matrix<int>({{2, 1, 0, -2},
                                 {1, 2, 1, 2},
                                 {3, 4, 2, 3},
                                 {-2, -3, 1, -2}}).det() == -7);
    REQUIRE(Matrix::Matrix<int>({{8, 7, 2, 0},
                                 {8, 7, 2, 1},
                                 {4, 4, 4, 5},
                                 {0, 4, -3, 2}}).det() == -108);
    REQUIRE(Matrix::Matrix<int>({{1, 2, 3, 4},
                                 {2, 3, 4, 1},
                                 {3, 4, 1, 2},
                                 {4, 1, 2, 3}}).det() == 160);
}
