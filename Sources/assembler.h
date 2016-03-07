#ifndef LUNA_ASSEMBLER_H
#define LUNA_ASSEMBLER_H

#include "global.h"

namespace Luna{
    class ExternalLabel{
    public:
        ExternalLabel(uword addr): address_(addr){}

        uword Address() const{
            return address_;
        }
    private:
        uword address_;
    };

    class AssemblerBuffer{
    public:
        AssemblerBuffer();
        ~AssemblerBuffer();

        template<typename T> void Emit(T value){
            *reinterpret_cast<T*>(this->cursor_) = value;
            this->cursor_ += sizeof(T);
        }

        uword Address(word pos) const{
            return this->contents_ + pos;
        }

        uword Size() const{
            return this->cursor_ - this->contents_;
        }

        template<typename T> T Load(word pos){
            return *reinterpret_cast<T*>(this->contents_ + pos);
        }

        template<typename T> void Store(word pos, T value){
            *reinterpret_cast<T*>(this->contents_ + pos) = value;
        }
    private:
        uword contents_;
        uword cursor_;
        uword limit_;

        friend class Assembler;
    };
}

#ifdef ARCH_IS_X64
#include "assembler_x64.h"
#elif ARCH_IS_X32
#include "assembler_x86.h"
#else
#error "Unsupported CPU Architecture"
#endif

#endif