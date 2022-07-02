#pragma once
#include <s3dg/render/opengl/utils.h>

namespace s3dg{
namespace render{
namespace opengl{

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned int normalized;

    static unsigned int get_size_of_type(unsigned int type);
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;

    void inc_stride(unsigned int count, unsigned int type);
public:
    VertexBufferLayout(): stride(0){};

    template<typename T>
    void push(unsigned int count);

    inline const std::vector<VertexBufferElement>& get_elements() const { return elements; }
    inline unsigned int get_stride() const { return stride; }
};

}
}
}
