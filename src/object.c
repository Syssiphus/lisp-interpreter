
#include <stdlib.h>
#include <stdio.h>

#include "object.h"
#include "memory.h"

object *make_fixnum(long num)
{
    object *obj;

    obj = alloc_object();
    obj->type = FIXNUM;
    obj->data.fixnum.value = num;
    return obj;
}

char is_fixnum_object(object *obj)
{
    return obj->type == FIXNUM;
}

long get_fixnum_value(object *obj)
{
    if (obj->type != FIXNUM)
    {
        fprintf(stderr, "Object is not a FIXNUM object.\n");
        exit(1);
    }

    return obj->data.fixnum.value;
}

object *make_character(int c)
{
    object *obj;

    obj = alloc_object();
    obj->type = CHARACTER;
    obj->data.character.value = c;
    return obj;
}

char is_character_object(object *obj)
{
    return obj->type == CHARACTER;
}

int get_character_value(object *obj)
{
    if (obj->type != CHARACTER)
    {
        fprintf(stderr, "Object is not a CHARACTER object.\n");
        exit(1);
    }

    return obj->data.character.value;
}

