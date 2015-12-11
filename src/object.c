
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <gc.h>

#include "globals.h"
#include "object.h"
#include "memory.h"
#include "builtins.h"

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
        __builtin_trap();
    }

    return obj->data.fixnum.value;
}

/** REAL NUMBERS */
object *make_realnum(double num)
{
    object *obj;

    obj = alloc_object();
    obj->type = REALNUM;
    obj->data.realnum.value = num;
    return obj;
}

char is_realnum_object(object *obj)
{
    return obj->type == REALNUM;
}
    
double get_realnum_value(object *obj)
{
    return obj->data.realnum.value;
}

/** COMPLEX NUMBERS */
object *make_complexnum(double real, double imag)
{
    object *obj;

    obj = alloc_object();
    obj->type = COMPLEXNUM;
    obj->data.complexnum.real = real;
    obj->data.complexnum.imag = imag;
    return obj;
}

char is_complexnum_object(object *obj)
{
    return obj->type == COMPLEXNUM;
}

double get_complexnum_real_value(object *obj)
{
    return obj->data.complexnum.real;
}

double get_complexnum_imag_value(object *obj)
{
    return obj->data.complexnum.imag;
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
        __builtin_trap();
    }

    return obj->data.character.value;
}

/** STRINGS */
object *make_string(char *str)
{
    object *obj;

    obj = alloc_object();

    obj->data.string.value = GC_malloc(strlen(str) + 1);
    if ( ! obj->data.string.value)
    {
        fprintf(stderr, "Error allocating string space for string '%s'.", str);
        __builtin_trap();
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

/* Booleans */
char is_boolean_object(object *obj)
{
    return obj->type == BOOLEAN;
}

char is_false(object *obj)
{
    if ( ! is_boolean_object(obj))
    {
        fprintf(stderr, "Not a boolean value.\n");
        __builtin_trap();
    }

    return obj->data.boolean.value == 0;
}

char is_true(object *obj)
{
    return ( ! is_false(obj));
}

/* Symbols */
object *make_symbol(char *str)
{
    object *obj;
    object *temp;

    temp = symbol_table;
    while ( ! is_the_empty_list(temp))
    {
        if (strcmp(car(temp)->data.symbol.value, str) == 0)
        {
            return car(temp);
        }
        temp = cdr(temp);
    }

    obj = alloc_object();
    obj->type = SYMBOL;
    obj->data.symbol.value = GC_malloc(strlen(str) + 1);
    if ( ! obj->data.symbol.value)
    {
        fprintf(stderr, "Error allocating string space for symbol string "
                "'%s'.", str);
        __builtin_trap();
    }
    strcpy(obj->data.symbol.value, str);
    symbol_table = cons(obj, symbol_table);
    return obj;
}

char is_symbol_object(object *obj)
{
    return obj->type == SYMBOL;
}

char *get_symbol_value(object *obj)
{
    if ( ! is_symbol_object(obj))
    {
        fprintf(stderr, "Not a symbol object.\n");
        __builtin_trap();
    }

    return obj->data.symbol.value;
}

/* The empty list '() */
char is_the_empty_list(object *obj)
{
    return obj->type == THE_EMPTY_LIST;
}

/* Pairs */
object *make_pair(object *a, object *b)
{
    object *obj = alloc_object();
    obj->type = PAIR;
    obj->data.pair.car = a;
    obj->data.pair.cdr = b;
    return obj;
}

char is_pair_object(object *obj)
{
    return obj->type == PAIR;
}

/* Errors */
object *make_error(const char *fmt, ...)
{
    va_list args;
#define ERROR_MSG_SIZE 1024
    object *obj = alloc_object();
    obj->type = ERROR;

    obj->data.error.message = GC_malloc(ERROR_MSG_SIZE);
    if ( ! obj->data.error.message)
    {
        fprintf(stderr, "Out of memory in %s().", __func__);
        __builtin_trap();
    }

    va_start(args, fmt);
    vsnprintf(obj->data.error.message, ERROR_MSG_SIZE - 1, fmt, args);
    va_end(args);

    fprintf(stderr, "%s\n", obj->data.error.message);
    __builtin_trap();
}

char is_error_object(object *obj)
{
    return obj->type == ERROR;
}

char *get_error_message(object *obj)
{
    return obj->data.error.message;
}

/* Primitive procedure */
object *make_primitive_proc(primitive_proc_t fn)
{
    object *obj = alloc_object();
    obj->type = PRIMITIVE_PROC;
    obj->data.primitive_proc.fn = fn;
    return obj;
}

char is_primitive_proc_object(object *obj)
{
    return obj->type == PRIMITIVE_PROC;
}

primitive_proc_t get_primitive_proc_value(object *obj)
{
    return obj->data.primitive_proc.fn;
}

object *make_eof(FILE *which)
{
    object *obj = alloc_object();
    obj->type = END_OF_FILE;
    obj->data.end_of_file.stream = which;
    return obj;
}

char is_eof_object(object *obj)
{
    return obj->type == END_OF_FILE;
}

FILE *get_eof_stream(object *obj)
{
    return obj->data.end_of_file.stream;
}

void close_input_port_on_collect(void *obj, void *arg)
{
    close_input_port((object *)obj);
}

object *make_input_port(FILE *in)
{
    object *obj;
    obj = alloc_with_finalizer(close_input_port_on_collect);
    obj->type = INPUT_PORT;
    obj->data.input_port.stream = in;
    return obj;
}

char is_input_port_object(object *obj)
{
    return obj->type == INPUT_PORT;
}

FILE *get_input_port_stream(object *obj)
{
    return obj->data.input_port.stream;
}

void close_input_port(object *obj)
{
    if (obj->data.input_port.stream)
    {
        fclose(obj->data.input_port.stream);
        obj->data.input_port.stream = NULL;
    }
}

void close_output_port_on_collect(void *obj, void *arg)
{
    close_output_port((object *)obj);
}

object *make_output_port(FILE *out)
{
    object *obj;
    obj = alloc_with_finalizer(close_output_port_on_collect);
    obj->type = OUTPUT_PORT;
    obj->data.output_port.stream = out;
    return obj;
}

char is_output_port_object(object *obj)
{
    return obj->type == OUTPUT_PORT;
}

FILE *get_output_port_stream(object *obj)
{
    return obj->data.output_port.stream;
}

void close_output_port(object *obj)
{
    if (obj->data.output_port.stream)
    {
        fclose(obj->data.output_port.stream);
        obj->data.output_port.stream = NULL;
    }
}

object *make_lambda(object *arguments, object *body)
{
    return cons(lambda_symbol, cons(arguments, body));
}

object *make_begin(object *obj)
{
    return cons(begin_symbol, obj);
}

object *make_assignment(object *var, object *value)
{
    return cons(set_symbol, cons(var, cons(value, the_empty_list)));
}

object *make_compound_proc(object *parameters, object *body, object *env)
{
    object *obj = alloc_object();
    obj->type = COMPOUND_PROC;
    obj->data.compound_proc.parameters = parameters;
    obj->data.compound_proc.body = body;
    obj->data.compound_proc.env = env;
    return obj;
}

char is_compound_proc_object(object *obj)
{
    return obj->type == COMPOUND_PROC;
}


