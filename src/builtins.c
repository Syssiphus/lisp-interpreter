
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "globals.h"
#include "builtins.h"
#include "memory.h"
#include "read.h"
#include "eval.h"

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
    long result = 0;

    if ( ! is_pair_object(car(arguments)))
    {
        /* We need a pair object to count */
        return make_error("'length' needs list as argument.");
    }

    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("'length' takes exactly 1 argument (PAIR).");
    }

    /* Count elements */
    arguments = car(arguments);
    while ( ! is_the_empty_list(arguments))
    {
        ++result; arguments = cdr(arguments);
    }
    return make_fixnum(result);
}

object *add_proc(object *arguments)
{
    char need_realnum = 0;
    double result = 0;
    if (is_the_empty_list(arguments))
    {
        return make_error("Arguments missing");
    }
    while ( ! is_the_empty_list(arguments))
    {
        double summand;
        if (is_realnum_object(car(arguments)))
        {
            summand = get_realnum_value(car(arguments));
            need_realnum = 1;
        }
        else
        {
            summand = get_fixnum_value(car(arguments));
        }
        result = result + summand;
        arguments = cdr(arguments);
    }
    return need_realnum ? make_realnum(result) : make_fixnum(result);
}

object *sub_proc(object *arguments)
{
    char need_realnum = 0;
    char first_value = 1;
    double result = 0;
    if (is_the_empty_list(arguments))
    {
        return make_error("Arguments missing");
    }
    while ( ! is_the_empty_list(arguments))
    {
        double subtractor;
        if (is_realnum_object(car(arguments)))
        {
            subtractor = get_realnum_value(car(arguments));
            need_realnum = 1;
        }
        else
        {
            subtractor = get_fixnum_value(car(arguments));
        }

        if (first_value)
        {
            first_value = 0;
            if (is_the_empty_list(cdr(arguments)))
            {
                result = subtractor * -1;
            }
            else
            {
                result = subtractor;
            }
        }
        else
        {
            result = result - subtractor;
        }
        arguments = cdr(arguments);
    }
    return need_realnum ? make_realnum(result) : make_fixnum(result);
}

object *mul_proc(object *arguments)
{
    char need_realnum = 0;
    double result = 1;
    if (is_the_empty_list(arguments))
    {
        return make_error("Arguments missing");
    }
    while ( ! is_the_empty_list(arguments))
    {
        double factor;
        if (is_realnum_object(car(arguments)))
        {
            factor = get_realnum_value(car(arguments));
            need_realnum = 1;
        }
        else
        {
            factor = get_fixnum_value(car(arguments));
        }
        result = result * factor;
        arguments = cdr(arguments);
    }
    return need_realnum ? make_realnum(result) : make_fixnum(result);
}

object *quotient_proc(object *arguments)
{
    char need_realnum = 0;
    double result, value1, value2;
    if (is_the_empty_list(arguments) 
            || is_the_empty_list(cdr(arguments))
            || ! is_the_empty_list(cddr(arguments)))
    {
        return make_error("'quotient' needs exactly 2 arguments.");
    }
    if (is_realnum_object(car(arguments)))
    {
        value1 = get_realnum_value(car(arguments));
        need_realnum = 1;
    }
    else
    {
        value1 = get_fixnum_value(car(arguments));
    }
    if (is_realnum_object(cadr(arguments)))
    {
        value2 = get_realnum_value(cadr(arguments));
        need_realnum = 1;
    }
    else
    {
        value2 = get_fixnum_value(cadr(arguments));
    }
    result = value1 / value2;
    return need_realnum ? make_realnum(result) : make_fixnum(result);
}

object *mem_usage_proc(object *obj)
{
    return make_fixnum(memory_usage());
}

object *not_proc(object *arguments)
{
    object *l = length_proc(cons(arguments, the_empty_list));

    if (is_error_object(l)) return l;

    if (get_fixnum_value(l) != 1)
    {
        return make_error("Too many arguments to 'not' "
                "(got: %ld, expected: 1)", get_fixnum_value(l));
    }

    if (is_boolean_object(car(arguments)) && is_false(car(arguments)))
    {
        return true;
    }

    return false;
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
        case REALNUM:
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

    double value1, value2;

    switch (obj1->type)
    {
        case FIXNUM:
            value1 = get_fixnum_value(obj1);
            break;
        case REALNUM:
            value1 = get_realnum_value(obj1);
            break;
        default:
            return false;
    }

    switch (obj2->type)
    {
        case FIXNUM:
            value2 = get_fixnum_value(obj2);
            break;
        case REALNUM:
            value2 = get_realnum_value(obj2);
            break;
        default:
            return false;
    }

    return (value1 == value2) ? true : false;
}

object *load_proc(object *arguments)
{
    char *filename;
    FILE *in;
    object *exp;
    object *result = the_empty_list;

    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("'load' takes exactly 1 argument.");
    }

    if ( ! is_string_object(car(arguments)))
    {
        return make_error("'load' expects a STRING argument.");
    }

    filename = car(arguments)->data.string.value;
    in = fopen(filename, "r");
    if (in == NULL)
    {
        return make_error("Unable to open file '%s' (%s).", 
                filename, strerror(errno));
    }

    while (1)
    {
        exp = read(in);
        if (is_eof_object(exp))
        {
            break;
        }

        result = eval(exp, the_global_environment);
        if (is_error_object(result))
        {
            break;
        }
    }

    return result;
}



