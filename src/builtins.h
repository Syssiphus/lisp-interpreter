
#pragma once

#include "object.h"

object *cons(object *a, object *b);
object *car(object *obj);
object *cdr(object *obj);
void set_car(object *dst, object *obj);
void set_cdr(object *dst, object *obj);

object *add_proc(object *arguments);
object *sub_proc(object *arguments);
object *mul_proc(object *arguments);

object *length_proc(object *arguments);

