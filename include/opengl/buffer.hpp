#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <queue>
#include <glm/glm.hpp>
#include <unordered_map>
#include <map>
#include <list>

#include <chrono>

#include <core.hpp>

namespace Heptcore{
    void checkGLError(const char *file, int line);
    #define CHECK_GL_ERROR() checkGLError(__FILE__, __LINE__)

    template <typename T, int type>
    class Buffer{
        private:
            uint opengl_buffer_id;
            
            size_t buffer_size = 0;

            bool initialized = false;
        public:
            Buffer(){
                glGenBuffers(1, &opengl_buffer_id);
            }
            ~Buffer(){
                glDeleteBuffers(1, &opengl_buffer_id);
            }

            Buffer(std::vector<T> data): Buffer(){
                initialize(data.size());
                insert(0,data.size(), data.data());
            }

            /*
                Creates the actual buffer of some size
            */
            void initialize(size_t size, T* data = nullptr){
                if(size == 0) return;

                bind();
                glBufferData(type, size * sizeof(T), data, GL_DYNAMIC_DRAW);
                buffer_size = size;

                initialized = true;
            }

            /*
                Inserts data into a buffer, needs to be initialized
            */
            void insert(size_t at, size_t size, T* data){
                if(!initialized) throw std::logic_error("Inserting into uninitialized buffer.");

                if(at + size > buffer_size) throw std::logic_error("Insert out of bounds the buffer."); // Dont overflow

                bind();
                glBufferSubData(type, at * sizeof(T), size * sizeof(T), data);
            }
            void bind(){
                glBindBuffer(type, opengl_buffer_id);
            }

            size_t size(){
                return buffer_size;
            }
    };

    template <typename T, int type>
    class DoubleBuffer{
        private:
            Buffer<T,type> buffers[2];
            int current = 0;
        public:
            void swap(){
                this->current = !this->current;
            }
            Buffer<T,type>& getBuffer(){
                return this->buffers[this->current];
            }
            Buffer<T,type>& getBackBuffer(){
                return this->buffers[!this->current];
            }
    };

    template <typename T>
    class PersistentBuffer{
        private:
            uint buffer_id;
            uint type;
            size_t size;
            T* _data;
        
        public:
            PersistentBuffer(size_t size, uint type): type(type), size(size){
                glGenBuffers(1, &buffer_id);
                glBindBuffer(type, buffer_id);
                glBufferStorage(type, size, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

                _data = static_cast<T*>(glMapBufferRange(type, 0, size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));

                if(!_data) {
                    throw std::runtime_error("Failed to map persistent buffer.");
                }
            }

            ~PersistentBuffer(){
                glDeleteBuffers(1, &buffer_id);
            }

            uint getID() {return buffer_id;}
            T* data() {return this->_data;};

    };
}