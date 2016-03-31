#include "assembler.h"
#include "ast.h"

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

    void Assembler::movsd(XMMRegister dst, const Address& src) {
        EmitUint8(0xF2);
        EmitREX_RB(dst, src);
        EmitUint8(0x0F);
        EmitUint8(0x10);
        EmitOperand(dst & 7, src);
    }

    void Assembler::movsd(const Address& dst, XMMRegister src) {
        EmitUint8(0xF2);
        EmitREX_RB(src, dst);
        EmitUint8(0x0F);
        EmitUint8(0x11);
        EmitOperand(src & 7, dst);
    }

    void Assembler::leaq(Register dst, const Address& src){
        EmitOperandREX(dst, src, REX_W);
        EmitUint8(0x8D);
        EmitOperand(dst & 7, src);
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

    void Assembler::movaps(XMMRegister dst, XMMRegister src) {
        EmitREX_RB(dst, src);
        EmitUint8(0x0F);
        EmitUint8(0x28);
        EmitXmmRegisterOperand(dst & 7, src);
    }

    void Assembler::addsd(XMMRegister dst, XMMRegister src) {
        EmitUint8(0xF2);
        EmitREX_RB(dst, src);
        EmitUint8(0x0F);
        EmitUint8(0x58);
        EmitXmmRegisterOperand(dst & 7, src);
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

    void Assembler::subq(Register dst, Register src) {
        Operand oper(src);
        EmitOperandREX(dst, oper, REX_W);
        EmitUint8(0x2B);
        EmitOperand(dst & 7, oper);
    }

    void Assembler::subq(Register dst, const Immediate& imm){
        if(imm.Is32()){
            EmitRegisterREX(dst, REX_W);
            EmitComplex(5, Operand(dst), imm);
        } else{
            movq(TMP, imm);
            subq(dst, TMP);
        }
    }

    void Assembler::subq(Register dst, const Address& addr) {
        EmitOperandREX(dst, addr, REX_W);
        EmitUint8(0x2B);
        EmitOperand(dst & 7, addr);
    }

    void Assembler::subq(const Address& addr, Register src){
        EmitOperandREX(src, addr, REX_W);
        EmitUint8(0x29);
        EmitOperand(src & 7, addr);
    }

    void Assembler::subq(const Address& addr, const Immediate& imm) {
        if(imm.Is32()){
            EmitOperandREX(0, addr, REX_W);
            EmitComplex(5, Operand(addr), imm);
        } else{
            movq(TMP, imm);
            subq(addr, TMP);
        }
    }

    void Assembler::pushq(const Immediate& imm) {
        if(imm.Is8()){
            EmitUint8(0x6A);
            EmitUint8((uint8_t) (imm.value_ & 0xFF));
        } else if(imm.Is32()){
            EmitUint8(0x68);
            EmitImmediate(imm);
        } else{
            movq(TMP, imm);
            pushq(TMP);
        }
    }

    void Assembler::pushq(Register src){
        EmitRegisterREX(src, REX_NONE);
        EmitUint8((uint8_t) (0x50 | (src & 7)));
    }

    void Assembler::popq(Register src) {
        EmitRegisterREX(src, REX_NONE);
        EmitUint8((uint8_t) (0x58 | (src & 7)));
    }

    void Assembler::andq(Register dst, Register src) {
        Operand oper(src);
        EmitOperandREX(dst, oper, REX_W);
        EmitUint8(0x23);
        EmitOperand(dst & 7, oper);
    }

    void Assembler::andq(Register dst, const Immediate& imm){
        if(imm.Is32()){
            EmitRegisterREX(dst, REX_W);
            EmitComplex(4, Operand(dst), imm);
        } else{
            movq(TMP, imm);
            andq(dst, TMP);
        }
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

    void Assembler::LoadWordFromPool(Register dst, int32_t off) {
        movq(dst, Address(PP, off, true));
    }

    void Assembler::LoadImmediate(Register reg, const Immediate& imm) {
        if(imm.Is32()){
            movq(reg, imm);
        } else{
            int32_t off = static_cast<int32_t>(ObjectPool::element_offset(object_pot_.FindImmediate(static_cast<uword>(imm.value_))));
            LoadWordFromPool(reg, off);
        }
    }

    void Assembler::MoveImmediate(const Address& dst, const Immediate& imm) {
        if(imm.Is32()){
            movq(dst, imm);
        } else{
            LoadImmediate(TMP, imm);
            movq(dst, TMP);
        }
    }

    void Assembler::LoadObject(Register dst, const Object& object){
        int32_t offset = static_cast<int32_t>(ObjectPool::element_offset(object_pot_.FindImmediate(static_cast<uword>(reinterpret_cast<int64_t>(&object)))));
        LoadWordFromPool(dst, offset);
    }

    void Assembler::PushObject(const Object& object) {
        LoadImmediate(TMP, Immediate(reinterpret_cast<int64_t>(&object)));
        pushq(TMP);
    }

    void Assembler::EnterStackFrame(int size) {
        pushq(RBP);
        movq(RBP, RSP);
        if(size != 0){
            Immediate space(size);
            subq(RSP, space);
        }
    }

    void Assembler::LeaveStackFrame(){
        movq(RSP, RBP);
        popq(RBP);
    }
}