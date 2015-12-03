
#include <stdlib.h>
#include <stdio.h>
#include "globals.h"
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


