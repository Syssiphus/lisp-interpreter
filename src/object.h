
#pragma once

#include <stdio.h>
#include <pcre.h>

#include "defines.h"

#include "uthash.h"

typedef enum 
{
  FIXNUM         /* Integer number (64 bit) */
, REALNUM        /* Real number (double) */
, COMPLEXNUM     /* Complex number (a+bi)*/
, BOOLEAN        /* Truth type (boolean) */
, CHARACTER      /* Character type */
, STRING         /* String type */
, VECTOR         /* Vector type */
, PAIR           /* Cons cell (Pair type) */
, SYMBOL         /* Symbol type */
, PRIMITIVE_PROC /* Primitive procedure */
, COMPOUND_PROC  /* Compound procedure */
, THE_EMPTY_LIST /* The empty list object '() */
, ERROR          /* Error object */
, INPUT_PORT     /* Input port object */
, OUTPUT_PORT    /* Output port object */
, SOCKET         /* Socket object */
, RE_PATTERN     /* Regular expression pattern */
, END_OF_FILE    /* End of file type (no idea if this is good)*/
, ENVIRONMENT    /* Special type for the environment hash data */
} object_type;

struct object;

typedef struct object *(*primitive_proc_t)(struct object *, struct object *);

typedef struct 
{
    char *symbol;
    struct object *obj;
    UT_hash_handle hh;
} env_entry;

typedef struct object
{
    object_type type;
    char        used;
    char        marked;

    union
    {
        struct 
        {
            long value;
        } fixnum;

        struct
        {
            double value;
        } realnum;

        struct
        {
            double real;
            double imag;
        } complexnum;

        struct
        {
            int value;
        } character;

        struct
        {
            char *value;
        } string;

        struct
        {
            int value;
        } boolean;

        struct
        {
            char   *value;
            size_t size;
        } symbol;

        struct
        {
            struct object **items;
            size_t length;
        } vector;

        struct
        {
            struct object *car;
            struct object *cdr;
        } pair;

        struct
        {
            char * message;
        } error;

        struct
        {
            FILE *stream;
        } input_port;

        struct
        {
            FILE *stream;
        } output_port;

        struct
        {
            int  fd;
        } socket;

        struct
        {
            primitive_proc_t fn;
        } primitive_proc;

        struct
        {
            struct object *parameters;
            struct object *body;
            struct object *env;
        } compound_proc;
        
        struct
        {
            char *pattern_string;
            pcre *pattern;
        } re_pattern;

        struct
        {
            FILE *stream;
        } end_of_file;
        
        struct
        {
            env_entry *env;
        } environment;
    } data;
} object;

object *make_fixnum(long num);
_static_inline_ char is_fixnum_object(object *obj) {return obj->type == FIXNUM;}
long get_fixnum_value(object *obj);

object *make_realnum(double num);
_static_inline_ char 
is_realnum_object(object *obj) {return obj->type == REALNUM;}
double get_realnum_value(object *obj);

object *make_complexnum(double real, double imag);
_static_inline_ char 
is_complexnum_object(object *obj) {return obj->type == COMPLEXNUM;}
double get_complexnum_real_value(object *obj);
double get_complexnum_imag_value(object *obj);

object *add_fixnum_value(object *obj, object *summand);
object *add_realnum_value(object *obj, object *summand);
object *add_complexnum_value(object *obj, object *summand);

object *mul_fixnum_value(object *obj, object *factor);
object *mul_realnum_value(object *obj, object *factor);
object *mul_complexnum_value(object *obj, object *factor);

object *make_character(int c);
_static_inline_ char 
is_character_object(object *obj) {return obj->type == CHARACTER;}
int get_character_value(object *obj);

object *make_string(char *str);
_static_inline_ char is_string_object(object *obj) {return obj->type == STRING;}
char *get_string_value(object *obj);

_static_inline_ char 
is_boolean_object(object *obj) {return obj->type == BOOLEAN;}
_static_inline_ char 
is_false(object *obj) {return obj->data.boolean.value == 0;}
_static_inline_ char is_true(object *obj) {return ( ! is_false(obj));}

object *make_symbol(char *str);
_static_inline_ char is_symbol_object(object *obj) {return obj->type == SYMBOL;}
_static_inline_ char 
*get_symbol_value(object *obj) {return obj->data.symbol.value;}
_static_inline_ size_t 
get_symbol_size(object *obj) {return obj->data.symbol.size;}
_static_inline_ char 
is_the_empty_list(object *obj) {return obj->type == THE_EMPTY_LIST;}

object *make_vector(size_t size);
_static_inline_ char is_vector_object(object *obj) {return obj->type == VECTOR;}
object *get_vector_item(object *vector, size_t pos);
object *set_vector_item(object *vector, size_t pos, object *obj);
object *vector_length(object *vector);

object *make_pair(object *a, object *b);
_static_inline_ char 
is_pair_object(object *obj) {return obj->type == PAIR;}

object *make_error(const char *fmt, ...);
_static_inline_ char 
is_error_object(object *obj) {return obj->type == ERROR;}
char *get_error_message(object *obj);

object *make_primitive_proc(primitive_proc_t fn);
_static_inline_ char 
is_primitive_proc_object(object *obj) {return obj->type == PRIMITIVE_PROC;}
primitive_proc_t get_primitive_proc_value(object *obj);

object *make_eof(FILE *which);
_static_inline_ char 
is_eof_object(object *obj) {return obj->type == END_OF_FILE;}
FILE *get_eof_stream(object *obj);

object *make_input_port(FILE *in);
_static_inline_ char 
is_input_port_object(object *obj) {return obj->type == INPUT_PORT;}
FILE *get_input_port_stream(object *obj);
void close_input_port(object *obj);

object *make_output_port(FILE *out);
_static_inline_ char 
is_output_port_object(object *obj) {return obj->type == OUTPUT_PORT;}
FILE *get_output_port_stream(object *obj);
void close_output_port(object *obj);

object *make_socket(void);
object *make_socket_from_fd(int fd);
_static_inline_ char 
is_socket_object(object *obj) {return obj->type == SOCKET;}
int get_socket_fd(object *obj);
void close_socket(object *obj);

object *make_lambda(object *arguments, object *body);
object *make_begin(object *obj);
object *make_assignment(object *var, object *value);

object *make_compound_proc(object *parameters, object *body, object *env);
_static_inline_ char 
is_compound_proc_object(object *obj) {return obj->type == COMPOUND_PROC;}

object *make_re_pattern(const char *pattern_string);
_static_inline_ char 
is_re_pattern_object(object *obj) {return obj->type == RE_PATTERN;}
const char *get_re_pattern_string(object *obj);
pcre *get_re_pattern_value(object *obj);

object *make_environment(env_entry *entry);
_static_inline_ char 
is_environment_object(object *obj) {return obj->type == ENVIRONMENT;}
_static_inline_ env_entry 
*get_environment_obj(object *obj) {return obj->data.environment.env;};
_static_inline_ void
set_environment_obj(object *obj, env_entry *entry)
{
    obj->data.environment.env = entry;
}
