#include "global.h"
#if defined(OS_IS_LINUX)

#include "os.h"

namespace Luna{
    word OS::ActivationFrameAlignment() {
#if defined(ARCH_IS_X64)
        return 16;
#elif defined(ARCH_IS_32)
        return 8;
#else
#error "Unsupported Architecture"
#endif
    }
}

#endif