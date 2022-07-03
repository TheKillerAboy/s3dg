#pragma once
#include <s3dg/render/opengl/utils.h>
#include <s3dg/math/vec.h>
#include <unordered_map>

namespace s3dg{
namespace render{
namespace opengl{

struct ShaderProgramSource{
    std::string fragment;
    std::string vertex;
};

class Shader
{
private:
    std::string filepath;
    unsigned int renderer_id;
    std::unordered_map<std::string, int> uniform_locations;

    ShaderProgramSource load_shader_source() const;
    unsigned int create_shader(const ShaderProgramSource& source) const;
    unsigned int compile_shader(unsigned int type, const std::string& source) const;
    int get_uniform_location(const std::string& name);
public:
    Shader(const std::string& filepath);
    ~Shader();

    void bind() const;
    void unbind() const;

    template<size_t size, typename T>
    void set_uniform(const std::string& name, const math::Vec<size,T>& v);
};

}
}
}
