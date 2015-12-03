
#pragma once

#include "object.h"

object *eval(object *expr, object *env);
object *list_of_values(object *expr, object *env);

