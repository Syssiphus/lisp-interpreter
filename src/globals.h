
#pragma once

#include "object.h"

/* DEBUGGING */
#ifdef _DEBUG
#define DEBUG_BREAK __builtin_trap()
#else 
#define DEBUG_BREAK do {} while(0)
#endif
#define DEBUGGING_ON
#ifdef DEBUGGING_ON
#define dbg_pos() fprintf(stderr, "%s, %d, %s()\n", __FILE__, __LINE__, \
        __func__);
#define dbg_print(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);
#else
#define dbg_pos()
#define dbg_print(fmt, ...) 
#endif

/* Unused parameter marker */
#define UNUSED(x) (void)(x)

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
object *do_symbol;
object *ok_symbol;

object *lambda_symbol;
object *begin_symbol;
object *let_symbol;

object *if_symbol;
object *cond_symbol;
object *else_symbol;

object *and_symbol;
object *or_symbol;

object *with_output_to_file_symbol;
object *current_input_port_symbol;
object *current_output_port_symbol;

object *argv_symbol;

