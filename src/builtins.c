
#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <math.h>
#include <pcre.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <dlfcn.h>
/* Does this work? stdbool.h is included through dlfcn.h and defines true/false
   but we already use true/false as objects. Maybe the undef's wont hurt */
#undef true
#undef false

#include "globals.h"
#include "builtins.h"
#include "memory.h"
#include "read.h"
#include "eval.h"

void scheme_write(FILE *out, object *obj);
object *find_variable(object *symbol, object *env);

long _number_of_args(object *arguments)
{
    long nr_args = 0;

    while ( ! is_the_empty_list(arguments))
    {
        nr_args++;
        arguments = cdr(arguments);
    }

    return nr_args;
}

/* This shall never be called. It is being handled in eval() */
object *apply_fake_proc(object *arguments, object *env)
{
    UNUSED(arguments);
    return make_error("Primitive function 'apply' should not "
                      "be called natively.");
}

/* This shall never be called. It is being handled in eval() */
object *eval_fake_proc(object *arguments, object *env)
{
    UNUSED(arguments);
    return make_error("Primitive function 'eval' should not "
                      "be called natively.");
}

object *cons_proc(object *arguments, object *env)
{
    object *car_obj = car(arguments);
    object *cdr_obj = cadr(arguments);
    return cons(car_obj, cdr_obj);
}

object *car_proc(object *arguments, object *env)
{
    return car(car(arguments));
}

object *cdr_proc(object *arguments, object *env)
{
    return cdr(car(arguments));
}

object *set_car_proc(object *arguments, object *env)
{
    set_car(car(arguments), cadr(arguments));
    return ok_symbol;
}

object *set_cdr_proc(object *arguments, object *env)
{
    set_cdr(car(arguments), cadr(arguments));
    return ok_symbol;
}

object *is_pair_proc(object *arguments, object *env)
{
    return is_pair_object(car(arguments)) ? true : false;
}

object *is_boolean_proc(object *arguments, object *env)
{
    return is_boolean_object(car(arguments)) ? true : false;
}

object *length_proc(object *arguments, object *env)
{
    long result = 0;

    if (_number_of_args(arguments) != 1)
    {
        return make_error("'length' takes exactly 1 argument (PAIR).");
    }

    if ( is_the_empty_list(car(arguments)))
    {
        return 0;
    }

    if ( ! is_pair_object(car(arguments)))
    {
        /* We need a pair object to count */
        return make_error("'length' needs list as argument.");
    }

    /* Count elements */
    arguments = car(arguments);
    while ( ! is_the_empty_list(arguments))
    {
        ++result; arguments = cdr(arguments);
    }
    return make_fixnum(result);
}

object *add_proc(object *arguments, object *env)
{
    object *result = make_fixnum(0);

    if (_number_of_args(arguments) == 0)
    {
        return make_error("Arguments missing");
    }
    while ( ! is_the_empty_list(arguments))
    {
        object *summand = car(arguments);
        switch (summand->type)
        {
            case FIXNUM:
                result = add_fixnum_value(result, summand);
                break;
            case REALNUM:
                result = add_realnum_value(result, summand);
                break;
            case COMPLEXNUM:
                result = add_complexnum_value(result, summand);
                break;
            default:
                return make_error("Trying to add illegal type.");
        }
        arguments = cdr(arguments);
    }
    return result;
}

object *sub_proc(object *arguments, object *env)
{
    char need_realnum = 0;
    char first_value = 1;
    double result = 0;
    if (_number_of_args(arguments) == 0)
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

object *mul_proc(object *arguments, object *env)
{
    object *result = make_fixnum(1);

    if (_number_of_args(arguments) == 0)
    {
        return make_error("Arguments missing");
    }
    while ( ! is_the_empty_list(arguments))
    {
        object *factor = car(arguments);
        switch (factor->type)
        {
            case FIXNUM:
                result = mul_fixnum_value(result, factor);
                break;
            case REALNUM:
                result = mul_realnum_value(result, factor);
                break;
            case COMPLEXNUM:
                result = mul_complexnum_value(result, factor);
                break;
            default:
                return make_error("Unknow type in multiplication.");
        }
        arguments = cdr(arguments);
    }

    return result;
}

object *quotient_proc(object *arguments, object *env)
{
    char need_realnum = 0;
    double result, value1, value2;
    if (_number_of_args(arguments) != 2)
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

object *remainder_proc(object *arguments, object *env)
{
    char need_realnum = 0;
    double result, value1, value2;
    if (_number_of_args(arguments) != 2)
    {
        return make_error("'remainder' needs exactly 2 arguments.");
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
    result = fmod(value1, value2);
    return need_realnum ? make_realnum(result) : make_fixnum(result);
}

object *modulo_proc(object *arguments, object *env)
{
    long result, value1, value2;
    if (_number_of_args(arguments) != 2)
    {
        return make_error("'modulo' needs exactly 2 arguments.");
    }
    if ( ! is_fixnum_object(car(arguments)) 
            || ! is_fixnum_object(cadr(arguments)))
    {
        return make_error("'modulo' needs integer arguments.");
    }

