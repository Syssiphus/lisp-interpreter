
#pragma once

typedef enum 
{
  FIXNUM         /* Integer number (64 bit) */
, BOOLEAN        /* Truth type (boolean) */
, CHARACTER      /* Character type */
, STRING         /* String type */
, PAIR           /* Cons cell (Pair type) */
, SYMBOL         /* Symbol type */
, PRIMITIVE_PROC /* Primitive procedure */
, COMPOUND_PROC  /* Compound procedure */
, THE_EMPTY_LIST /* The empty list object '() */
, ERROR          /* Error object */
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
            primitive_proc_t fn;
        } primitive_proc;
    } data;
} object;

object *make_fixnum(long num);
char is_fixnum_object(object *obj);
long get_fixnum_value(object *obj);

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

object *make_eof(void);
char is_eof_object(object *obj);

object *make_lambda(object *arguments, object *body);

