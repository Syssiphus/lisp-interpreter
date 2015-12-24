
#pragma once

#include <stddef.h>

#include <gc.h>

#include "object.h"

#define MEMORY_POOL_SIZE 50000

#define STRING_BUFFER_SIZE 128

typedef struct
{
    size_t size;
    char * buffer;
} string_buffer;

void init_memory_pool(void);
long memory_usage(void);
object *alloc_object(void);
object *alloc_with_finalizer(GC_finalization_proc p);



