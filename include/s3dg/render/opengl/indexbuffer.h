#pragma once

namespace s3dg{
namespace render{
namespace opengl{

class IndexBuffer
{
private:
    unsigned int renderer_id;
    unsigned int count;
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    inline unsigned int get_count() const { return count; }
};

    
}
}
}
