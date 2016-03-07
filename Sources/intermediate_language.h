#ifndef LUNA_INTERMEDIATE_LANGUAGE_H
#define LUNA_INTERMEDIATE_LANGUAGE_H

#include "assembler.h"

namespace Luna{
#define DECL_INSTR void EmitMachineCode(Assembler* compiler);

    class Instruction{
    public:
        virtual void EmitMachineCode(Assembler* compiler) = 0;
    };

    class Object;
    class Function;

    class StoreTableEntryInstr: public Instruction{
    public:
        StoreTableEntryInstr(Register reg, int index, Object* obj):
                register_(reg),
                object_(obj),
                index_(index){}

        DECL_INSTR;
    private:
        Register register_;
        Object* object_;
        int index_;
    };

    class GetTableEntryInstr: public Instruction{
    public:
        GetTableEntryInstr(Register reg, int index, Register out):
                register_(reg),
                out_(out),
                index_(index){}

        DECL_INSTR;
    private:
        Register register_;
        Register out_;
        int index_;
    };

    class PushArgumentInstr: public Instruction{
    public:
        PushArgumentInstr(Object* val):
                value_(val){}

        DECL_INSTR;
    private:
        Object* value_;
    };

    class NativeCallInstr: public Instruction{
    public:
        NativeCallInstr(Function* ref):
                function_(ref){}

        DECL_INSTR;
    private:
        Function* function_;
    };

    class ReturnInstr: public Instruction{
    public:
        DECL_INSTR;
    };
}

#endif