    value1 = get_fixnum_value(car(arguments));
    value2 = get_fixnum_value(cadr(arguments));
    result = value1 % value2;
    return make_fixnum(result);
}

object *floor_proc(object *arguments, object *env)
{
    if (is_fixnum_object(car(arguments)))
    {
        return car(arguments);
    }
    else
    {
        return make_fixnum(floor(get_realnum_value(car(arguments))));
    }
}

object *make_rectangular_proc(object *arguments, object *env)
{
    double real, imag;

    if (_number_of_args(arguments) != 2)
    {
        return make_error("Wrong number of arguments.");
    }
    if (! is_realnum_object(car(arguments)) 
            || ! is_realnum_object(cadr(arguments)))
    {
        return make_error("Wrong argument type. Need REALNUM.");
    }

    real = get_realnum_value(car(arguments));
    imag = get_realnum_value(cadr(arguments));
    return make_complexnum(real, imag);
}

object *magnitude_proc(object *arguments, object *env)
{
    double real, imag;
    if (_number_of_args(arguments) != 1)
    {
        return make_error("Wrong number of arguments.");
    }
    if (is_complexnum_object(car(arguments)))
    {
        real = get_complexnum_real_value(car(arguments));
        imag = get_complexnum_imag_value(car(arguments));
        return make_realnum(sqrt((real * real) + (imag * imag)));
    }
    else if (is_fixnum_object(car(arguments)))
    {
        long number = get_fixnum_value(car(arguments));
        return make_fixnum(labs(number));
    }
    else
    {
        return make_error("Wrong argument type.");
    }
}

object *mem_usage_proc(object *obj, object *env)
{
    return make_fixnum(memory_usage());
}

object *is_number_proc(object *arguments, object *env)
{
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Too many arguments to 'number?'.");
    }

    return is_true(is_real_proc(arguments, env))
        || is_true(is_integer_proc(arguments, env))
        || is_true(is_complex_proc(arguments, env))
        ? true : false;
}

object *is_complex_proc(object *arguments, object *env)
{
    if (_number_of_args(arguments) != 1)
    {
        return make_error("Wrong number of arguments.");
    }
    return is_complexnum_object(car(arguments)) ? true : false;
}

object *is_real_proc(object *arguments, object *env)
{
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Too many arguments to 'number?'.");
    }

    return is_realnum_object(car(arguments)) ? true : false;
}

object *is_rational_proc(object *arguments, object *env)
{
    UNUSED(arguments);
    return make_error("'rational?' not implemented yet.");
}

object *is_integer_proc(object *arguments, object *env)
{
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Too many arguments to 'number?'.");
    }

    return is_fixnum_object(car(arguments)) ? true : false;
}

object *is_eqv_proc(object *arguments, object *env)
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
            return is_symbol_equal_proc(arguments, env);
        case STRING:
            return is_string_equal_proc(arguments, env);
        case CHARACTER:
            return is_character_equal_proc(arguments, env);
        case FIXNUM:
        case REALNUM:
        case COMPLEXNUM:
            return is_number_equal_proc(arguments, env);
        default:
            return (obj1 == obj2) ? true : false;
    }
}

object *is_eq_proc(object *arguments, object *env)
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
            return get_symbol_value(obj1) == get_symbol_value(obj2) 
                ? true : false;
        case STRING:
            return get_string_value(obj1) == get_string_value(obj2)
                ? true : false;
        case CHARACTER:
            return is_character_equal_proc(arguments, env);
        case FIXNUM:
        case REALNUM:
        case COMPLEXNUM:
            return is_number_equal_proc(arguments, env);
        default:
            return (obj1 == obj2) ? true : false;
    }
}

object *is_symbol_proc(object *arguments, object *env)
{
    if (_number_of_args(arguments) != 1)
    {
        return make_error("'symbol?' takes exactly 1 argument, %d given.",
                          _number_of_args(arguments));
    }
    
    return is_symbol_object(car(arguments)) ? true : false;
}

object *is_symbol_equal_proc(object *arguments, object *env)
{
    object *obj1 = car(arguments);
    object *obj2 = cadr(arguments);

    if ( ! is_symbol_object(obj1) || ! is_symbol_object(obj2))
    {
        return false;
    }
    return (obj1 == obj2) ? true : false;
}

object *is_string_equal_proc(object *arguments, object *env)
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

object *is_character_equal_proc(object *arguments, object *env)
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

