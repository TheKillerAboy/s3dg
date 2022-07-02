#pragma once

namespace s3dg{
namespace render{
namespace opengl{

class VertexBuffer
{
private:
    unsigned int renderer_id;
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
};

    
}
}
}
