#pragma once

#include "object.h"

object *make_environment(void);
object *setup_environment(void);
void populate_environment(object *env);
object *extend_environment(object *vars, object *vals, object *base_env);
void add_symbol_to_frame(object *var, object *val, object *frame);
void define_variable(object *symbol, object *value, object *env);
object *find_variable(object *symbol, object *env);


