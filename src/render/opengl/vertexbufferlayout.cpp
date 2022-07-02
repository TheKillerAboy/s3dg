#include <s3dg/render/opengl/vertexbufferlayout.h>
#include <s3dg/render/opengl/utils.h>

namespace s3dg {
    namespace render {
        namespace opengl {


            unsigned int VertexBufferElement::get_size_of_type(unsigned int type) {
                switch (type) {
                    case GL_FLOAT:
                        return 4;

                    case GL_UNSIGNED_INT:
                        return 4;

                    case GL_UNSIGNED_BYTE:
                        return 1;
                }

                assert(false);
                return 0;
            }

            void VertexBufferLayout::inc_stride(unsigned int count, unsigned int type) {
                stride += count * VertexBufferElement::get_size_of_type(type);
            }

            // template<typename T>
            // void VertexBufferLayout::push(unsigned int count) {
            //     static_assert(false);
            // }

            template<>
            void VertexBufferLayout::push<float>(unsigned int count) {
                elements.push_back({GL_FLOAT, count, false});
                inc_stride(count, GL_FLOAT);
            }

            template<>
            void VertexBufferLayout::push<unsigned int>(unsigned int count) {
                elements.push_back({GL_UNSIGNED_INT, count, false});
                inc_stride(count, GL_UNSIGNED_INT);
            }

            template<>
            void VertexBufferLayout::push<unsigned char>(unsigned int count) {
                elements.push_back({GL_UNSIGNED_BYTE, count, true});
                inc_stride(count, GL_UNSIGNED_BYTE);
            }
        }
    }
}
