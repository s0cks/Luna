#ifndef LUNA_TYPE_H
#define LUNA_TYPE_H

#include <stddef.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "global.h"

#define TYPES_LIST(V) \
    V(String) \
    V(Number) \
    V(Function) \
    V(Boolean) \
    V(Nil) \
    V(Table)

namespace Luna{
    class AstNode;
    class Scope;

#define VALUE_OFFSET(V) \
    static word ValueOffset(){ \
        return offsetof(V, value_); \
    }

    enum TID{
        kIllegalTID = 1,
#define DEF_TID(clazz) k##clazz##TID,
        TYPES_LIST(DEF_TID)
#undef DEF_TID,
        kUnknownTID
    };

    enum Modifier{
        kMOD_COMPOUND,
        kMOD_NATIVE
    };

    class Object{
    protected:
        enum TID type_;

        Object(enum TID type = kIllegalTID):
                type_(type){}
    public:
        enum TID Type() const{
            return type_;
        }

        bool IsExecutable() const{
            return this->type_ == kFunctionTID;
        }

        bool IsNil() const{
            return this->type_ == kNilTID;
        }

        static Object* NIL;
    };

    class String : public Object{
    public:
        String(const std::string& val):
                value_(val),
                Object(kStringTID){}

        word Hash(){
            int h = 0;
            if(value_.size() > 0){
                int off = 0;
                for(int i = 0; i < value_.size(); i++){
                    h = 31 * h + value_.at((unsigned long) off++);
                }
            }
            return h;
        }

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
        Function(const std::string& name, Scope* scope):
                name_(name),
                scope_(scope),
                params_(0),
                ast_(std::vector<AstNode*>()),
                modifiers_(kMOD_COMPOUND),
                Object(kFunctionTID){}

        Function(const std::string& name, Scope* scope, int mods):
                name_(name),
                modifiers_(mods),
                params_(0),
                scope_(scope),
                ast_(std::vector<AstNode*>()),
                Object(kFunctionTID){}

        void* GetCompoundFunction() const{
            return this->compound_;
        }

        CFunction GetNativeFunction() const{
            return this->native_;
        }

        int GetParametersCount() const{
            return this->params_;
        }

        void SetParametersCount(int count){
            params_ = count;
        }

        bool IsNative() const{
            return ((modifiers_ & kMOD_NATIVE) == kMOD_NATIVE);
        }

        static inline Function* Native(const std::string& name, CFunction f){
            Function* func = new Function(name, nullptr, kMOD_NATIVE);
            func->native_ = f;
            return func;
        }

        int GetLocalCount() const{
            return this->locals_;
        }

        std::vector<AstNode*> ast_;

        void AllocateVariables();

        static word CompoundOffset(){
            return offsetof(Function, compound_);
        }

        static word NativeOffset(){
            return offsetof(Function, native_);
        }
    private:
        int modifiers_;
        int params_;
        int locals_;
        std::string name_;
        Scope* scope_;
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

        static Boolean* TRUE;
        static Boolean* FALSE;
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
            return reinterpret_cast<Object**>(reinterpret_cast<word>(this) + sizeof(Table) + offset);
        }
    };

    class ObjectPool{
    public:
        enum EntryKind{
            kTagged,
            kImmediate,
            kNative
        };

        struct Entry{
            union{
                Object* immediate_;
                uword native_;
            };
            EntryKind type_;
        };

        static ObjectPool* New(word len);

        EntryKind Info(word index){
            return data()[index].type_;
        }

        void SetObjectAt(word index, const Object& obj){
            data()[index].immediate_ = &const_cast<Object&>(obj);
        }

        Object* ObjectAt(word index){
            return data()[index].immediate_;
        }

        void SetRawAt(word index, uword raw){
            data()[index].native_ = raw;
        }

        uword RawAt(word index){
            return data()[index].native_;
        }

        static word element_offset(word index){
            word offset = (reinterpret_cast<word>(reinterpret_cast<ObjectPool*>(32)->data()) - 32);
            return offset + ((sizeof(ObjectPool::Entry)) * index);
        }
    private:
        word length_;

        Entry* data(){
            return reinterpret_cast<Entry*>(reinterpret_cast<uword>(this) + sizeof(ObjectPool));
        }
    };
}

#define LUNA_STRING(V) \
    (*reinterpret_cast<std::string*>(reinterpret_cast<uword>((V)) + String::ValueOffset()))

#define LUNA_NUMBER(V) \
    (*reinterpret_cast<double*>(reinterpret_cast<uword>((V)) + Number::ValueOffset()))

#define LUNA_BOOLEAN(V) \
    (*reinterpret_cast<bool*>(reinterpret_cast<uword>((V)) + Boolean::ValueOffset()))

#define IS_NUMBER(V) ((V)->Type() == kNumberTID)
#define IS_STRING(V) ((V)->Type() == kStringTID)
#define IS_BOOLEAN(V) ((V)->Type() == kBooleanTID)
#define IS_TABLE(V) ((V)->Type() == kTableTID)
#define IS_FUNCTION(V) ((V)->Type() == kFunctionTID)

#endif