#ifndef LUNA_VM_H
#define LUNA_VM_H

#include <map>
#include "type.h"

namespace Luna{
    class Scope{
    public:
        Scope(Scope* parent = nullptr):
                parent_(parent),
                scope_(std::map<std::string, Object*>()){}

        Object* Lookup(const std::string& value){
            Scope* curr = this;
            while(curr != nullptr){
                if(curr->scope_.find(value) != curr->scope_.end()){
                    return curr->scope_.find(value)->second;
                }

                curr = curr->parent_;
            }

            return nullptr;
        }

        void Define(const std::string& name, Object* value){
            scope_[name] = value;
        }
    private:
        Scope* parent_;
        std::map<std::string, Object*> scope_;
    };

    namespace Internal{
        static void CPrint(Object* obj){
            switch(obj->Type()){
                case kStringTID: std::cout << *reinterpret_cast<std::string*>(reinterpret_cast<word>(static_cast<String*>(obj)) + String::ValueOffset()) << std::endl; break;
                case kNumberTID: std::cout << *reinterpret_cast<double*>(reinterpret_cast<word>(static_cast<Number*>(obj)) + Number::ValueOffset()) << std::endl; break;
            }
        }

#define NATIVE_FUNCTION(NAME, F) \
        scope->Define((NAME), Function::Native((NAME), (void*) F));

        static void InitOnce(Scope* scope){
            NATIVE_FUNCTION("print", &CPrint);
        }
    }
}

#endif