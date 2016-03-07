#include "global.h"

#ifndef ARCH_IS_X64
#error "Unsupported CPU Architecture"
#endif

#include "intermediate_language.h"
#include "type.h"

#define __ compiler->

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
            __ push(register_);
        } else{
            if(value_->RequiresFpu()){
                __ movq(static_cast<Register>(XMM0), Immediate(reinterpret_cast<word>(value_)));
                __ push(static_cast<Register>(XMM0));
            } else{
                __ movq(TMP, Immediate(reinterpret_cast<word>(value_)));
                __ push(TMP);
            }
        }
    }

    void NativeCallInstr::EmitMachineCode(Assembler* compiler) {
        __ pop(RDI);
        ExternalLabel label(reinterpret_cast<uword>(function_->GetNativeFunction()));
        __ call(&label);
    }

    void NewTableInstr::EmitMachineCode(Assembler* compiler) {
        fprintf(stderr, "Unimplemented code");
        abort();
    }

    void LoadObjectInstr::EmitMachineCode(Assembler* compiler) {
        __ movq(register_, Immediate(reinterpret_cast<uword>(object_)));
    }
}