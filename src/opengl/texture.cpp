#include <opengl/texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Heptcore;

static std::array<uint, 32> texture_bindings = {};

BindableTexture::BindableTexture(){
    glGenTextures(1, &this->texture);
}
BindableTexture::~BindableTexture(){
    glDeleteTextures(1, &this->texture);
}
void BindableTexture::bind(int unit) const{
    if(unit < 0 || unit >= 32) return;
    if(texture_bindings[unit] == this->texture) return;

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(TYPE, this->texture);

    texture_bindings[unit] = this->texture;
}  

void BindableTexture::unbind(int unit) const{
    if(unit < 0 || unit >= 32) return;
    if(texture_bindings[unit] != this->texture) return;

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(TYPE, 0);

    texture_bindings[unit] = 0;
}

void BindableTexture::parameter(int identifier, int value){
    glTexParameteri(TYPE, identifier, value);
}

uint BindableTexture::getType() const {return TYPE;}
uint BindableTexture::getID() const {return texture;}

void Texture2D::loadData(unsigned char* data, int width, int height, int channels){
    glBindTexture(GL_TEXTURE_2D, this->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    if(channels != 3 && channels != 4){
        std::cout << "Invalid number of channels when loading texture (only 4 and 3 allowed): " << channels;
        return;
    }

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture2D::Texture2D(const char* filename): Texture2D(){
    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return;
    }

    loadData(data, width, height, nrChannels);

    stbi_image_free(data);
}

Texture2D::Texture2D(unsigned char* data, int width, int height): Texture2D(){
    loadData(data, width, height, 4);
}

void Texture2D::configure(int storage_type, int color_format, int data_type, int width, int height, void* data){
    if(configured) reset();
    bind(0);

    glTexImage2D(GL_TEXTURE_2D, 0, storage_type, width, height, 0, color_format, data_type, data );
    
    parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  

    configured = true;
}

void Texture2D::reset(){
    glDeleteTextures(1, &this->texture);
    glGenTextures(1, &this->texture);
}

TextureArray2D::TextureArray2D(){
    TYPE = GL_TEXTURE_2D_ARRAY;
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->texture);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

void TextureArray2D::loadFromFiles(std::vector<std::string>& filenames, int layerWidth, int layerHeight){
    TYPE = GL_TEXTURE_2D_ARRAY;
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->texture);

    int size = (int)filenames.size();

    int mipLevels = (int) floor(log2(fmax(layerWidth, layerHeight))) + 1;
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_RGBA8, layerWidth, layerHeight,  size);

    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data;  
    for(int i = 0; i < size; i++)
    {   
        data = stbi_load(filenames[i].c_str(), &width, &height, &nrChannels, 4);
    
        if (!data) throw std::runtime_error("Failed to load texture in texture array 2D.\n");

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, std::min(width, layerWidth), std::min(height, layerHeight), 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    //CHECK_GL_ERROR();;

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);

    //CHECK_GL_ERROR();;

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    //CHECK_GL_ERROR();;
}

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

void Skybox::load(std::array<std::string, 6> filenames){
    TYPE = GL_TEXTURE_CUBE_MAP;

    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data = nullptr;  
    for(uint i = 0; i < 6; i++)
    {
        data = stbi_load(filenames[i].c_str(), &width, &height, &nrChannels, 0);

        if (!data) {
            std::cout << "Failed to load texture: " <<  filenames[i] << std::endl;
            throw std::runtime_error("Failed to load texture when creating skybox.");
        }
       // std::cout << "Loaded " << filenames[i] << " successfully! Channels:" << nrChannels << std::endl;

        if(nrChannels != 4 && nrChannels != 3){
            std::cout << "Invalid channels in skybox texture: " << nrChannels << std::endl;
            throw std::runtime_error("Invalid channels in skybox texture.");
        }

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    uint VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_DYNAMIC_DRAW);
    
    //CHECK_GL_ERROR();;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //CHECK_GL_ERROR();;

    this->vertexBufferID = VBO;
    this->vao = vao;
}
void Skybox::draw(){
    glDepthMask(GL_FALSE);
    glBindVertexArray(this->vao);
    
    //CHECK_GL_ERROR();;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
    
    //CHECK_GL_ERROR();;

    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    //CHECK_GL_ERROR();;
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}
Skybox::~Skybox(){
    glDeleteBuffers(1 , &this->vertexBufferID);
    glDeleteVertexArrays(1, &this->vao);
}