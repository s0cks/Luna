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

    word ObjectPot::AddObject(Entry entry, Patchability patchability) {
        object_pool_.Push(entry);
        if(patchability == kNotPatchable){
            object_map_.Put(entry, object_pool_.Length() - 1);
        }
        return object_pool_.Length() - 1;
    }

    word ObjectPot::FindObject(Entry entry, Patchability patchability) {
        if(patchability == kNotPatchable){
            return object_map_.Get(entry)->value_;
        }

        return AddObject(entry, patchability);
    }

    word ObjectPot::FindObject(const Object& obj, Patchability patchability) {
        return FindObject(Entry(&const_cast<Object&>(obj)), patchability);
    }

    word ObjectPot::FindImmediate(uword imm) {
        return FindObject(Entry(imm, ObjectPool::kImmediate), kNotPatchable);
    }

    word ObjectPot::AddObject(const Object& obj, Patchability patchability) {
        return AddObject(Entry(&const_cast<Object&>(obj)), patchability);
    }

    word ObjectPot::AddImmediate(uword imm) {
        return AddObject(Entry(imm, ObjectPool::kImmediate), kNotPatchable);
    }
}