
#include <stdlib.h>
#include <stdio.h>
#include "builtins.h"

object *cons(object *a, object *b)
{
    return make_pair(a, b);
}

object *car(object *obj)
{
    if (is_pair_object(obj))
    {
        return obj->data.pair.car;
    }
    fprintf(stderr, "Not a pair object.\n");
    exit(1);
}

object *cdr(object *obj)
{
    if (is_pair_object(obj))
    {
        return obj->data.pair.cdr;
    }
    fprintf(stderr, "Not a pair object.\n");
    exit(1);
}

