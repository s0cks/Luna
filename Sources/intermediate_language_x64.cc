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
        __ movq(Address(register_, (index_ * kWordSize)), Immediate(reinterpret_cast<word>(object_)));
    }

    void GetTableEntryInstr::EmitMachineCode(Assembler* compiler) {
        index_ = (int) (sizeof(Table) + (index_ * kWordSize));
        __ movq(out_, FieldAddress(register_, index_));
    }

    void PushArgumentInstr::EmitMachineCode(Assembler* compiler) {
        if(value_->RequiresFpu()){
            __ movq(static_cast<Register>(XMM0), Immediate(reinterpret_cast<word>(value_)));
            __ push(static_cast<Register>(XMM0));
        } else{
            __ movq(TMP, Immediate(reinterpret_cast<word>(value_)));
            __ push(TMP);
        }
    }

    void NativeCallInstr::EmitMachineCode(Assembler* compiler) {
        __ pop(RDI);
        ExternalLabel label(reinterpret_cast<uword>(function_->GetNativeFunction()));
        __ call(&label);
    }
}