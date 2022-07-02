#include <s3dg/render/opengl/utils.h>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, const int& line) {
    while (GLenum error = glGetError()) {
        spdlog::error("[OpenGL Error] ({}): {} {}:{}", error, function, file, line);
        return false;
    }

    return true;
}
