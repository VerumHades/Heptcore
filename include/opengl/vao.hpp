#pragma once

#include <vector>
#include <initializer_list>

#include <opengl/buffer.hpp>

namespace Heptcore{
    enum VertexBindingType{
        FLOAT = 1,
        VEC2 = 2,
        VEC3 = 3,
        VEC4 = 4
    };

    class VertexFormat{
        private:
            std::vector<VertexBindingType> bindings = {};
            uint totalSize = 0;
            bool per_instance = false;
        public:
            VertexFormat(std::initializer_list<VertexBindingType> bindings, bool per_instance = false);
            void apply(uint& slot);
            uint getVertexSize(){return totalSize;}
    };

    /*
        A class to manage the vertex array object and its format
    */
    class VertexArrayObject{
        private:
            uint vao_id;

            struct BoundBuffer{
                Buffer<float, GL_ARRAY_BUFFER>* buffer_pointer;
                VertexFormat format;
            };

            std::vector<BoundBuffer> buffers;
        public:
            VertexArrayObject(){
                glGenVertexArrays(1,  &vao_id);
            }
            ~VertexArrayObject(){
                glDeleteVertexArrays(1,  &vao_id);
            }

            size_t attachBuffer(Buffer<float, GL_ARRAY_BUFFER>* buffer_pointer, VertexFormat format){
                buffers.push_back({buffer_pointer, format});

                update();
                
                return format.getVertexSize();
            }

            void attachBuffer(Buffer<uint, GL_ELEMENT_ARRAY_BUFFER>* buffer){
                bind();
                buffer->bind();
                unbind();
            }

            /*
                Updates buffers, bindings locations are based on how the buffers were attached sequentialy
            */
            void update(){
                bind();

                uint slot = 0;

                for(auto& [buffer_pointer, format]: buffers){
                    buffer_pointer->bind();
                    format.apply(slot);
                }

                unbind();
            }
            void bind() const {
                glBindVertexArray(vao_id);
            }
            void unbind() const {
                glBindVertexArray(0);
            }
    };
}