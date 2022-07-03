#pragma once
#include <s3dg/render/opengl/utils.h>
#include <s3dg/render/opengl/vertexarray.h>
#include <s3dg/render/opengl/indexbuffer.h>
#include <s3dg/render/opengl/shader.h>

namespace s3dg{
namespace render{
namespace opengl{

class Renderer{
public:
    void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
};

}
}
}
