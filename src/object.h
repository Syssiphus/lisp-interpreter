
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

    union
    {
    } data;
} object;

