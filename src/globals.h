
#pragma once

/* #define OBJECT_REFERENCES */

#include "object.h"

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

