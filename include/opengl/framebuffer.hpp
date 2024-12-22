#pragma once

#include <glad/glad.h>
#include <core.hpp>
#include <opengl/texture.hpp>

namespace Heptcore{
    class Framebuffer{
        public:
            struct FramebufferTexture{
                uint internal_format;
                uint format;
                uint data_type;
            };

        protected:
            int width;
            int height;

        private:
            std::vector<Texture2D> textures = {};

            uint framebuffer_id;
            uint depth_renderbuffer_id;
            
        public:
            Framebuffer(int width, int height, std::vector<FramebufferTexture> textures);
            void bind();
            void unbind();

            std::vector<Texture2D>& getTextures() { return textures; };

            void bindTextures();
            void unbindTextures();

            int getWidth(){return width;}
            int getHeight(){return height;}
    };
    }