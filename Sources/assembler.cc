#include <stdlib.h>
#include "assembler.h"

namespace Luna{
    static inline uword AllocUnsafe(word size){
        return reinterpret_cast<uword>(malloc((size_t) size));
    }

    static const word kMinGap = 32;
    static const word kInitialBufferCapacity = 4 * (1024 * 1024);

    static inline uword Limit(uword data, word cap){
        return data + cap + kMinGap;
    }

    AssemblerBuffer::AssemblerBuffer(){
        this->cursor_ = this->contents_ = AllocUnsafe(kInitialBufferCapacity);
        this->limit_ = Limit(this->contents_, kInitialBufferCapacity);
    }
}