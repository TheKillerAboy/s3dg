#include <s3dg/render/opengl/renderer.h>

namespace s3dg {
    namespace render {
        namespace opengl {

            void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) {
                va.bind();
                ib.bind();
                shader.bind();
                GLCall(glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr));
            }

        }
    }
}
