#include <s3dg/render/opengl/shader.h>
#include <regex>
#include <fstream>

namespace s3dg {
    namespace render {
        namespace opengl {

            Shader::Shader(const std::string& filepath): filepath(filepath), renderer_id(0) {
                auto source = load_shader_source();
                renderer_id = create_shader(source);
            }
            Shader::~Shader() {
                GLCall(glDeleteProgram(renderer_id));
            }

            ShaderProgramSource Shader::load_shader_source() const {
                std::ifstream stream(filepath);
                std::regex shader_segement_expr("^#shader (\\w+)$");
                std::smatch expr_match;
                std::string line;
                std::string current_shader = "";
                std::unordered_map<std::string, std::string> shader_segments;

                while (getline(stream, line)) {
                    std::regex_match(line, expr_match, shader_segement_expr);

                    if (expr_match.empty()) {
                        shader_segments[current_shader] += line + '\n';
                    } else {
                        current_shader = expr_match[1];
                    }
                }

                return { shader_segments["fragment"], shader_segments["vertex"] };
            }

            unsigned int Shader::compile_shader(unsigned int type, const std::string& source) const {
                GLCall(unsigned int id = glCreateShader(type));
                const char* src = source.c_str();
                GLCall(glShaderSource(id, 1, &src, NULL));
                GLCall(glCompileShader(id));
                int result;
                GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

                if (result == GL_FALSE) {
                    int lenght;
                    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght));
                    char* message = (char*)alloca(lenght * sizeof(char));
                    GLCall(glGetShaderInfoLog(id, lenght, &lenght, message));
                    spdlog::error("Failed to compile {}:\n{}", type, message);
                    GLCall(glDeleteShader(id));
                    return 0;
                }

                return id;
            }

            unsigned int Shader::create_shader(const ShaderProgramSource& source) const {
                GLCall(unsigned int program = glCreateProgram());
                unsigned int vs = compile_shader(GL_VERTEX_SHADER, source.vertex);
                unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, source.fragment);
                GLCall(glAttachShader(program, vs));
                GLCall(glAttachShader(program, fs));
                GLCall(glLinkProgram(program));
                GLCall(glValidateProgram(program));
                GLCall(glDeleteShader(vs));
                GLCall(glDeleteShader(fs));
                return program;
            }

            void Shader::bind() const {
                GLCall(glUseProgram(renderer_id));
            }
            void Shader::unbind() const {
                GLCall(glUseProgram(0));
            }

            template<>
            void Shader::set_uniform(const std::string& name, const math::Vec<4, float>& v) {
                GLCall(glUniform4f(get_uniform_location(name), v[0], v[1], v[2], v[3]));
            }

            int Shader::get_uniform_location(const std::string& name) {
                auto it = uniform_locations.find(name);

                if (it != uniform_locations.end()) {
                    return it->second;
                }

                GLCall(int location = glGetUniformLocation(renderer_id, name.c_str()));
                uniform_locations[name] = location;

                if (location == -1) {
                    spdlog::warn("Uniform location {} doesn't exist", name);
                }

                return location;
            }

        }
    }
}
