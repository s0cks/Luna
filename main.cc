#include <type.h>
#include <parser.h>
#include <compiler.h>
#include <intermediate_language.h>

using namespace Luna;

typedef void (*AssembledFunction)();

int main(int argc, char** argv){
    Scope* global = new Scope();

    Luna::Internal::InitOnce(global);
    FILE* fin = fopen(argv[1], "r");
    Function* script = (new Parser(fin))->Parse(global);
    fclose(fin);

    Compiler::Compile(script);
    reinterpret_cast<AssembledFunction>(script->GetCompoundFunction())();
    return 0;
}