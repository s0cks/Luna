#include "compiler.h"
#include "assembler.h"
#include "intermediate_language.h"

namespace Luna{
    void Compiler::Compile(Function* func){
        Assembler* assm = new Assembler;
        for(std::vector<Instruction*>::iterator iter = func->instructions_.begin(); iter != func->instructions_.end(); iter++){
            (*iter)->EmitMachineCode(assm);
        }
        func->compound_ = assm->Finalize();
        //TODO: Deletable Assembler: delete assm;
    }
}