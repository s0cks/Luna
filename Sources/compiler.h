#ifndef LUNA_COMPILER_H
#define LUNA_COMPILER_H

#include "type.h"

namespace Luna{
    class Compiler{
    public:
        static void Compile(Function* func);
    };
}

#endif