#include "vm.h"

namespace Luna{
    bool Scope::AddVariable(LocalVariable* var) {
        if(LookupLocalVariable(var->Name())){
            return false;
        }

        variables_.Push(var);
        if(var->Owner() == nullptr){
            var->SetOwner(this);
        }
        return true;
    }

    LocalVariable* Scope::LookupLocalVariable(const std::string& name) {
        for(int i = 0; i < variables_.Length(); i++){
            if(variables_.At(i)->Name() == name){
                return variables_.At(i);
            }
        }

        return nullptr;
    }

    LocalVariable* Scope::LookupVariable(const std::string& name) {
        Scope* current = this;
        while(current != nullptr){
            LocalVariable* var = current->LookupLocalVariable(name);
            if(var != nullptr){
                return var;
            }

            current = current->parent_;
        }

        return nullptr;
    }

    int Scope::AllocateVariables(int num_pars, int first_index) {
        int pos = 0;
        int index = first_index;
        while(pos < num_pars){
            LocalVariable* par = variables_.At(pos);
            pos++;

            par->SetIndex(index--);
        }

        index = first_index;
        while(pos < variables_.Length()){
            LocalVariable* var = variables_.At(pos);
            pos++;

            if(var->Owner() == this){
                var->SetIndex(index--);
            }
        }

        int min_index = index;
        if(this->child_ != nullptr){
            Scope* child = this->child_;
            int child_index = child->AllocateVariables(0, 0);
            if(child_index < min_index){
                min_index = child_index;
            }
        }
        return min_index;
    }
}