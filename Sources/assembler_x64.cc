#include "assembler.h"

namespace Luna{
    void Assembler::call(Register reg){
        Operand oper(reg);
        EmitOperandREX(2, oper, REX_NONE);
        EmitUint8(0xFF);
        EmitOperand(2, oper);
    }

    void Assembler::call(const Address& addr) {
        EmitOperandREX(2, addr, REX_NONE);
        EmitUint8(0xFF);
        EmitOperand(2, addr);
    }

    void Assembler::call(Label* lbl){
        EmitUint8(0xE8);
        EmitLabel(lbl, 5);
    }

    void Assembler::call(const ExternalLabel* label) {
        EmitRegisterREX(TMP, REX_W);
        EmitUint8(0xB8 | (TMP & 7));
        EmitInt64(label->Address());
        call(TMP);
    }

    void Assembler::movq(Register dst, Register src) {
        Operand oper(dst);
        EmitOperandREX(src, oper, REX_W);
        EmitUint8(0x89);
        EmitOperand(src & 7, oper);
    }

    void Assembler::movq(Register dst, const Address& src){
        EmitOperandREX(dst, src, REX_W);
        EmitUint8(0x8B);
        EmitOperand(dst & 7, src);
    }

    void Assembler::movq(const Address& dst, Register src) {
        EmitOperandREX(src, dst, REX_W);
        EmitUint8(0x89);
        EmitOperand(src & 7, dst);
    }

    void Assembler::movq(Register dst, const Immediate& imm) {
        if(imm.Is32()){
            Operand oper(dst);
            EmitOperandREX(0, oper, REX_W);
            EmitUint8(0xC7);
            EmitOperand(0, oper);
        } else{
            EmitRegisterREX(dst, REX_W);
            EmitUint8((uint8_t) (0xB8 | (dst & 7)));
        }
        EmitImmediate(imm);
    }

    void Assembler::movq(const Address& dst, const Immediate& imm) {
        if(imm.Is32()){
            Operand oper(dst);
            EmitOperandREX(0, oper, REX_W);
            EmitUint8(0xC7);
            EmitOperand(0, oper);
            EmitImmediate(imm);
        } else{
            movq(TMP, imm);
            movq(dst, TMP);
        }
    }

    void Assembler::addq(Register dst, Register src) {
        Operand oper(src);
        EmitOperandREX(dst, oper, REX_W);
        EmitUint8(0x03);
        EmitOperand(dst & 7, oper);
    }

    void Assembler::addq(Register dst, const Address& addr){
        EmitOperandREX(dst, addr, REX_W);
        EmitUint8(0x03);
        EmitOperand(dst & 7, addr);
    }

    void Assembler::addq(Register dst, const Immediate& imm) {
        if(imm.Is32()){
            EmitRegisterREX(dst, REX_W);
            EmitComplex(0, Operand(dst), imm);
        } else{
            movq(TMP, imm);
            addq(dst, TMP);
        }
    }

    void Assembler::addq(const Address& address, const Immediate& imm) {
        if(imm.Is32()){
            EmitOperandREX(0, address, REX_W);
            EmitComplex(0, Operand(address), imm);
        } else{
            movq(TMP, imm);
            addq(address, TMP);
        }
    }

    void Assembler::addq(const Address& address, Register src) {
        EmitOperandREX(src, address, REX_W);
        EmitUint8(0x01);
        EmitOperand(src & 7, address);
    }

    void Assembler::push(Register src){
        EmitRegisterREX(src, REX_NONE);
        EmitUint8((uint8_t) (0x50 | (src & 7)));
    }

    void Assembler::pop(Register src) {
        EmitRegisterREX(src, REX_NONE);
        EmitUint8((uint8_t) (0x58 | (src & 7)));
    }

    void Assembler::ret() {
        EmitUint8(0xC3);
    }

    void Assembler::EmitComplex(int rm, const Operand& oper, const Immediate& imm) {
        if(imm.Is8()){
            EmitUint8(0x83);
            EmitOperand(rm, oper);
            EmitUint8((uint8_t) (imm.value_ & 0xFF));
        } else if(oper.IsRegister(RAX)){
            EmitUint8((uint8_t) (0x05 + (rm << 3)));
            EmitImmediate(imm);
        } else{
            EmitUint8(0x81);
            EmitOperand(rm, oper);
            EmitImmediate(imm);
        }
    }

    void Assembler::EmitLabel(Label* label, word instr_size) {
        if(label->IsBound()){
            word off = label->Position() - buffer_.Size();
            EmitInt32((int32_t) (off - instr_size));
        } else{
            EmitLabelLink(label);
        }
    }

    void Assembler::EmitLabelLink(Label* label) {
        word pos = buffer_.Size();
        EmitInt32((int32_t) label->position_);
        label->LinkTo(pos);
    }

    void Assembler::EmitImmediate(const Immediate& imm) {
        if(imm.Is32()){
            EmitInt32(static_cast<int32_t>(imm.value_));
        } else{
            EmitInt64(imm.value_);
        }
    }

    void Assembler::EmitOperand(int rm, const Operand& oper) {
        word len = oper.length_;
        EmitUint8((uint8_t) (oper.encoding_[0] + (rm << 3)));
        for(word i = 1; i < len; i++){
            EmitUint8(oper.encoding_[i]);
        }
    }
}