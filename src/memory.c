
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GC_ACTIVE

#ifdef GC_ACTIVE
#include <gc.h>
#endif

#include "memory.h"

void *mem_pool;
void *active_mem_pool;
void *reserve_mem_pool;

void init_memory_pool(void)
{
#ifdef GC_ACTIVE
    GC_enable_incremental();
#else
    mem_pool = malloc(sizeof(object) * MEMORY_POOL_SIZE * 2);
    if ( ! mem_pool)
    {
        fprintf(stderr, "Cannot allocate memory pool.\n");
        exit(1);
    }

    /* Clear mem pool */
    memset(mem_pool, 0x0, sizeof(object) * MEMORY_POOL_SIZE * 2);

    active_mem_pool = mem_pool;
    reserve_mem_pool = mem_pool + (MEMORY_POOL_SIZE * sizeof(object));
#endif
}

long memory_usage(void)
{
#ifdef GC_ACTIVE
    return 0; /* TODO: implement for GC */
#else
    int i;
    long retval = 0;

    for (i = 0; i < MEMORY_POOL_SIZE; ++i)
    {
        if (((object *)active_mem_pool)[i].used) ++retval;
    }
    return retval;
#endif
}

object *get_mempool_object(void)
{
#ifdef GC_ACTIVE
    return GC_malloc(sizeof(object));
#else
    int i;
    object *obj = (object *)active_mem_pool;

    for (i = 0; i < MEMORY_POOL_SIZE; ++i)
    {
        if ( ! obj[i].used)
        {
            return (obj + i);
        }
    }

    return 0;
#endif
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

