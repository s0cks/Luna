#ifndef LUNA_TYPE_H
#define LUNA_TYPE_H

#include <stddef.h>
#include <string>
#include <iostream>
#include <vector>
#include "global.h"
#include "intermediate_language.h"

#define TYPES_LIST(V) \
    V(String) \
    V(Number) \
    V(Function) \
    V(Boolean) \
    V(Nil) \
    V(Table)

namespace Luna{
#define VALUE_OFFSET(V) \
    static word ValueOffset(){ \
        return offsetof(V, value_); \
    }

    enum TID{
#define DEF_TID(clazz) k##clazz##TID,
        TYPES_LIST(DEF_TID)
#undef DEF_TID
        kUnknownTID
    };

    enum Modifier{
        kMOD_COMPOUND,
        kMOD_NATIVE
    };

    class Object{
    protected:
        enum TID type_;

        Object(enum TID type):
                type_(type){}
    public:
        enum TID Type() const{
            return this->type_;
        }

        bool IsExecutable() const{
            return this->type_ == kFunctionTID;
        }

        bool RequiresFpu() const{
            return this->type_ == kNumberTID;
        }

        static Object* NIL;
    };

    class String : public Object{
    public:
        String(const std::string& val):
                value_(val),
                Object(kStringTID){}

        VALUE_OFFSET(String);
    private:
        std::string value_;
    };

    class Number : public Object{
    public:
        Number(const double& val):
                value_(val),
                Object(kNumberTID){}

        VALUE_OFFSET(Number);
    private:
        double value_;
    };

    typedef void (*CFunction)(Object*);

    class Function : public Object{
    public:
        Function(const std::string& name):
                name_(name),
                modifiers_(kMOD_COMPOUND),
                instructions_(std::vector<Instruction*>()),
                Object(kFunctionTID){}

        Function(const std::string& name, int mods):
                name_(name),
                modifiers_(mods),
                instructions_(std::vector<Instruction*>()),
                Object(kFunctionTID){}

        void* GetCompoundFunction() const{
            return this->compound_;
        }

        CFunction GetNativeFunction() const{
            return this->native_;
        }

        bool IsNative() const{
            return ((modifiers_ & kMOD_NATIVE) == kMOD_NATIVE);
        }

        static Function* Native(const std::string& name, CFunction f){
            Function* func = new Function(name, kMOD_NATIVE);
            func->native_ = f;
            return func;
        }
    private:
        int modifiers_;
        std::string name_;
        std::vector<Instruction*> instructions_;
        union{
            void* compound_;
            CFunction native_;
        };

        friend class Parser;
        friend class Compiler;
    };

    class Boolean : public Object{
    public:
        Boolean(const bool& val):
                value_(val),
                Object(kBooleanTID){}

        VALUE_OFFSET(Boolean);
    private:
        bool value_;
    };

    class Table : public Object{
    public:
        static Table* New(word len);

        void SetAt(word index, Object* val){
            *ObjectAddrAtOffset(index * kWordSize) = val;
        }

        Object* GetAt(word index){
            return *ObjectAddrAtOffset(index * kWordSize);
        }
    private:
        word size_;
        word asize_;

        Table(word len):
            size_(0),
            asize_(len),
            Object(kTableTID){}

        Object** ObjectAddrAtOffset(word offset) const{
            return reinterpret_cast<Object**>(reinterpret_cast<word>(this) + offset);
        }
    };
}

#endif