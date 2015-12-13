
#pragma once

#include <stdio.h>

typedef enum 
{
  FIXNUM         /* Integer number (64 bit) */
, REALNUM        /* Real number (double) */
, COMPLEXNUM     /* Complex number (a+bi)*/
, BOOLEAN        /* Truth type (boolean) */
, CHARACTER      /* Character type */
, STRING         /* String type */
, PAIR           /* Cons cell (Pair type) */
, SYMBOL         /* Symbol type */
, PRIMITIVE_PROC /* Primitive procedure */
, COMPOUND_PROC  /* Compound procedure */
, THE_EMPTY_LIST /* The empty list object '() */
, ERROR          /* Error object */
, INPUT_PORT     /* Input port object */
, OUTPUT_PORT    /* Output port object */
, END_OF_FILE    /* End of file type (no idea if this is good)*/
} object_type;

struct object;

typedef struct object *(*primitive_proc_t)(struct object *);

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
            char *value;
        } symbol;

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
            FILE *stream;
        } end_of_file;
    } data;
} object;

object *make_fixnum(long num);
char is_fixnum_object(object *obj);
long get_fixnum_value(object *obj);

object *make_realnum(double num);
char is_realnum_object(object *obj);
double get_realnum_value(object *obj);

object *make_complexnum(double real, double imag);
char is_complexnum_object(object *obj);
double get_complexnum_real_value(object *obj);
double get_complexnum_imag_value(object *obj);

object *add_fixnum_value(object *obj, object *summand);
object *add_realnum_value(object *obj, object *summand);
object *add_complexnum_value(object *obj, object *summand);

object *mul_fixnum_value(object *obj, object *factor);
object *mul_realnum_value(object *obj, object *factor);
object *mul_complexnum_value(object *obj, object *factor);

object *make_character(int c);
char is_character_object(object *obj);
int get_character_value(object *obj);

object *make_string(char *str);
char is_string_object(object *obj);
char *get_string_value(object *obj);

char is_boolean_object(object *obj);
char is_false(object *obj);
char is_true(object *obj);

object *make_symbol(char *str);
char is_symbol_object(object *obj);
char *get_symbol_value(object *obj);

char is_the_empty_list(object *obj);

object *make_pair(object *a, object *b);
char is_pair_object(object *obj);

object *make_error(const char *fmt, ...);
char is_error_object(object *obj);
char *get_error_message(object *obj);

object *make_primitive_proc(primitive_proc_t fn);
char is_primitive_proc_object(object *obj);
primitive_proc_t get_primitive_proc_value(object *obj);

object *make_eof(FILE *which);
char is_eof_object(object *obj);
FILE *get_eof_stream(object *obj);

object *make_input_port(FILE *in);
char is_input_port_object(object *obj);
FILE *get_input_port_stream(object *obj);
void close_input_port(object *obj);

object *make_output_port(FILE *out);
char is_output_port_object(object *obj);
FILE *get_output_port_stream(object *obj);
void close_output_port(object *obj);

object *make_lambda(object *arguments, object *body);
object *make_begin(object *obj);
object *make_assignment(object *var, object *value);

object *make_compound_proc(object *parameters, object *body, object *env);
char is_compound_proc_object(object *obj);



