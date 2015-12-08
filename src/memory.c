
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"

void *mem_pool;
void *active_mem_pool;
void *reserve_mem_pool;

void init_memory_pool(void)
{
    GC_enable_incremental();
}

long memory_usage(void)
{
    return 0; /* TODO: implement for GC */
}

object *get_mempool_object(void)
{
    return GC_malloc(sizeof(object));
}

object *alloc_object(void)
{
    object *obj;
    obj = get_mempool_object();

    if ( ! obj)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }

    obj->used = 1;

    return obj;
}

object *alloc_with_finalizer(GC_finalization_proc p)
{
    object *obj = alloc_object();
    GC_register_finalizer((void *) obj, p, NULL, 0, 0);
    return obj;
}

