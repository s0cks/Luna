#include <assembler.h>
#include <vm.h>

using namespace Luna;

typedef void (*AssembledFunction)();

#define __ assm->

static void CPrint(Number* num1, Number* num2){
    Internal::CPrint(num1);
    Internal::CPrint(num2);
}

int main(int argc, char** argv){
    Number* num = new Number(3.141592654);
    ExternalLabel cprint_label(reinterpret_cast<uword>(&CPrint));
    Assembler* assm;

    assm = new Assembler();
    __ EnterStackFrame(0);
    __ movq(RDI, Immediate(bit_cast<int32_t, Number*>(num)));
    __ movq(RSI, Immediate(bit_cast<int32_t, Number*>(num)));
    __ call(&cprint_label);
    __ LeaveStackFrame();
    __ ret();

    reinterpret_cast<AssembledFunction>(assm->Finalize())();
    return 0;
}