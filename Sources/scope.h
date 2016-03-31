#ifndef LUNA_SCOPE_H
#define LUNA_SCOPE_H

#include <string>
#include "type.h"
#include "array.h"

namespace Luna{
    class LocalVariable;

    class Scope{
    public:
        Scope(Scope* parent = nullptr):
                parent_(parent),
                child_(nullptr),
                variables_(Array<LocalVariable*>()){
            if(parent != nullptr){
                parent->child_ = this;
            }
        }

        bool AddVariable(LocalVariable* var);
        LocalVariable* LookupVariable(const std::string& name);
        LocalVariable* LookupLocalVariable(const std::string& name);
        int AllocateVariables(int num_pars, int first_index);
    private:
        Scope* parent_;
        Scope* child_;
        Array<LocalVariable*> variables_;
    };

    class LocalVariable{
    public:
        LocalVariable(const std::string& name, enum TID tid):
                type_(tid),
                constant_value_(nullptr),
                owner_(nullptr),
                name_(name){}

        void SetOwner(Scope* scope){
            owner_ = scope;
        }

        void SetIndex(int index){
            index_ = index;
        }

        int GetIndex(){
            return index_;
        }

        Scope* Owner(){
            return owner_;
        }

        std::string Name(){
            return name_;
        }

        enum TID Type(){
            return type_;
        }

        void SetConstantValue(Object* value){
            constant_value_ = value;
        }

        bool IsConstant(){
            return constant_value_ != nullptr;
        }

        Object* GetConstantValue(){
            return constant_value_;
        }
    private:
        std::string name_;
        enum TID type_;
        Scope* owner_;
        Object* constant_value_;
        int index_;
    };
}

#endif