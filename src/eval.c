
#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "eval.h"

char is_self_evaluating(object *obj);

object *eval(object *expr, object *env)
{
    if (is_self_evaluating(expr))
    {
        return expr;
    }

    fprintf(stderr, "%s, %d: Unknown object type '%s'\n", 
            __FILE__, __LINE__, __func__);
    exit(1); /* TODO: Error handling */
}

char is_self_evaluating(object *obj)
{
    return is_fixnum_object(obj)
        || is_character_object(obj)
        || is_string_object(obj)
        || is_boolean_object(obj)
        
        || is_symbol_object(obj) /* FIXME: this has to be changed! */;
}



