#ifndef LUNA_ASSEMBLER_X64_H
#define LUNA_ASSEMBLER_X64_H

#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <iostream>
#include "global.h"
#include "constants_x64.h"
#include "assembler.h"

namespace Luna{
    class Immediate{
    public:
        Immediate(int64_t value): value_(value){}

        bool Is32() const{
            return IsInt(32, value_);
        }

        bool Is8() const{
            return IsInt(8, value_);
        }
    private:
        int64_t value_;

        friend class Assembler;
    };

    class Operand{
    public:
        uint8_t rex() const{
            return rex_;
        }

        uint8_t mod() const{
            return (uint8_t) ((encoding_at(0) >> 6) & 3);
        }

        Register rm() const{
            int rm_rex = (rex_ & REX_B) << 3;
            return static_cast<Register>(rm_rex + (encoding_at(0) & 7));
        }

        ScaleFactor scale() const{
            return static_cast<ScaleFactor>((encoding_at(1) >> 6) & 3);
        }

        Register index() const{
            int index_rex = (rex_ & REX_W) << 2;
            return static_cast<Register>(index_rex + ((encoding_at(1) >> 3) & 7));
        }

        Register base() const{
            int base_rex = (rex_ & REX_B) << 3;
            return static_cast<Register>(base_rex + (encoding_at(1) & 7));
        }
    protected:
        Operand(): length_(0), rex_(REX_NONE){}

        void SetModRM(int mod, Register rm){
            if((rm > 7) && !((rm == R12) && (mod != 3))){
                rex_ |= REX_B;
            }
            encoding_[0] = (uint8_t) ((mod << 6) | (rm & 7));
            length_ = 1;
        }

        void SetSIB(ScaleFactor scale, Register index, Register base){
            if(base > 7) rex_ |= REX_B;
            if(index > 7) rex_ |= REX_W;
            encoding_[1] = (scale << 6) | ((index & 7) << 3) | (base & 7);
            length_ = 2;
        }

        void SetDisp8(int8_t disp){
            encoding_[length_++] = static_cast<uint8_t>(disp);
        }

        void SetDisp32(int32_t disp){
            memmove(&encoding_[length_], &disp, sizeof(disp));
            length_ += sizeof(disp);
        }
    private:
        uint8_t rex_;
        uint8_t length_;
        uint8_t encoding_[6];

        Operand(Register reg): rex_(REX_NONE){
            SetModRM(3, reg);
        }

        uint8_t encoding_at(word index) const{
            return encoding_[index];
        }

        bool IsRegister(Register reg) const{
            return ((reg > 7 ? 1 : 0) == (rex_ == REX_B))
                && ((encoding_at(0) & 0xF8) == 0xC0) && ((encoding_at(0) & 0x07) == reg);
        }

        friend class Assembler;
    };

    class Address: public Operand{
    public:
        Address(Register base, int32_t disp){
            if((disp == 0) && ((base & 7) != RBP)){
                SetModRM(0, base);
                if((base & 7) == RSP){
                    SetSIB(TIMES_1, RSP, base);
                }
            } else if(IsInt(8, disp)){
                SetModRM(1, base);
                if((base & 7) == RSP){
                    SetSIB(TIMES_1, RSP, base);
                }
                SetDisp8((int8_t) disp);
            } else{
                SetModRM(2, base);
                if((base & 7) == RSP){
                    SetSIB(TIMES_1, RSP, base);
                }
                SetDisp32(disp);
            }
        }

        Address(Register index, ScaleFactor scale, int32_t disp){
            SetModRM(0, RSP);
            SetSIB(scale, index, RBP);
            SetDisp32(disp);
        }

        Address(Register base, Register index, ScaleFactor scale, int32_t disp){
            if((disp == 0) && ((base & 7) != RBP)){
                SetModRM(0, RSP);
                SetSIB(scale, index, base);
            } else if(IsInt(8, disp)){
                SetModRM(1, RSP);
                SetSIB(scale, index, base);
                SetDisp8((int8_t) disp);
            } else{
                SetModRM(2, RSP);
                SetSIB(scale, index, base);
                SetDisp32(disp);
            }
        }
    private:
        Address(Register base, int32_t disp, bool fixed){
            SetModRM(2, base);
            if((base & 7) == RSP){
                SetSIB(TIMES_1, RSP, base);
            }
            SetDisp32(disp);
        }

        friend class Assembler;
    };

    class FieldAddress : public Address{
    public:
        FieldAddress(Register base, int32_t disp):
                Address(base, disp - kWordSize){}

        FieldAddress(Register base, Register index, ScaleFactor scale, int32_t disp):
                Address(base, index, scale, disp - kWordSize){}
    };

    class Label{
    public:
        Label(): position_(0), unresolved_(0){}

        word Position() const{
            return -position_ - kWordSize;
        }

