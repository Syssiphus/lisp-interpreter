
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <gc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

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

long get_fixnum_value(object *obj)
{
    if (obj->type != FIXNUM)
    {
        fprintf(stderr, "Object is not a FIXNUM object.\n");
        DEBUG_BREAK;
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

double get_realnum_value(object *obj)
{
    return obj->data.realnum.value;
}

/** ARITHMETICS */
object *add_fixnum_value(object *obj, object *summand)
{
    object *result;
    long x = get_fixnum_value(summand);

    switch(obj->type)
    {
        case FIXNUM:
            result = make_fixnum(get_fixnum_value(obj) + x);
            break;
        case REALNUM:
            result = make_realnum(get_realnum_value(obj) + x);
            break;
        case COMPLEXNUM:
            result = make_complexnum(get_complexnum_real_value(obj) + x,
                                     get_complexnum_imag_value(obj) + x);
            break;
        default:
            return make_error("Wrong argument type.");
    }

    return result;
}

object *add_realnum_value(object *obj, object *summand)
{
    object *result;
    double x = get_realnum_value(summand);

    switch(obj->type)
    {
        case FIXNUM:
            result = make_realnum(get_fixnum_value(obj) + x);
            break;
        case REALNUM:
            result = make_realnum(get_realnum_value(obj) + x);
            break;
        case COMPLEXNUM:
            result = make_complexnum(get_complexnum_real_value(obj) + x,
                                     get_complexnum_imag_value(obj) + x);
            break;
        default:
            return make_error("Wrong argument type.");
    }

    return result;
}

object *add_complexnum_value(object *obj, object *summand)
{
    object *result;
    double x = get_complexnum_real_value(summand);
    double y = get_complexnum_imag_value(summand);

    switch(obj->type)
    {
        case FIXNUM:
            result = make_complexnum(get_fixnum_value(obj) + x,
                                     get_fixnum_value(obj) + y);
            break;
        case REALNUM:
            result = make_complexnum(get_realnum_value(obj) + x,
                                     get_realnum_value(obj) + y);
            break;
        case COMPLEXNUM:
            result = make_complexnum(get_complexnum_real_value(obj) + x,
                                     get_complexnum_imag_value(obj) + y);
            break;
        default:
            return make_error("Wrong argument type.");
    }

    return result;
}

object *mul_fixnum_value(object *obj, object *factor)
{
    object *result;
    long x = get_fixnum_value(factor);

    switch(obj->type)
    {
        case FIXNUM:
            result = make_fixnum(get_fixnum_value(obj) * x);
            break;
        case REALNUM:
            result = make_realnum(get_realnum_value(obj) * x);
            break;
        case COMPLEXNUM:
        {
            double a, b;
            a = get_complexnum_real_value(obj);
            b = get_complexnum_imag_value(obj);
            result = make_complexnum(a * x, b * x); 
            break;
        }
        default:
            return make_error("Wrong argument type.");
    }

    return result;
}

object *mul_realnum_value(object *obj, object *factor)
{
    object *result;
    double x = get_realnum_value(factor);

    switch(obj->type)
    {
        case FIXNUM:
            result = make_realnum(get_fixnum_value(obj) * x);
            break;
        case REALNUM:
            result = make_realnum(get_realnum_value(obj) * x);
            break;
        case COMPLEXNUM:
        {
            double a, b;
            a = get_complexnum_real_value(obj);
            b = get_complexnum_imag_value(obj);
            result = make_complexnum(a * x, b * x); 
            break;
        }
        default:
            return make_error("Wrong argument type.");
    }

    return result;
}

object *mul_complexnum_value(object *obj, object *factor)
{
    object *result;
    double c, d;
    c = get_complexnum_real_value(factor);
    d = get_complexnum_imag_value(factor);
    
    switch (obj->type)
    {
        case FIXNUM:
            result = make_complexnum(get_fixnum_value(obj) * c,
                                     get_fixnum_value(obj) * d);
            break;
        case REALNUM:
            result = make_complexnum(get_realnum_value(obj) * c,
                                     get_realnum_value(obj) * d);
            break;
        case COMPLEXNUM:
        {
            double a, b;
            a = get_complexnum_real_value(obj);
            b = get_complexnum_imag_value(obj);
            result = make_complexnum(((a * c) - (b * d)), ((a * d) + (b * c)));
            break;
        }
        default:
            return make_error("Wrong argument type.");
    }

    return result;
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

int get_character_value(object *obj)
{
    if (obj->type != CHARACTER)
    {
        fprintf(stderr, "Object is not a CHARACTER object.\n");
        DEBUG_BREAK;
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
        DEBUG_BREAK;
    }
    strcpy(obj->data.string.value, str);
    obj->type = STRING;
    return obj;
}

char *get_string_value(object *obj)
{
    return obj->data.string.value;
}

/* Symbols */
object *make_symbol(char *str)
{
    object *obj;

    obj = alloc_object();
    obj->type = SYMBOL;
    obj->data.symbol.value = GC_malloc(strlen(str) + 1);
    if ( ! obj->data.symbol.value)
    {
        fprintf(stderr, "Error allocating string space for symbol string "
                "'%s'.", str);
        DEBUG_BREAK;
    }
    strcpy(obj->data.symbol.value, str);
    obj->data.symbol.size = strlen(obj->data.symbol.value);

    return obj;
}

/** VECTORS */
object *make_vector(size_t size)
{
    object *obj = alloc_object();
    obj->type = VECTOR;
    obj->data.vector.items = GC_malloc(size * sizeof(object *));
    if ( ! obj->data.vector.items)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    obj->data.vector.length = size;
    return obj;
}

object *get_vector_item(object *vector, size_t pos)
{
    if (pos >= vector->data.vector.length)
    {
        return make_error("Illegal index %ld to vector of size %ld.", 
                          pos, vector->data.vector.length);
    }
    return vector->data.vector.items[pos];
}

object *set_vector_item(object *vector, size_t pos, object *obj)
{
    if (pos >= vector->data.vector.length)
    {
        return make_error("Illegal index %ld to vector of size %ld.", 
                          pos, vector->data.vector.length);
    }
    vector->data.vector.items[pos] = obj;
    return ok_symbol;
}

object *vector_length(object *vector)
{
    return make_fixnum(vector->data.vector.length);
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
        DEBUG_BREAK;
    }

    va_start(args, fmt);
    vsnprintf(obj->data.error.message, ERROR_MSG_SIZE - 1, fmt, args);
    va_end(args);

    fprintf(stderr, "%s\n", obj->data.error.message);
    DEBUG_BREAK;
    return obj;
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

FILE *get_eof_stream(object *obj)
{
    return obj->data.end_of_file.stream;
}

void close_input_port_on_collect(void *obj, void *arg)
{
    UNUSED(arg);
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
    UNUSED(arg);
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

void close_socket_on_collect(void *obj, void *arg)
{
    UNUSED(arg);
    close_socket((object *)obj);
}

object *make_socket(void)
{
    object *obj;
    
    obj = alloc_with_finalizer(close_socket_on_collect);
    obj->type = SOCKET;
    obj->data.socket.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (obj->data.socket.fd == -1)
    {
        return make_error("Error creating the socket: %s",
                          strerror(errno));
    }
    /* Set the address re-use */
    if (setsockopt(obj->data.socket.fd, SOL_SOCKET, SO_REUSEADDR, 
                   &(int){ 1 }, sizeof(int)) < 0)
    {
        return make_error("Error setting the socket options: %s",
                          strerror(errno));
    }
 
    return obj;
}

object *make_socket_from_fd(int fd)
{
    object *obj;
    
    obj = alloc_with_finalizer(close_socket_on_collect);
    obj->type = SOCKET;
    obj->data.socket.fd = fd;
    
    return obj;
}

int get_socket_fd(object *obj)
{
    return obj->data.socket.fd;
}

void close_socket(object *obj)
{
    if (obj->data.socket.fd != -1)
    {
        if (close(obj->data.socket.fd) == -1)
        {
            fprintf(stderr, "Error while closing socket: %s\n",
                    strerror(errno));
            exit(1);
        }
        obj->data.socket.fd = -1;
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
#ifdef OBJECT_REFERENCES
    obj->data.compound_proc.optimized = 0;
#else
    obj->data.compound_proc.optimized = 1;
#endif
    obj->data.compound_proc.parameters = parameters;
    obj->data.compound_proc.body = body;
    obj->data.compound_proc.env = env;
    return obj;
}

void clean_up_re_pattern(void *obj, void *arg)
{
    object * realobj = (object *)obj;
    UNUSED(arg);

    if(realobj->data.re_pattern.pattern)
    {
        free(realobj->data.re_pattern.pattern);
    }
}

object *make_re_pattern(const char *pattern_string)
{
    object *obj;
    const char * error;
    int erroffset;
    
    obj = alloc_with_finalizer(clean_up_re_pattern);
    obj->type = RE_PATTERN;
    obj->data.re_pattern.pattern_string = GC_malloc(strlen(pattern_string) + 1);
    strcpy(obj->data.re_pattern.pattern_string, pattern_string);
    obj->data.re_pattern.pattern = pcre_compile(pattern_string, 
                                                0, &error, &erroffset, NULL);
    if ( ! obj->data.re_pattern.pattern)
    {
        return make_error("failed to compile regex pattern '%s'.", 
                          pattern_string);
    }
    
    return obj;
}

const char * get_re_pattern_string(object *obj)
{
    return obj->data.re_pattern.pattern_string;
}

pcre *get_re_pattern_value(object *obj)
{
    return obj->data.re_pattern.pattern;
}

object *make_environment(env_entry *entry)
{
    object *obj;

    obj = alloc_object();
    obj->type = ENVIRONMENT;
    obj->data.environment.env = entry;
    return obj;
}

object *make_obj_ref(object *obj)
{
    object *ref = alloc_object();
    ref->type = OBJ_REF;
    ref->data.obj_ref.object = &obj;
    return ref;
}
