#pragma once
#include <s3dg/math/matrix.hpp>

namespace s3dg {
    namespace math {

        template<size_t size, typename T>
        class Vec: public Matrix<1, size, T> {
          public:
            inline T get(const size_t& i) const {
                return get(0, i);
            }
            inline T operator [](const size_t& index) const {
                return get(index);
            }
        };


        template<typename T>
        using Vec2 = Vec<2, T>;

        template<typename T>
        using Vec3 = Vec<3, T>;

        template<typename T>
        using Vec4 = Vec<4, T>;

        typedef Vec2<float> Vec2f;
        typedef Vec3<float> Vec3f;
        typedef Vec4<float> Vec4f;

        typedef Vec2<unsigned char> Vec2uc;
        typedef Vec3<unsigned char> Vec3uc;
        typedef Vec4<unsigned char> Vec4uc;

        typedef Vec2<unsigned int> Vec2ui;
        typedef Vec3<unsigned int> Vec3ui;
        typedef Vec4<unsigned int> Vec4ui;
    }
}
