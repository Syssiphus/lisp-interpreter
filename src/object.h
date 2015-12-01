
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
} object_type;

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


