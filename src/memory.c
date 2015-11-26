
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"

void * mem_pool;
void * active_mem_pool;
void * reserve_mem_pool;

void init_memory_pool(void)
{
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
}

object *get_mempool_object(void)
{
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

string_buffer alloc_temp_string_buffer(void)
{
    string_buffer retval;
    retval.size = STRING_BUFFER_SIZE;
    retval.buffer = malloc(retval.size);

    if ( ! retval.buffer)
    {
        fprintf(stderr, "Unable to alloc memory for string buffer.\n");
        exit(1);
    }

    fprintf(stdout, "Allocating string buffer (%d).\n", retval.size);

    return retval;
}

void realloc_temp_string_buffer(string_buffer *strbf)
{
    size_t new_size = strbf->size + STRING_BUFFER_SIZE;
    char *new_buffer = realloc(strbf->buffer, new_size);

    if ( ! new_buffer)
    {
        fprintf(stderr, "Unable to re-alloc memory for string buffer.\n");
        exit(1);
    }

    fprintf(stdout, "Reallocated string buffer (%d).\n", new_size);

    strbf->size += new_size;
    strbf->buffer = new_buffer;
}

