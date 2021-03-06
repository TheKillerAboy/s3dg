#include <s3dg/render/opengl/vertexbuffer.h>
#include <s3dg/render/opengl/utils.h>

namespace s3dg {
    namespace render {
        namespace opengl {

            VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
                GLCall(glGenBuffers(1, &renderer_id));
                GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));
                GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
            }
            VertexBuffer::~VertexBuffer() {
                GLCall(glDeleteBuffers(1, &renderer_id));
            }

            void VertexBuffer::bind() const {
                GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));
            }
            void VertexBuffer::unbind() const {
                GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
            }

        }
    }
}