object *is_number_equal_proc(object *arguments, object *env)
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

object *is_number_lt_proc(object *arguments, object *env)
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

    return (value1 < value2) ? true : false;
}

object *is_number_gt_proc(object *arguments, object *env)
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

    return (value1 > value2) ? true : false;
}

object *is_number_lteq_proc(object *arguments, object *env)
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

    return (value1 <= value2) ? true : false;
}

object *is_number_gteq_proc(object *arguments, object *env)
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

    return (value1 >= value2) ? true : false;
}

object *load_proc(object *arguments, object *env)
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
    in = fopen(filename, "r+");
    if (in == NULL)
    {
        return make_error("Unable to open file '%s' (%s).", 
                filename, strerror(errno));
    }

    while (1)
    {
        exp = scheme_read(in);
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

object *open_input_file_proc(object *arguments, object *env)
{
    FILE * input_file;
    const char * filename = get_string_value(car(arguments));

    input_file = fopen(filename, "r+");
    if ( ! input_file)
    {
        return make_error("Unable to open input file '%s'. (%s)"
                , filename
                , strerror(errno));
    }
    return make_input_port(input_file);
}

object *open_output_file_proc(object *arguments, object *env)
{
    FILE * output_file;
    const char * filename = get_string_value(car(arguments));

    output_file = fopen(filename, "w+");
    if ( ! output_file)
    {
        return make_error("Unable to open output file '%s'. (%s)"
                , filename
                , strerror(errno));
    }
    return make_output_port(output_file);
}

object *is_input_port_proc(object *arguments, object *env)
{
    return is_input_port_object(car(arguments)) ? true : false;
}

object *is_output_port_proc(object *arguments, object *env)
{
    return is_output_port_object(car(arguments)) ? true : false;
}

object *is_string_proc(object *arguments, object *env)
{
    if (is_the_empty_list(arguments) || ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    return is_string_object(car(arguments)) ? true : false;
}

object *make_string_proc(object *arguments, object *env)
{
    long length = 0;
    char initializer = ' ';
    char *str;
    object *obj;

    if (is_the_empty_list(arguments))
    {
        return make_error("Wrong number of arguments.");
    }
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        if ( ! is_character_object(cadr(arguments)))
        {
            return make_error("Wrong type for string initializer.");
        }
        initializer = get_character_value(cadr(arguments));
    }
    if ( ! is_fixnum_object(car(arguments)))
    {
        return make_error("Wrong argument type.");
    }
    length = get_fixnum_value(car(arguments));

    str = malloc(length + 1);
    if ( ! str)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    memset(str, initializer, length);
    str[length + 1] = '\0';
    obj = make_string(str);
    free(str);
    return obj;
}

object *string_length_proc(object *arguments, object *env)
{
    if (is_the_empty_list(arguments))
    {
        return make_error("Wrong number of arguments.");
    }
    if ( ! is_string_object(car(arguments)))
    {
        return make_error("Wrong argument type.");
    }

    return make_fixnum(strlen(get_string_value(car(arguments))));
}

object *string_ref_proc(object *arguments, object *env)
{
    long length, index;
    const char * str;

    if (is_the_empty_list(arguments) || is_the_empty_list(cdr(arguments))
        || ! is_the_empty_list(cddr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    if ( ! is_string_object(car(arguments)))
    {
        return make_error("Wrong type for argument 1. Should be STRING.");
    }
    if ( ! is_fixnum_object(cadr(arguments)))
    {
        return make_error("Wrong type for argument 2. Should be INTEGER.");
    }

    str = get_string_value(car(arguments));
    length = strlen(str);
    index = get_fixnum_value(cadr(arguments));

    if (index >= length)
    {
        return make_error("Illegal index to string.");
    }

    return make_character(str[index]);
}

object *string_set_proc(object *arguments, object *env)
{
    long length, index;
    char * str;
    char c;

    if (is_the_empty_list(arguments) 
            || is_the_empty_list(cdr(arguments))
            || is_the_empty_list(cddr(arguments))
            || ! is_the_empty_list(cdddr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    if ( ! is_string_object(car(arguments)))
    {
        return make_error("Wrong type for argument 1. Should be STRING.");
    }
    if ( ! is_fixnum_object(cadr(arguments)))
    {
        return make_error("Wrong type for argument 2. Should be INTEGER.");
    }
    if ( ! is_character_object(caddr(arguments)))
    {
        return make_error("Wrong type for argument 2. Should be CHARACTER.");
    }

    str = get_string_value(car(arguments));
    length = strlen(str);
    index = get_fixnum_value(cadr(arguments));
    c = get_character_value(caddr(arguments));

    if (index >= length)
    {
        return make_error("Illegal index to string.");
    }

    str[index] = c;
    return ok_symbol;
}

object *is_char_proc(object *arguments, object *env)
{
    if (is_the_empty_list(arguments) || ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    return is_character_object(car(arguments)) ? true : false;
}

object *number_to_string_proc(object *arguments, object *env)
{
    char buffer[1024];

    if (_number_of_args(arguments) != 1)
    {
        return make_error("Wrong number of arguments");
    }

    if (is_fixnum_object(car(arguments)))
    {
        long value = get_fixnum_value(car(arguments));
        sprintf(buffer, "%ld", value);
    }
    else if (is_realnum_object(car(arguments)))
    {
        double value = get_realnum_value(car(arguments));
        sprintf(buffer, "%f", value);
    }
    else if (is_complexnum_object(car(arguments)))
    {
        double real, imag;
        real = get_complexnum_real_value(car(arguments));
        imag = get_complexnum_imag_value(car(arguments));
        sprintf(buffer, "%f%+fi", real, imag);
    }
    else
    {
        return make_error("Wrong argument type.");
    }
    return make_string(buffer);
}

object *list_to_string_proc(object *arguments, object *env)
{
    char *str = "";
    long list_length;
    object *list;

    if (_number_of_args(arguments) != 1)
    {
        return make_error("'list->string' takes exactly 1 argument.");
    }
    
    if ( ! is_pair_object(car(arguments))
         && ! is_the_empty_list(car(arguments)))
    {
        return make_error("'list->string' expects PAIR as first argument.");
    }
    
    list = car(arguments);
    list_length = get_fixnum_value(length_proc(arguments, env));

    if (list_length > 0)
    {
        long i = 0;
        str = GC_malloc(list_length + 1);
        memset(str, 0, list_length + 1);

        while ( ! is_the_empty_list(list))
        {
            object *c = car(list);
            if ( ! is_character_object(c))
            {
                return make_error("'list->string' supplied PAIR object "
                                  "contains non-CHARACTER objects.");
            }
            str[i] = get_character_value(c);
            list = cdr(list);
            ++i;
        }
    }
    
    return make_string(str);
}

object *string_to_list_proc(object *arguments, object *env)
{
    UNUSED(env);
    object *obj = the_empty_list;
    char * str;
    int i;
    
    if (_number_of_args(arguments) != 1)
    {
        return make_error("'string->list' takes exactly 1 argument.");
    }
    
    if ( ! is_string_object(car(arguments)))
    {
        return make_error("'string->list' expects STRING as first argument.");
    }
    
    str = get_string_value(car(arguments));

    for (i = strlen(str) - 1; i >= 0; --i)
    {
        obj = cons(make_character(str[i]), obj);
    }

    return obj;
}

object *char_to_int_proc(object *arguments, object *env)
{
    char c;
    if (is_the_empty_list(arguments) || ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    if ( ! is_character_object(car(arguments)))
    {
        return make_error("Wrong argument type.");
    }
    c = get_character_value(car(arguments));
    return make_fixnum(c);
}

object *int_to_char_proc(object *arguments, object *env)
{
    int c;
    if (is_the_empty_list(arguments) || ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    if ( ! is_fixnum_object(car(arguments)))
    {
        return make_error("Wrong argument type.");
    }
    c = get_fixnum_value(car(arguments));
    return make_character(c);
}

object *error_proc(object *arguments, object *env)
{
    while ( ! is_the_empty_list(arguments))
    {
        scheme_write(stderr, car(arguments));
        fprintf(stderr, " ");
        arguments = cdr(arguments);
    }

    fprintf(stderr, "\n");
    exit(1);
}

object *quit_proc(object *arguments, object *env)
{
    UNUSED(arguments);
    exit(0);
}

object *exit_proc(object *arguments, object *env)
{
    exit(get_fixnum_value(car(arguments)));
}

void _pretty_print(object *arguments, char *old_indent, char is_last)
{
    char indent[4096];
    memset(indent, 0x00, 4096);
    strcpy(indent, old_indent);

    fprintf(stdout, "%s", indent);

    if (is_last)
    {
        fprintf(stdout, "\\-");
        memcpy(indent + strlen(indent), "| ", 2);
    }
    else
    {
        fprintf(stdout, "|-");
        memcpy(indent + strlen(indent), "| ", 2);
    }

    switch (arguments->type)
    {
        case FIXNUM:
            fprintf(stdout, "FIXNUM: %ld\n", get_fixnum_value(arguments));
            break;
        case REALNUM:
            fprintf(stdout, "REALNUM: %f\n", get_realnum_value(arguments));
            break;
        case COMPLEXNUM:
            fprintf(stdout, "COMPLEXNUM: %f%+fi\n",
                    get_complexnum_real_value(arguments),
                    get_complexnum_imag_value(arguments));
            break;
        case BOOLEAN:
            fprintf(stdout, "BOOLEAN: #\\%c\n", 
                    (arguments == true) ? 't' : 'f');
            break;
        case CHARACTER:
            fprintf(stdout, "CHARACTER: %c\n", get_character_value(arguments));
            break;
        case STRING:
            fprintf(stdout, "STRING: \"%s\"\n", get_string_value(arguments));
            break;
        case PRIMITIVE_PROC:
            fprintf(stdout, "PRIMITIVE_PROC\n");
            break;
        case COMPOUND_PROC:
            fprintf(stdout, "COMPOUND_PROC\n");
            _pretty_print(arguments->data.compound_proc.parameters, indent, 0);
            _pretty_print(arguments->data.compound_proc.body, indent, 1);
            break;
        case VECTOR:
            fprintf(stdout, "VECTOR\n");
            break;
        case PAIR:
            fprintf(stdout, "PAIR\n");
            _pretty_print(car(arguments), indent, 1);
            _pretty_print(cdr(arguments), old_indent, 0);
            break;
        case SYMBOL:
            fprintf(stdout, "SYMBOL: %s\n", get_symbol_value(arguments));
            break;
        case THE_EMPTY_LIST:
            fprintf(stdout, "'()\n");
            break;
        case ERROR:
            fprintf(stdout, "ERROR: %s\n", get_error_message(arguments));
            break;
        case INPUT_PORT:
            fprintf(stdout, "INPUT_PORT\n");
            break;
        case OUTPUT_PORT:
            fprintf(stdout, "OUTPUT_PORT\n");
            break;
        case SOCKET:
            fprintf(stdout, "SOCKET\n");
            break;
        case RE_PATTERN:
            fprintf(stdout, "RE_PATTERN: \"%s\"\n", 
                    get_re_pattern_string(arguments));
            break;
        case END_OF_FILE:
            fprintf(stdout, "EOF\n");
            break;
    }
}

object *pretty_print_structure_proc(object *arguments, object *env)
{
    _pretty_print(car(arguments), "", 0);
    return ok_symbol;
}

object *load_dynlib_proc(object *arguments, object *env)
{
    long argnr = _number_of_args(arguments);
    object *filename;
    object *verbose = true;
    void *handle;
    char *error;
    object *(*init_fn)(void);

    if (argnr == 0 || argnr > 2)
    {
        return make_error("'load-dynlib' takes at least 1 argument and at most 2.");
    }
    
    filename = car(arguments);
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        verbose = cadr(arguments);
    }
    
    UNUSED(verbose);
    
    handle = dlopen(get_string_value(filename), RTLD_NOW);
    if ( ! handle)
    {
        fprintf(stderr, 
                "Unable to load dynamic library '%s', Error: %s\n", 
                get_string_value(filename), dlerror());
        return false;
    }
    
    init_fn = dlsym(handle, "init_dynlib");
    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, 
                "Unable to initialize dynamic library '%s', Error: %s\n", 
                get_string_value(filename), error);
        return false;
    }
    
    return init_fn();
}

object *re_pattern_proc(object *arguments, object *env)
{
    object *arg = car(arguments);

    if ( _number_of_args(arguments) != 1)
    {
        return make_error("'re-pattern' takes exactly 1 argument.");
    }

    if ( ! is_string_object(arg))
    {
        return make_error("'re-pattern' needs a STRING as argument.");
    }

    return make_re_pattern(get_string_value(arg));
}

object *call_re_pattern_proc(char *pattern, object *env)
{
    return re_pattern_proc(cons(make_string(pattern), the_empty_list), env);
}

object *re_match_proc(object *arguments, object *env)
{
#define OVECCOUNT 30
    object *result = the_empty_list;
    object *re_pattern;
    char   *input;
    int rc, i;
    int ovector[OVECCOUNT];
    
    if ( _number_of_args(arguments) != 2)
    {
        return make_error("'re-match' expects 2 arguments.");
    }
    
    if ( ! is_string_object(car(arguments)) 
         && ! is_re_pattern_object(car(arguments)))
    {
        return make_error("'re-match' expects a STRING or a RE_PATTERN "
                          "as first argument.");
    }
    
    if ( ! is_string_object(cadr(arguments)))
    {
        return make_error("'re-match' expects a STRING as second argument.");
    }

    if (is_string_object(car(arguments)))
    {
        char *pattern = get_string_value(car(arguments));
        re_pattern = call_re_pattern_proc(pattern, env);
    }
    else
    {
        re_pattern = car(arguments);
    }
    
    input   = get_string_value(cadr(arguments));
    
    rc = pcre_exec(get_re_pattern_value(re_pattern), NULL, input, 
                   strlen(input), 0, 0, ovector, OVECCOUNT);

    if (rc < 0)
    {
        switch (rc)
        {
            case PCRE_ERROR_NOMATCH:
                return result; /* shortcut, skip the rest of the func */

            default:
                return make_error("Matching error: %d", rc);
        }
    }
    
    if (rc == 0)
    {
        rc = OVECCOUNT/3;
        /* printf("ovector only has room for %d "
           "captured substrings\n", rc - 1); */
    }

    for (i = rc-1; i >= 0; i--)
    {
        char *stringcopy;
        char *substring_start = input + ovector[2*i];
        int substring_length = ovector[2*i+1] - ovector[2*i];
        /* printf("%2d: %.*s\n", i, substring_length, substring_start); */

        stringcopy = GC_malloc(substring_length + 1);
        if ( ! stringcopy)
        {
            fprintf(stderr, "Out of memory.\n");
            exit(1);
        }
        strncpy(stringcopy, substring_start, substring_length);
        result = cons(make_string(stringcopy), result);
        GC_free(stringcopy); /* Explicitly collect the GC object */
    }
    
    return result;
}

object *is_vector_proc(object *arguments, object *env)
{
    if (_number_of_args(arguments) != 1)
    {
        return make_error("'vector?' takes exactly 1 argument.");
    }
    
    return is_vector_object(car(arguments)) ? true : false;
}

object *make_vector_proc(object *arguments, object *env)
{
    long arity = _number_of_args(arguments);
    long size;
    object *vector;

    if (arity != 1 && arity != 2)
    {
        return make_error("'make-vector' takes 1 or 2 argument.");
    }
    
    if ( ! is_fixnum_object(car(arguments)))
    {
        return make_error("'make-vector' needs a fixnum as the "
                          "first argument.");
    }

    size = get_fixnum_value(car(arguments));
    vector = make_vector(size);

    if (arity == 2)
    {
        object *filler = cadr(arguments);
        long i;
        for (i = 0; i < size; ++i)
        {
            set_vector_item(vector, i, filler);
        }
    }
    
    return vector;
}

object *vector_length_proc(object *arguments, object *env)
{
    long arity = _number_of_args(arguments);

    if (arity != 1)
    {
        return make_error("'vector-length' takes exactly 1 argument.");
    }
    
    return vector_length(car(arguments));
}

object *vector_ref_proc(object *arguments, object *env)
{
    long arity = _number_of_args(arguments);

    if (arity != 2)
    {
        return make_error("'vector-length' takes exactly 2 argument.");
    }
    
    return get_vector_item(car(arguments), get_fixnum_value(cadr(arguments)));
}

object *vector_set_proc(object *arguments, object *env)
{
    long arity = _number_of_args(arguments);

    if (arity != 3)
    {
        return make_error("'vector-length' takes exactly 3 argument.");
    }
    
    return set_vector_item(car(arguments), get_fixnum_value(cadr(arguments)),
                           caddr(arguments));
}

object *make_socket_proc(object *arguments, object *env)
{
    if (_number_of_args(arguments) != 0)
    {
        return make_error("'make-socket' takes no arguments.");
    }
    
    return make_socket();
}

object *socket_bind_proc(object *arguments, object *env)
{
    object *socket, *port;
    struct sockaddr_in serv_addr;
    long portno;

    if (_number_of_args(arguments) != 2)
    {
        return make_error("'socket-bind' expects 2 arguments.");
    }
    
    socket = car(arguments);
    port = cadr(arguments);
    
    if ( ! is_socket_object(socket))
    {
        return make_error("'socket-bind' expects a SOCKET as first argument.");
    }
    
    if ( ! is_fixnum_object(port))
    {
        return make_error("'socket-bind' expects a FIXNUM as second argument'");
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = get_fixnum_value(port);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(socket->data.socket.fd, 
             (struct sockaddr *) &serv_addr, 
             sizeof(serv_addr)) < 0) 
    {
        return make_error("'socket-bind' error: %s",
                          strerror(errno));
    }
    
    return ok_symbol;
}

object *socket_listen_proc(object *arguments, object *env)
{
    object *socket, *backlog;

    if (_number_of_args(arguments) != 2)
    {
        return make_error("'socket-listen' expects 2 arguments.");
    }
    
    socket = car(arguments);
    backlog = cadr(arguments);

    if ( ! is_socket_object(socket))
    {
        return make_error("'socket-listen' expects a SOCKET as "
                          "first argument.");
    }

    if ( ! is_fixnum_object(backlog))
    {
        return make_error("'socket-listen' expects a FIXNUM as "
                          "second argument.");
    }
    
    if (listen(get_socket_fd(socket), get_fixnum_value(backlog)) == -1)
    {
        return make_error("'socket-listen' error: %s",
                          strerror(errno));
    }

    return ok_symbol;
}

object *socket_accept_proc(object *arguments, object *env)
{
    object *socket;
    socklen_t clilen;
    int newsockfd;
    struct sockaddr_in cli_addr;

    if (_number_of_args(arguments) != 1)
    {
        return make_error("'socket-accept' expects 1 argument.");
    }
    
    socket = car(arguments);
    
    if ( ! is_socket_object(socket))
    {
        return make_error("'socket-listen' expects a SOCKET as "
                          "first argument.");
    }

    clilen = sizeof(cli_addr);
   /* Accept actual connection from the client */
    newsockfd = accept(get_socket_fd(socket), 
                       (struct sockaddr *)&cli_addr, 
                       &clilen);
	
    if (newsockfd < 0) 
    {
        return make_error("'socket-listen' error: %s",
                          strerror(errno));
    }
    
    return make_socket_from_fd(newsockfd);
}

object *close_socket_proc(object *arguments, object *env)
{
    object *socket;

    if (_number_of_args(arguments) != 1)
    {
        return make_error("'socket-close' takes exactly 1 argument.");
    }
    
    socket = car(arguments);
    
    if ( ! is_socket_object(socket))
    {
        return make_error("'socket-close' expects a SOCKET as first argument.");
    }
    
    close_socket(socket);
    return ok_symbol;
}

object *is_socket_proc(object *arguments, object *env)
{
    object *socket;

    if (_number_of_args(arguments) != 1)
    {
        return make_error("'socket?' takes exactly 1 argument.");
    }
    
    socket = car(arguments);
    
    return is_socket_object(socket) ? true : false;
}

object *select_proc(object *arguments, object *env)
{
    object *rd_list, *wr_list, *err_list;
    fd_set rd_set, wr_set, err_set;
    long timeout;
    struct timeval timeout_s;
    int max_fds = 0;
    object *rd_results = the_empty_list;
    object *wr_results = the_empty_list;
    object *err_results = the_empty_list;
    object *results = the_empty_list;
    
    /* Check for proper number and types of arguments */
    if (_number_of_args(arguments) != 4)
    {
        return make_error("'select' takes exactly 4 arguments.");
    }
    
    if ( ! is_pair_object(car(arguments))
         && ! is_the_empty_list(car(arguments)))
    {
        return make_error("'select' expects a PAIR as first argument.");
    }
    
    if ( ! is_pair_object(cadr(arguments))
         && ! is_the_empty_list(cadr(arguments)))
    {
        return make_error("'select' expects a PAIR as second argument.");
    }

    if ( ! is_pair_object(caddr(arguments))
         && ! is_the_empty_list(caddr(arguments)))
    {
        return make_error("'select' expects a PAIR as third argument.");
    }

    if ( ! is_fixnum_object(cadddr(arguments)))
    {
        return make_error("'select' expects a FIXNUM as fourth argument.");
    }

    /* Clear the sets */
    FD_ZERO(&rd_set);
    FD_ZERO(&wr_set);
    FD_ZERO(&err_set);
    
    /* Read args */
    rd_list = car(arguments);
    wr_list = cadr(arguments);
    err_list = caddr(arguments);
    timeout = get_fixnum_value(cadddr(arguments));
    
    timeout_s.tv_sec = 0;
    timeout_s.tv_usec = timeout;
    
    /* Set the FD_SET's */
    {
        object *args_list[3] = {rd_list, wr_list, err_list};
        fd_set *sets_list[3] = {&rd_set, &wr_set, &err_set};
        unsigned long i;

        for (i = 0; i < sizeof(sets_list) / sizeof(fd_set *); ++i)
        {
            while (args_list[i] != the_empty_list)
            {
                int fd;

                /* Evaluate the object, it might be a symbol */
                set_car(args_list[i], eval(car(args_list[i]), env));
                fd = get_socket_fd(car(args_list[i]));
                if (fd > max_fds) max_fds = fd;
                FD_SET(fd, sets_list[i]);
                args_list[i] = cdr(args_list[i]);
            }
        }
        max_fds += 1;
    }

    if (select(max_fds, &rd_set, &wr_set, &err_set, &timeout_s) < 0)
    {
        return make_error("'select' error: %s", strerror(errno));
    }
    
    /* Read the results */
    {
        object *args_list[3]      = {rd_list, wr_list, err_list};
        object **results_list[3]  = {&rd_results, &wr_results, &err_results};
        fd_set *sets_list[3]      = {&rd_set, &wr_set, &err_set};
        unsigned long i;
        int fd;

        for (i = 0; i < sizeof(sets_list) / sizeof(fd_set *); ++i)
        {
            for (fd = 0; fd < max_fds; ++fd)
            {
                if (FD_ISSET(fd, sets_list[i]))
                {
                    /* File descriptor is set,
                       find the proper stream in our list
                       of the original arguments */
                    object *list = args_list[i];
                    while ( ! is_the_empty_list(list))
                    {
                        if (get_socket_fd(car(list)) == fd)
                        {
                            *results_list[i] = cons(car(list), 
                                                    *results_list[i]);
                        }
                        list = cdr(list);
                    }
                }
            }
        }
    }

    results = cons(rd_results,
                   cons(wr_results,
                        cons(err_results, the_empty_list)));
    return results;
}

object *sleep_proc(object *arguments, object *env)
{
    unsigned int secs;
    UNUSED(env);
    if (_number_of_args(arguments) != 1)
    {
        return make_error("'sleep' takes exactly 1 argument");
    }
    
    if (is_fixnum_object(car(arguments)))
    {
        secs = get_fixnum_value(car(arguments)) * 1000000;
    }
    else if (is_realnum_object(car(arguments)))
    {
        secs = floor(get_realnum_value(car(arguments)) * 1000000);
    }
    else
    {
        return make_error("'sleep' expects a FIXNUM/REAL as first argument.");
    }
    
    usleep(secs);
    
    return ok_symbol;
}

object *write_char_proc(object *arguments, object *env)
{
    object *character;
    object *port = find_variable(current_output_port_symbol, env);
    int nr_args = _number_of_args(arguments);

    if (nr_args != 1 && nr_args != 2)
    {
        return make_error("'write-char' takes 1 or 2 arguments.");
    }

    character = eval(car(arguments), env);

    if(cdr(arguments) != the_empty_list)
    {
        port = eval(cadr(arguments), env);
    }
    
    if ( ! is_character_object(character))
    {
        return make_error("'write-char' expects a CHARACTER as "
                          "first argument.");
    }

    if (is_output_port_object(port))
    {
        char c = get_character_value(character);
        fwrite(&c, 1, 1, get_output_port_stream(port));
    }
    else if (is_socket_object(port))
    {
        char c = get_character_value(character);
        write(get_socket_fd(port), &c, 1); 
    }
    else
    {
        return make_error("'write-char' expects a OUTPUT_PORT or "
                          "STREAM as second argument.");
    }

    return ok_symbol;
}

object *read_char_proc(object *arguments, object *env)
{
    char c;
    object *port = find_variable(current_input_port_symbol, env);
    int nr_args = _number_of_args(arguments);

    if (nr_args != 0 && nr_args != 1)
    {
        return make_error("'read-char' takes 0 or 1 arguments.");
    }
    
    if ( ! is_the_empty_list(arguments))
    {
        port = eval(car(arguments), env);
    }

    if (is_input_port_object(port))
    {
        fread(&c, 1, 1, get_input_port_stream(port));
    }
    else if (is_socket_object(port))
    {
        ssize_t result = read(get_socket_fd(port), &c, 1);

        if (result < 0)
        {
            return make_error("'read-char' error: %s", strerror(errno));
        }
    }
    else
    {
        return make_error("'read-char' expects a INPUT_PORT or "
                          "STREAM as first argument.");
    }

    return make_character(c);;
}

object *write_proc(object *arguments, object *env)
{
    object *port = find_variable(current_output_port_symbol, env);
    int nr_args = _number_of_args(arguments);

    if (nr_args != 1 && nr_args != 2)
    {
        return make_error("'write' takes 1 or 2 arguments.");
    }

    if(cdr(arguments) != the_empty_list)
    {
        port = eval(cadr(arguments), env);
    }
    
    if (is_output_port_object(port))
    {
        scheme_write(get_output_port_stream(port), car(arguments));
        putc('\n', get_output_port_stream(port));
    }
    else if (is_socket_object(port))
    {
        return make_error("'write' does not support SOCKET objects yet.");
    }
    else
    {
        return make_error("'write' expects a OUTPUT_PORT or "
                          "SOCKET as second argument.");
    }

    return ok_symbol;
}

object *read_proc(object *arguments, object *env)
{
    object *port = find_variable(current_input_port_symbol, env);
    int nr_args = _number_of_args(arguments);

    if (nr_args != 0 && nr_args != 1)
    {
        return make_error("'read' takes 0 or 1 arguments.");
    }
    
    if ( ! is_the_empty_list(arguments))
    {
        port = eval(car(arguments), env);
    }

    if (is_input_port_object(port))
    {
        return scheme_read(get_input_port_stream(port));
    }
    else if (is_socket_object(port))
    {
        return make_error("'read' does not support SOCKET objects yet");
    }
    else
    {
        return make_error("'read' expects a INPUT_PORT or "
                          "SOCKET as first argument.");
    }
}
