#include <heptcore.hpp>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

void framebufferToPixels(uint32_t* pixels) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            pixels[y * WIDTH + x] = (255 << 24) | ((x * 255 / WIDTH) << 16) | ((y * 255 / HEIGHT) << 8) | 255;
        }
    }
}

int main() {
    Heptcore::Window window{1920,1080,"Hello World!"};
    
    Heptcore::ShaderProgram program{};
    // Simple shader program
    program.addShaderSource(R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )", GL_VERTEX_SHADER);

    program.addShaderSource(R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D screenTexture;
        void main() {
            FragColor = texture(screenTexture, TexCoord);
        }
    )", GL_FRAGMENT_SHADER);

    program.compile();
    program.use();

    uint32_t* pixels = new uint32_t[WIDTH * HEIGHT];
    framebufferToPixels(pixels);

    Heptcore::Texture2D texture{reinterpret_cast<unsigned char*>(pixels), WIDTH, HEIGHT};
    
    texture.bind(0);
    program.setSamplerSlot("screenTexture", 0);

    Heptcore::FullscreenQuad quad{};

    // Main loop
    while (!window.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);

        quad.render();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}