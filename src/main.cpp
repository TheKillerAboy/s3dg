#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ctime>
#include <map>
#include <s3dg/render/opengl/vertexarray.h>
#include <s3dg/render/opengl/vertexbuffer.h>
#include <s3dg/render/opengl/vertexbufferlayout.h>
#include <s3dg/render/opengl/indexbuffer.h>
#include <s3dg/render/opengl/shader.h>
#include <s3dg/render/opengl/renderer.h>
#include <s3dg/math/vec.h>

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << '\n';
    }

    std::cout << glGetString(GL_VERSION) << '\n';
    float poitions[] = {
        -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f,
        };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    s3dg::render::opengl::VertexBuffer vb(poitions, sizeof(float) * 8);
    s3dg::render::opengl::VertexBufferLayout vbl;
    vbl.push<float>(2);
    s3dg::render::opengl::VertexArray va;
    va.add_buffer(vb, vbl);
    s3dg::render::opengl::IndexBuffer ib(indices, 6);
    s3dg::render::opengl::Shader shader("res/shaders/basic.shader");
    s3dg::math::Vec4f color({0.1f, 0.8f, 0.2f, 1});
    float inc = 0.05f;
    s3dg::render::opengl::Renderer renderer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        shader.bind();
        shader.set_uniform("u_Color", color);

        if (color[0] > 1.0f) {
            inc = -0.05f;
        } else if (color[0] < 0.0f) {
            inc = 0.05f;
        }

        color[0] += inc;
        renderer.draw(va, ib, shader);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
