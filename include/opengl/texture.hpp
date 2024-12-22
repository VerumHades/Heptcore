#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <unordered_map>
#include <glm/glm.hpp>

#include <core.hpp>

namespace Heptcore{
    class BindableTexture{
        protected: 
            uint texture = 0;
            uint TYPE = GL_TEXTURE_2D;
            BindableTexture();
            virtual ~BindableTexture();
        public:
            void bind(int unit) const;
            void unbind(int unit) const;
            void parameter(int identifier, int value);
            uint getType() const;
            uint getID() const;
    };

    class Texture2D: public BindableTexture{
        private:
            bool configured = false;
            void loadData(unsigned char* data, int width, int height, int channels);

        public:
            Texture2D(){TYPE = GL_TEXTURE_2D;};
            Texture2D(const char* filename);
            Texture2D(unsigned char* data, int width, int height);
            void configure(int internal_format, int format, int data_type, int width, int height, void* data = nullptr);
            void reset();
    };

    class TextureArray2D: public BindableTexture{
        public:
            TextureArray2D();
            void setup(int width, int height, int layers){
                bind(0);
                glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height,  layers);
            }
            void loadFromFiles(std::vector<std::string>& filenames, int layerWidth, int layerHeight);
    };

    class Skybox: public BindableTexture{
        private:
            uint vertexBufferID;
            uint vao;
        public:
            void load(std::array<std::string, 6> filenames);
            ~Skybox();

            void draw();
    };
}