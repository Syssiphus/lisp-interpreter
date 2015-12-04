
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "builtins.h"
#include "memory.h"

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

void set_car(object *dst, object *obj)
{
    dst->data.pair.car = obj;
}

void set_cdr(object *dst, object *obj)
{
    dst->data.pair.cdr = obj;
}

object *length_proc(object *arguments)
{
    if ( ! is_pair_object(car(arguments)))
    {
        return make_error("'length' needs list as argument.");
    }
    else if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Too many arguments to 'length'.");
    }
    else
    {
        long result = 0;
        arguments = car(arguments);
        while ( ! is_the_empty_list(arguments))
        {
            ++result; arguments = cdr(arguments);
        }
        return make_fixnum(result);
    }
}

object *add_proc(object *arguments)
{
    long result = 0;
    while ( ! is_the_empty_list(arguments))
    {
        result += get_fixnum_value(car(arguments));
        arguments = cdr(arguments);
    }
    return make_fixnum(result);
}

object *sub_proc(object *arguments)
{
    if (is_the_empty_list(arguments))
    {
        return make_error("Arguments missing");
    }
    else
    {
        long result = get_fixnum_value(car(arguments));
        arguments = cdr(arguments);
        if (is_the_empty_list(arguments))
        {
            result *= -1;
        }
        else
        {
            while ( ! is_the_empty_list(arguments))
            {
                result -= get_fixnum_value(car(arguments));
                arguments = cdr(arguments);
            }
        }
        return make_fixnum(result);
    }
}

object *mul_proc(object *arguments)
{
    long result = 1;
    while ( ! is_the_empty_list(arguments))
    {
        result *= get_fixnum_value(car(arguments));
        arguments = cdr(arguments);
    }
    return make_fixnum(result);
}

object *quotient_proc(object *arguments)
{
    long result = 0;
    object *l = length_proc(cons(arguments, the_empty_list));

    if (is_error_object(l))
    {
        return make_error("Wrong argument type to 'quotient'.");
    }
    else if (get_fixnum_value(l) != 2)
    {
        return make_error("Procedure 'quotient' needs 2 arguments.");
    }
    else
    {
        result = get_fixnum_value(car(arguments))
            / get_fixnum_value(cadr(arguments));
        return make_fixnum(result);
    }
}

object *mem_usage_proc(object *obj)
{
    return make_fixnum(memory_usage());
}

object *is_eqv_proc(object *arguments)
{
    object *obj1 = car(arguments);
    object *obj2 = cadr(arguments);

    if (obj1->type != obj2->type)
    {
        return false;
    }

    switch (obj1->type)
    {
        case SYMBOL:
            return is_symbol_equal_proc(arguments);
        case STRING:
            return is_string_equal_proc(arguments);
        case CHARACTER:
            return is_character_equal_proc(arguments);
        case FIXNUM:
            return is_number_equal_proc(arguments);
        default:
            return (obj1 == obj2) ? true : false;
    }
}

object *is_symbol_equal_proc(object *arguments)
{
    object *obj1 = car(arguments);
    object *obj2 = cadr(arguments);

    if ( ! is_symbol_object(obj1) || ! is_symbol_object(obj2))
    {
        return false;
    }
    return (obj1 == obj2) ? true : false;
}

object *is_string_equal_proc(object *arguments)
{
    object *obj1 = car(arguments);
    object *obj2 = cadr(arguments);

    if ( ! is_string_object(obj1) || ! is_string_object(obj2))
    {
        return false;
    }

    return strcmp(get_string_value(obj1), get_string_value(obj2)) == 0
        ? true
        : false;
}

object *is_character_equal_proc(object *arguments)
{
    object *obj1 = car(arguments);
    object *obj2 = cadr(arguments);

    if ( ! is_character_object(obj1) || ! is_character_object(obj2))
    {
        return false;
    }

    return (get_character_value(obj1) == get_character_value(obj2))
        ? true
        : false;
}

object *is_number_equal_proc(object *arguments)
{
    object *obj1 = car(arguments);
    object *obj2 = cadr(arguments);

    if (obj1->type != obj2->type)
    {
        return false;
    }

    switch (obj1->type)
    {
        case FIXNUM:
            return (get_fixnum_value(obj1) == get_fixnum_value(obj2))
                ? true
                : false;
        default:
            return false;
    }
}



