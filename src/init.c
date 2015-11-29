
#include "globals.h"
#include "memory.h"

void init(void)
{
    /* Initialize the mempool (must be first call) */
    init_memory_pool();

    /* Boolean constants */
    true = alloc_object();
    true->type = BOOLEAN;
    true->data.boolean.value = 1;

    false = alloc_object();
    false->type = BOOLEAN;
    false->data.boolean.value = 0;
}

