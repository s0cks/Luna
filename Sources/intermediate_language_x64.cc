#include "global.h"

#ifndef ARCH_IS_X64
#error "Unsupported CPU Architecture"
#endif

#include "intermediate_language.h"

#define __ compiler->
#define UWORD(V) reinterpret_cast<uword>((V))
#define IMM(V) Immediate(UWORD(V))

namespace Luna{
    void ReturnInstr::EmitMachineCode(Assembler* compiler) {
        __ ret();
    }

    void StoreTableEntryInstr::EmitMachineCode(Assembler* compiler) {
        __ movq(Address(register_, (int32_t) (sizeof(Table) + (index_ * kWordSize))), Immediate(reinterpret_cast<word>(object_)));
    }

    void GetTableEntryInstr::EmitMachineCode(Assembler* compiler) {
        __ movq(out_, Address(register_, (int32_t) (sizeof(Table) + (index_ * kWordSize))));
    }

    void PushArgumentInstr::EmitMachineCode(Assembler* compiler) {
        if(type_ == 0xB){
            __ pushq(register_);
        } else{
            __ movq(TMP, IMM(value_));
            __ pushq(TMP);
        }
    }

    void NativeCallInstr::EmitMachineCode(Assembler* compiler) {
        ExternalLabel label(UWORD(function_->GetNativeFunction()));
        __ call(&label);
    }

    void CompoundCallInstr::EmitMachineCode(Assembler* compiler) {
        ExternalLabel label(UWORD(function_->GetCompoundFunction()));
        __ call(&label);
    }

    void NewTableInstr::EmitMachineCode(Assembler* compiler) {
        fprintf(stderr, "Unimplemented code");
        abort();
    }

    void LoadObjectInstr::EmitMachineCode(Assembler* compiler) {
        __ movq(register_, IMM(object_));
    }

    void StoreLocalInstr::EmitMachineCode(Assembler* compiler) {
        __ movq(Address(RBP, local_->GetIndex() * kWordSize), IMM(local_->GetConstantValue()));
    }

    void LoadLocalInstr::EmitMachineCode(Assembler* compiler) {
        __ movq(register_, Address(RBP, local_->GetIndex() * kWordSize));
    }
}