#include <string.h>
#include <assert.h>
#include "type.h"
#include "stack_frame_x64.h"
#include "scope.h"

namespace Luna{
    Object* Object::NIL = new Object(kNilTID);
    Boolean* Boolean::TRUE = new Boolean(true);
    Boolean* Boolean::FALSE = new Boolean(false);

    Table* Table::New(word len) {
        void* data = malloc(sizeof(Table) + (kWordSize * len));
        Table* table = reinterpret_cast<Table*>(data);
        table->type_ = kTableTID;
        table->size_ = 0;
        table->asize_ = len;
        return table;
    }

    ObjectPool* ObjectPool::New(word len) {
        void* data = malloc(sizeof(ObjectPool) + (sizeof(Entry) * len));
        ObjectPool* pool = reinterpret_cast<ObjectPool*>(data);
        pool->length_ = len;
        return pool;
    }

    void Function::AllocateVariables() {
        int first_param_index = kFirstLocalSlotFromFP;
        int first_local_index =  first_param_index - params_;
        int next_free = scope_->AllocateVariables(params_, first_param_index);
        assert(next_free <= first_local_index);
        locals_ = first_local_index - next_free;
    }
}