        word LinkPosition() const{
            return position_ - kWordSize;
        }

        bool IsBound() const{
            return position_ < 0;
        }

        bool IsLinked() const{
            return position_ > 0;
        }
    private:
        word position_;
        word unresolved_;

        void BindTo(word position){
            position_ = -position - kWordSize;
        }

        void LinkTo(word position){
            position_ = position + kWordSize;
        }

        friend class Assembler;
    };

    class Assembler{
    public:
        void call(Register reg);
        void call(const Address& addr);
        void call(Label* label);
        void call(const ExternalLabel* label);

        void movsd(XMMRegister dst, const Address& src);
        void movsd(const Address& dst, XMMRegister src);
        void movq(Register dst, const Immediate& imm);
        void movq(Register dst, Register src);
        void movq(Register dst, const Address& src);
        void movq(const Address& dst, Register src);
        void movq(const Address& dst, const Immediate& imm);
        void movaps(XMMRegister dst, XMMRegister src);

        void addsd(XMMRegister dst, XMMRegister src);
        void addq(Register dst, Register src);
        void addq(Register dst, const Immediate& imm);
        void addq(Register dst, const Address& address);
        void addq(const Address& address, const Immediate& imm);
        void addq(const Address& address, Register src);

        void subq(Register dst, Register src);
        void subq(Register dst, const Immediate& imm);
        void subq(Register dst, const Address& addr);
        void subq(const Address& addr, Register src);
        void subq(const Address& addr, const Immediate& imm);

        void pushq(const Immediate& imm);
        void pushq(Register src);
        void popq(Register src);

        void andq(Register dst, Register src);
        void andq(Register dst, const Immediate& imm);

        void leaq(Register dst, const Address& src);

        void LoadImmediate(Register reg, const Immediate& imm);
        void MoveImmediate(const Address& dst, const Immediate& imm);
        void LoadObject(Register dst, const Object& object);
        void PushObject(const Object& object);

        //TODO: Experimental?
        // ================================
        void EnterStackFrame(int size);
        void LeaveStackFrame();
        // =================================

        void ret();

        void* Finalize(){
            void* ptr = mmap(0, buffer_.cursor_ - buffer_.contents_, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
            if(ptr == NULL){
                std::cerr << "Cannot finalize assembled code" << std::endl;
                abort();
            }
            memcpy(ptr, reinterpret_cast<void*>(buffer_.contents_), buffer_.cursor_ - buffer_.contents_);
            return ptr;
        }

        ObjectPot object_pot_;
    private:
        AssemblerBuffer buffer_;

        inline void EmitUint8(uint8_t value){
            buffer_.Emit<uint8_t>(value);
        }

        inline void EmitInt32(int32_t value){
            buffer_.Emit<int32_t>(value);
        }

        inline void EmitInt64(int64_t value){
            buffer_.Emit<int64_t>(value);
        }

        inline void EmitRegisterREX(Register reg, uint8_t rex){
            rex |= (reg > 7 ? REX_B : REX_NONE);
            if(rex != REX_NONE) EmitUint8(REX_PREFIX | rex);
        }

        inline void EmitOperandREX(int rm, const Operand& oper, uint8_t rex){
            rex |= (rm > 7 ? REX_R : REX_NONE) | oper.rex();
            if(rex != REX_NONE) EmitUint8(REX_PREFIX | rex);
        }

        inline void EmitREX_RB(XMMRegister reg, XMMRegister base, uint8_t rex = REX_NONE){
            if(reg > 7) rex |= REX_R;
            if(base > 7) rex |= REX_B;
            if(rex != REX_NONE) EmitUint8(REX_PREFIX | rex);
        }

        inline void EmitREX_RB(XMMRegister reg, const Operand& oper, uint8_t rex = REX_NONE){
            if(reg > 7) rex |= REX_R;
            rex |= oper.rex();
            if(rex != REX_NONE) EmitUint8(REX_PREFIX | rex);
        }

        inline void EmitREX_RB(Register reg, XMMRegister base, uint8_t rex = REX_NONE){
            if(reg > 7) rex |= REX_R;
            if(base > 7) rex |= REX_B;
            if(rex != REX_NONE) EmitUint8(REX_PREFIX | rex);
        }

        inline void EmitXmmRegisterOperand(int rm, XMMRegister xmm){
            Operand oper;
            oper.SetModRM(3, static_cast<Register>(xmm));
            EmitOperand(rm, oper);
        }

        void EmitComplex(int rm, const Operand& oper, const Immediate& imm);
        void EmitOperand(int rm, const Operand& oper);
        void EmitImmediate(const Immediate& imm);
        void EmitLabel(Label* label, word instr_size);
        void EmitLabelLink(Label* label);
        void LoadWordFromPool(Register dst, int32_t off);
    };
}

#endif