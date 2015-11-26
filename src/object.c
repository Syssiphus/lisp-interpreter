
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "object.h"
#include "memory.h"

/** FIXNUMS */
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

/** CHARACTERS */
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

/** STRINGS */
object *make_string(char *str)
{
    object *obj;

    obj = alloc_object();

    obj->data.string.value = malloc(strlen(str) + 1);
    if ( ! obj->data.string.value)
    {
        fprintf(stderr, "Error allocating string space for string '%s'.", str);
        exit(1);
    }
    strcpy(obj->data.string.value, str);
    obj->type = STRING;
    return obj;
}

char is_string_object(object *obj)
{
    return obj->type == STRING;
}

char *get_string_value(object *obj)
{
    return obj->data.string.value;
}

