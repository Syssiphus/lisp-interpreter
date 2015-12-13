
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "globals.h"
#include "builtins.h"
#include "memory.h"
#include "read.h"
#include "eval.h"

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
    return make_error("Not a pair object.");
}

object *cdr(object *obj)
{
    if (is_pair_object(obj))
    {
        return obj->data.pair.cdr;
    }
    return make_error("Not a pair object.");
}

void set_car(object *dst, object *obj)
{
    dst->data.pair.car = obj;
}

void set_cdr(object *dst, object *obj)
{
    dst->data.pair.cdr = obj;
}

object *cons_proc(object *arguments)
{
    object *car_obj = car(arguments);
    object *cdr_obj = cadr(arguments);
    return cons(car_obj, cdr_obj);
}

object *car_proc(object *arguments)
{
    return car(car(arguments));
}

object *cdr_proc(object *arguments)
{
    return cdr(car(arguments));
}

object *set_car_proc(object *arguments)
{
    set_car(car(arguments), cadr(arguments));
    return ok_symbol;
}

object *set_cdr_proc(object *arguments)
{
    set_cdr(car(arguments), cadr(arguments));
    return ok_symbol;
}

object *is_pair_proc(object *arguments)
{
    return is_pair_object(car(arguments)) ? true : false;
}

object *is_boolean_proc(object *arguments)
{
    return is_boolean_object(car(arguments)) ? true : false;
}

object *length_proc(object *arguments)
{
    long result = 0;

    if (_number_of_args(arguments) != 1)
    {
        return make_error("'length' takes exactly 1 argument (PAIR).");
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

object *add_proc(object *arguments)
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

object *sub_proc(object *arguments)
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

object *mul_proc(object *arguments)
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

object *quotient_proc(object *arguments)
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

object *remainder_proc(object *arguments)
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

object *modulo_proc(object *arguments)
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

object *floor_proc(object *arguments)
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

object *make_rectangular_proc(object *arguments)
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

object *magnitude_proc(object *arguments)
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

object *mem_usage_proc(object *obj)
{
    return make_fixnum(memory_usage());
}

object *is_number_proc(object *arguments)
{
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Too many arguments to 'number?'.");
    }

    return is_true(is_real_proc(arguments))
        || is_true(is_integer_proc(arguments))
        || is_true(is_complex_proc(arguments))
        ? true : false;
}

object *is_complex_proc(object *arguments)
{
    if (_number_of_args(arguments) != 1)
    {
        return make_error("Wrong number of arguments.");
    }
    return is_complexnum_object(car(arguments)) ? true : false;
}

object *is_real_proc(object *arguments)
{
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Too many arguments to 'number?'.");
    }

    return is_realnum_object(car(arguments)) ? true : false;
}

object *is_rational_proc(object *arguments)
{
    return make_error("'rational?' not implemented yet.");
}

object *is_integer_proc(object *arguments)
{
    if ( ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Too many arguments to 'number?'.");
    }

    return is_fixnum_object(car(arguments)) ? true : false;
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

object *is_number_lt_proc(object *arguments)
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

object *is_number_gt_proc(object *arguments)
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

object *is_number_lteq_proc(object *arguments)
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

object *is_number_gteq_proc(object *arguments)
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

object *open_input_file_proc(object *arguments)
{
    FILE * input_file;
    const char * filename = get_string_value(car(arguments));

    input_file = fopen(filename, "r");
    if ( ! input_file)
    {
        return make_error("Unable to open input file '%s'. (%s)"
                , filename
                , strerror(errno));
    }
    return make_input_port(input_file);
}

object *open_output_file_proc(object *arguments)
{
    FILE * output_file;
    const char * filename = get_string_value(car(arguments));

    output_file = fopen(filename, "w");
    if ( ! output_file)
    {
        return make_error("Unable to open output file '%s'. (%s)"
                , filename
                , strerror(errno));
    }
    return make_output_port(output_file);
}

object *is_input_port_proc(object *arguments)
{
    return is_input_port_object(car(arguments)) ? true : false;
}

object *is_output_port_proc(object *arguments)
{
    return is_output_port_object(car(arguments)) ? true : false;
}

object *is_string_proc(object *arguments)
{
    if (is_the_empty_list(arguments) || ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    return is_string_object(car(arguments)) ? true : false;
}

object *make_string_proc(object *arguments)
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

object *string_length_proc(object *arguments)
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

object *string_ref_proc(object *arguments)
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

object *string_set_proc(object *arguments)
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

object *is_char_proc(object *arguments)
{
    if (is_the_empty_list(arguments) || ! is_the_empty_list(cdr(arguments)))
    {
        return make_error("Wrong number of arguments.");
    }
    return is_character_object(car(arguments)) ? true : false;
}

object *number_to_string_proc(object *arguments)
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

object *char_to_int_proc(object *arguments)
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

object *int_to_char_proc(object *arguments)
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

object *error_proc(object *arguments)
{
    fprintf(stderr, "Error: %s\n", get_string_value(car(arguments)));
    exit(1);
}

object *quit_proc(object *arguments)
{
    exit(0);
}

object *exit_proc(object *arguments)
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
        case END_OF_FILE:
            fprintf(stdout, "EOF\n");
            break;
    }
}

object *pretty_print_structure_proc(object *arguments)
{
    _pretty_print(arguments, "", 0);
    return ok_symbol;
}

