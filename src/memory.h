
#pragma once

#include "object.h"

#define MEMORY_POOL_SIZE 50000

void init_memory_pool(void);
object *alloc_object(void);

