#pragma once
#include <s3dg/render/opengl/utils.h>
#include <s3dg/render/opengl/vertexbuffer.h>
#include <s3dg/render/opengl/vertexbufferlayout.h>

namespace s3dg{
namespace render{
namespace opengl{

class VertexArray
{
private:
    unsigned int renderer_id;
public:
    VertexArray();
    ~VertexArray();

    void add_buffer(const VertexBuffer& buffer, const VertexBufferLayout& layout);

    void bind() const;
    void unbind() const;
};




}
}
}
