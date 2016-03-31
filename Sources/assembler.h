#ifndef LUNA_ASSEMBLER_H
#define LUNA_ASSEMBLER_H

#include "global.h"
#include "type.h"
#include "array.h"

namespace Luna{
    class ExternalLabel{
    public:
        ExternalLabel(uword addr): address_(addr){}

        uword Address() const{
            return address_;
        }
    private:
        uword address_;
    };

    class AssemblerBuffer{
    public:
        AssemblerBuffer();
        ~AssemblerBuffer(){}

        template<typename T> void Emit(T value){
            *reinterpret_cast<T*>(this->cursor_) = value;
            this->cursor_ += sizeof(T);
        }

        uword Address(word pos) const{
            return this->contents_ + pos;
        }

        uword Size() const{
            return this->cursor_ - this->contents_;
        }

        template<typename T> T Load(word pos){
            return *reinterpret_cast<T*>(this->contents_ + pos);
        }

        template<typename T> void Store(word pos, T value){
            *reinterpret_cast<T*>(this->contents_ + pos) = value;
        }
    private:
        uword contents_;
        uword cursor_;
        uword limit_;

        friend class Assembler;
    };

    //TODO: Improve upon the code below

    struct Entry{
        union{
            Object* object_;
            uword raw_;
        };
        ObjectPool::EntryKind type_;

        Entry(): raw_(), type_(){}
        Entry(const Object* obj): object_(const_cast<Object*>(obj)), type_(ObjectPool::kTagged){}
        Entry(uword value, ObjectPool::EntryKind type): raw_(value), type_(type){}
    };

    // Simple linked hash map?
    class ObjectMap{
    public:
        class Pair{
        public:
            Pair(Entry key, word value):
                    value_(value){
                key_.type_ = key.type_;
                if(key.type_ == ObjectPool::kTagged){
                    key_.object_ = key.object_;
                } else{
                    key_.raw_ = key.raw_;
                }
            }

            static word HashCode(Entry key){
                if(key.type_ != ObjectPool::kTagged){
                    return key.raw_;
                }

                if(key.object_->Type() == kNumberTID){
                    return static_cast<word>(bit_cast<int32_t, float>(static_cast<float>(LUNA_NUMBER(key.object_))));
                }

                if(key.object_->Type() == kStringTID){
                    return static_cast<String*>(key.object_)->Hash();
                }

                return key.object_->Type();
            }

            static inline bool Equals(Entry key1, Entry key2){
                if(key1.type_ != key2.type_) return false;
                if(key1.type_ == ObjectPool::kTagged){
                    return key1.object_ == key2.object_;
                }
                return key1.raw_ == key2.raw_;
            }

            Entry key_;
        private:
            word value_;

            friend class ObjectMap;
            friend class ObjectPot;
        };

        struct Bucket{
            struct Bucket* next;
            struct Bucket* prev;
            Pair* pair;
        };

        ObjectMap(): root_(nullptr){}

        Pair* Get(Entry key){
            Bucket* bucket = root_;
            while(bucket != nullptr){
                if(Pair::HashCode(bucket->pair->key_) == Pair::HashCode(key)){
                    if(Pair::Equals(bucket->pair->key_, key)){
                        return bucket->pair;
                    }
                }

                bucket = bucket->next;
            }

            return nullptr;
        }

        void Put(Entry key, word value){
            Pair* pair = Get(key);
            if(pair != nullptr){
                pair->value_ = value;
                return;
            }

            Bucket* bucket = new Bucket();
            bucket->pair = new Pair(key, value);
            bucket->next = root_;
            if(root_ != nullptr){
                root_->prev = bucket;
            }
            root_ = bucket;
        }

        void Delete(Entry key){
            Bucket* bucket = root_;
            while(bucket != nullptr){
                if(Pair::HashCode(bucket->pair->key_) == Pair::HashCode(key)){
                    if(Pair::Equals(bucket->pair->key_, key)){
                        delete bucket->pair;
                        if(bucket->next != nullptr){
                            bucket->next->prev = bucket->prev;
                        }
                        if(bucket->prev != nullptr){
                            bucket->prev->next = bucket->next;
                        }
                        delete bucket;
                    }
                }

                bucket = bucket->next;
            }
        }
    private:
        Bucket* root_;
    };

    class ObjectPot{
    public:
        enum Patchability{
            kPatchable,
            kNotPatchable
        };

        word AddObject(const Object& obj, Patchability patchability = kNotPatchable);
        word AddImmediate(uword imm);
        word FindObject(const Object& obj, Patchability patchability = kNotPatchable);
        word FindImmediate(uword imm);
    private:
        word AddObject(Entry entry, Patchability patchability);
        word FindObject(Entry entry, Patchability patchability);

        Array<Entry> object_pool_;
        ObjectMap object_map_;
    };

    //TODO: Till here
}

#if defined(ARCH_IS_X64)
#include "assembler_x64.h"
#elif defined(ARCH_IS_X32)
#include "assembler_x86.h"
#else
#error "Unsupported CPU Architecture"
#endif

#endif