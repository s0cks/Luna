#ifndef LUNA_ARRAY_H
#define LUNA_ARRAY_H

#include <string.h>
#include "global.h"

namespace Luna{
    template<typename V> class Array{
    public:
        Array():
                length_(0),
                size_(10),
                data_(new V[10]()){}

        Array(int size):
                length_(0),
                size_(size),
                data_(new V[size]()){}

        V& operator[](word index) const{
            return this->data_[index];
        }

        void Push(const V& v){
            if(this->length_ > this->size_){
                word new_size = this->size_ * 2;
                V* new_data = new V[new_size];
                for(int i = 0; i < this->size_; i++){
                    new_data[i] = this->data_[i];
                }
                delete this->data_;
                this->data_ = new_data;
                this->size_ = new_size;
            }

            this->data_[this->length_] = v;
            this->length_++;
        }

        V& At(word index){
            return this->data_[index];
        }

        word Length() const{
            return this->length_;
        }

        static word DataOffset(int index){
            return offsetof(Array, data_) + (index * sizeof(V));
        }
    private:
        word size_;
        word length_;
        V* data_;
    };
}

#endif