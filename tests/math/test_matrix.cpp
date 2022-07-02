#include <catch2/catch_all.hpp>
#include <s3dg/math/matrix.hpp>

using namespace s3dg::math;

TEST_CASE("matrix init", "[matrix]") {
    SECTION("clean") {
        Matrix<5, 5, int> m;

        for (size_t i = 0; i < 5; ++i) {
            for (size_t j = 0; j < 5; ++j) {
                REQUIRE(m.get(i, j) == 0);
            }
        }
    }
    SECTION("get") {
        /*
        1 2
        3 4
        */
        Matrix<2, 2, int> m({1, 2, 3, 4});

        for (size_t i = 0; i < 2; ++i) {
            for (size_t j = 0; j < 2; ++j) {
                REQUIRE((m.get(i, j) == i * 2 + j + 1));
            }
        }
    }
    SECTION("plus") {
        /*
        1 2
        3 4

        +

        1 2
        3 4
        */
        Matrix<2, 2, int> m({1, 2, 3, 4});
        auto m2 = m;
        auto m3 = m + m2;

        for (size_t i = 0; i < 2; ++i) {
            for (size_t j = 0; j < 2; ++j) {
                REQUIRE(m3.get(i, j) == (i * 2 + j + 1) * 2);
            }
        }
    }
    SECTION("equal") {
        /*
        1 2
        3 4
        */
        Matrix<2, 2, int> m({1, 2, 3, 4});
        auto m2 = m;
        REQUIRE(m == m2);
    }
    SECTION("mul") {
        /*
        1 2
        3 4

        *

        1 2
        3 4

        ==

        7 10
        15 22
        */
        Matrix<2, 2, int> m({1, 2, 3, 4});
        auto m2 = m;
        Matrix<2, 2, int> m3({7, 10, 15, 22});
        auto m4 = m * m2;
        REQUIRE(m3 == m4);
    }
    SECTION("mul 2") {
        /*
        1 2 3
        4 5 6

        *

        7 8
        9 10
        11 12

        ==

        7 10
        15 22
        */
        Matrix<2, 3, int> m({1, 2, 3, 4, 5, 6});
        Matrix<3, 2, int> m2({7, 8, 9, 10, 11, 12});
        Matrix<2, 2, int> m3({58, 64, 139, 154});
        auto m4 = m * m2;
        REQUIRE(m4 == m3);
    }
}
