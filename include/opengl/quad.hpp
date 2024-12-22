#pragma once

#include <opengl/buffer.hpp>
#include <opengl/vao.hpp>
#include <array>

namespace Heptcore{
    class FullscreenQuad{
        private:
            VertexArrayObject vao = {};
            Buffer<float, GL_ARRAY_BUFFER> quad_buffer = {};
        public:
            FullscreenQuad();
            void render();
    };
}