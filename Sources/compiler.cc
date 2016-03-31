#include "compiler.h"
#include "assembler.h"
#include "ast.h"
#include "intermediate_language.h"

namespace Luna{
    void Compiler::Compile(Function* func){
        Assembler* assm = new Assembler;
        std::vector<Instruction*> instrs = std::vector<Instruction*>();

        std::vector<AstNode*>::iterator nodeIter;
        for(nodeIter = func->ast_.begin(); nodeIter != func->ast_.end(); nodeIter++){
            std::cout << "Emitting instructions for " << (*nodeIter)->Name() << std::endl;
            (*nodeIter)->EmitInstructions(&instrs);
        }

        std::cout << "Total Instructions: " << instrs.size() << std::endl;

        std::vector<Instruction*>::iterator iter;
        for(iter = instrs.begin(); iter != instrs.end(); iter++){
            (*iter)->EmitMachineCode(assm);
        }

        func->compound_ = assm->Finalize();
    }
}