#include <opengl/vao.hpp>

using namespace Heptcore;

VertexFormat::VertexFormat(std::initializer_list<VertexBindingType> bindings, bool per_instance): per_instance(per_instance), bindings(bindings){
    totalSize = 0;
    for(auto& size: bindings) totalSize += size;
}   

void VertexFormat::apply(uint& slot){
    size_t stride =  totalSize * sizeof(float);
    size_t size_to_now = 0;

    for(auto& current_size: bindings){
        uintptr_t pointer = size_to_now * sizeof(float);

        glVertexAttribPointer(slot, (int) current_size, GL_FLOAT, GL_FALSE, (int)stride, (void*)pointer);
        glEnableVertexAttribArray(slot);
        if(per_instance) glVertexAttribDivisor(slot, 1);

        size_to_now += current_size;
        slot++;
    }
}
