
#pragma once

#include "object.h"

#define MEMORY_POOL_SIZE 50000

#define STRING_BUFFER_SIZE 128

typedef struct
{
    size_t size;
    char * buffer;
} string_buffer;

void init_memory_pool(void);
object *alloc_object(void);

string_buffer alloc_temp_string_buffer(void);
void realloc_temp_string_buffer(string_buffer *strbf);


