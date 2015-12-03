
#pragma once

#include "object.h"

object *cons(object *a, object *b);
object *car(object *obj);
object *cdr(object *obj);
void set_car(object *dst, object *obj);
void set_cdr(object *dst, object *obj);

#define caar(x) car(car(x))
#define cadr(x) car(cdr(x))
#define cdar(x) cdr(car(x))
#define cddr(x) cdr(cdr(x))

object *add_proc(object *arguments);
object *sub_proc(object *arguments);
object *mul_proc(object *arguments);
object *quotient_proc(object *arguments);

object *length_proc(object *arguments);

