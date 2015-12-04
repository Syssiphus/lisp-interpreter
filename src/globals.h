
#pragma once

#include "object.h"

/* DEBUGGING */
#define DEBUGGING_ON
#ifdef DEBUGGING_ON
#define dbg_pos() fprintf(stderr, "%s, %d, %s()\n", __FILE__, __LINE__, \
        __func__);
#define dbg_print(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);
#else
#define dbg_pos()
#define dbg_print(fmt, ...) 
#endif

/* Environment */
object *the_global_environment;

/* Boolean values */
object *true;
object *false;

/* The empty list */
object *the_empty_list;

/* The empty environment */
object *the_empty_environment;

/* Builtin/Primitive procedures symbols */
object *symbol_table;
object *define_symbol;
object *set_symbol;
object *quote_symbol;
object *ok_symbol;
object *lambda_symbol;
object *begin_symbol;
object *let_symbol;
object *mem_usage_symbol;
object *if_symbol;
object *cond_symbol;
object *else_symbol;
object *eqv_symbol;
object *symbol_eq_symbol;
object *string_eq_symbol;
object *char_eq_symbol;
object *num_eq_symbol;

