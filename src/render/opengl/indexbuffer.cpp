#include <s3dg/render/opengl/indexbuffer.h>
#include <s3dg/render/opengl/utils.h>

namespace s3dg {
    namespace render {
        namespace opengl {

            IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
                : count(count) {
                GLCall(glGenBuffers(1, &renderer_id));
                GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id));
                GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
            }
            IndexBuffer::~IndexBuffer() {
                GLCall(glDeleteBuffers(1, &renderer_id));
            }

            void IndexBuffer::bind() const {
                GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));
            }
            void IndexBuffer::unbind() const {
                GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
            }

        }
    }
}
