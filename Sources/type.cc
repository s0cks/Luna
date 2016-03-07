#include "type.h"

namespace Luna{
    Object* Object::NIL = new Object(kNilTID);

    Table* Table::New(word len) {
        void* data = malloc(sizeof(Table) + (kWordSize * len));
        Table* table = reinterpret_cast<Table*>(data);
        table->type_ = kTableTID;
        table->size_ = 0;
        table->asize_ = len;
        return table;
    }
}