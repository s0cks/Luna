#ifndef LUNA_VM_H
#define LUNA_VM_H

#include <map>
#include <assert.h>
#include "type.h"
#include "scope.h"

namespace Luna{
#define CHECK_TYPE(V) switch((V)->Type()){
#define IS(V) case k##V##TID:
#define DEFAULT default:
#define END }

    namespace Internal{
        static std::string CToString(Object* obj){
            CHECK_TYPE(obj)
                IS(String) return LUNA_STRING(obj);
                IS(Number) {
                    std::stringstream stream;
                    stream << LUNA_NUMBER(obj);
                    return stream.str();
                }
                IS(Boolean){
                    std::stringstream stream;
                    stream << LUNA_BOOLEAN(obj);
                    return stream.str();
                }
                DEFAULT{
                    std::stringstream stream;
                    stream << "Object[" << obj->Type() << "]";
                    return stream.str();
                }
            END
        }

        static std::string CType(Object* obj){
            CHECK_TYPE(obj)
                IS(String) return "string";
                IS(Number) return "number";
                IS(Table) return "table";
                IS(Function) return "function";
                IS(Boolean) return "boolean";
                DEFAULT{
                    return "[unknown]";
                }
            END
        }

        static void CPrint(Object* obj) {
            CHECK_TYPE(obj)
                IS(String) std::cout << LUNA_STRING(obj) << std::endl; break;
                IS(Number) std::cout << LUNA_NUMBER(obj) << std::endl; break;
                IS(Boolean) std::cout << LUNA_BOOLEAN(obj) << std::endl; break;
                DEFAULT break;
            END
        }

        static void InitOnce(Scope* scope){

        }
    }

#undef CHECK_TYPE
#undef IS
#undef DEFAULT
#undef END
}

#endif