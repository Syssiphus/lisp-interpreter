
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
#define cdddr(x) cdr(cdr(cdr(x)))

#define cadddr(x) car(cdr(cdr(cdr(x))))
#define caddar(x) car(cdr(cdr(car(x))))

object *cons_proc(object *arguments);
object *car_proc(object *arguments);
object *cdr_proc(object *arguments);
object *set_car_proc(object *arguments);
object *set_cdr_proc(object *arguments);

object *is_pair_proc(object *arguments);
object *is_boolean_proc(object *arguments);

object *add_proc(object *arguments);
object *sub_proc(object *arguments);
object *mul_proc(object *arguments);

object *quotient_proc(object *arguments);
object *remainder_proc(object *arguments);
object *modulo_proc(object *arguments);

object *floor_proc(object *arguments);

object *length_proc(object *arguments);

object *mem_usage_proc(object *obj);

object *is_number_proc(object *arguments);
object *is_complex_proc(object *arguments);
object *is_real_proc(object *arguments);
object *is_rational_proc(object *arguments);
object *is_integer_proc(object *arguments);

object *make_rectangular_proc(object *arguments);
object *magnitude_proc(object *arguments);

object *is_eqv_proc(object *arguments);
object *is_eq_proc(object *arguments);
object *is_symbol_proc(object *arguments);
object *is_symbol_equal_proc(object *arguments);
object *is_number_equal_proc(object *arguments);
object *is_number_lt_proc(object *arguments);
object *is_number_gt_proc(object *arguments);
object *is_number_lteq_proc(object *arguments);
object *is_number_gteq_proc(object *arguments);

object *load_proc(object *arguments);

object *open_input_file_proc(object *arguments);
object *open_output_file_proc(object *arguments);
object *is_input_port_proc(object *arguments);
object *is_output_port_proc(object *arguments);

object *is_string_equal_proc(object *arguments);
object *is_string_proc(object *arguments);
object *make_string_proc(object *arguments);
object *string_length_proc(object *arguments);
object *string_ref_proc(object *arguments);
object *string_set_proc(object *arguments);
object *number_to_string_proc(object *arguments);

object *is_char_proc(object *arguments);
object *is_character_equal_proc(object *arguments);
object *char_to_int_proc(object *arguments);
object *int_to_char_proc(object *arguments);

object *error_proc(object *arguments);
object *quit_proc(object *arguments);
object *exit_proc(object *arguments);

object *pretty_print_structure_proc(object *arguments);

