#ifndef LUNA_STACK_FRAME_H
#define LUNA_STACK_FRAME_H

#include "global.h"

#if defined(ARCH_IS_X64)
#include "stack_frame_x64.h"
#else
#error "Unsupported CPU architecture"
#endif

#endif