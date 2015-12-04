
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

#define caddr(x) car(cdr(cdr(x)))
#define caadr(x) car(car(cdr(x)))
#define caddr(x) car(cdr(cdr(x)))
#define cadar(x) car(cdr(car(x)))
#define cdadr(x) cdr(car(cdr(x)))

#define cadddr(x) car(cdr(cdr(cdr(x))))

object *add_proc(object *arguments);
object *sub_proc(object *arguments);
object *mul_proc(object *arguments);
object *quotient_proc(object *arguments);

object *length_proc(object *arguments);

object *mem_usage_proc(object *obj);

object *is_eqv_proc(object *arguments);

object *is_symbol_equal_proc(object *arguments);
object *is_string_equal_proc(object *arguments);
object *is_character_equal_proc(object *arguments);
object *is_number_equal_proc(object *arguments);

