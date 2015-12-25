#pragma once

#include "object.h"

object *setup_environment(void);
object *extend_environment(object *vars, object *vals, object *base_env);

void define_variable(object *symbol, object *value, object *env);

object *set_variable(object *symbol, object *value, object *env);
object *find_variable(object *symbol, object *env);

object *definition_variable(object *exp);
object *definition_value(object *exp);

object *load_file(char *filename);


