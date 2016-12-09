
#pragma once

#include "object.h"
#include "defines.h"

_static_inline_ object *cons(object *a, object *b)
{
    return make_pair(a, b);
}

_static_inline_ object *car(object *obj)
{
    if (is_pair_object(obj))
    {
        return obj->data.pair.car;
    }
    return make_error("Not a pair object.");
}

_static_inline_ object *cdr(object *obj)
{
    if (is_pair_object(obj))
    {
        return obj->data.pair.cdr;
    }
    return make_error("Not a pair object.");
}

_static_inline_ void set_car(object *dst, object *obj)
{
    dst->data.pair.car = obj;
}

_static_inline_ void set_cdr(object *dst, object *obj)
{
    dst->data.pair.cdr = obj;
}

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
#define cddddr(x) cdr(cdr(cdr(cdr(x))))

object *apply_fake_proc(object *arguments, object *env);
object *eval_fake_proc(object *arguments, object *env);

object *cons_proc(object *arguments, object *env);
object *car_proc(object *arguments, object *env);
object *cdr_proc(object *arguments, object *env);
object *set_car_proc(object *arguments, object *env);
object *set_cdr_proc(object *arguments, object *env);

object *is_pair_proc(object *arguments, object *env);
object *is_boolean_proc(object *arguments, object *env);

object *add_proc(object *arguments, object *env);
object *sub_proc(object *arguments, object *env);
object *mul_proc(object *arguments, object *env);

object *quotient_proc(object *arguments, object *env);
object *remainder_proc(object *arguments, object *env);
object *modulo_proc(object *arguments, object *env);

object *floor_proc(object *arguments, object *env);

object *length_proc(object *arguments, object *env);

object *mem_usage_proc(object *obj, object *env);

object *is_number_proc(object *arguments, object *env);
object *is_complex_proc(object *arguments, object *env);
object *is_real_proc(object *arguments, object *env);
object *is_rational_proc(object *arguments, object *env);
object *is_integer_proc(object *arguments, object *env);

object *make_rectangular_proc(object *arguments, object *env);
object *magnitude_proc(object *arguments, object *env);

object *is_eqv_proc(object *arguments, object *env);
object *is_eq_proc(object *arguments, object *env);
object *is_symbol_proc(object *arguments, object *env);
object *is_symbol_equal_proc(object *arguments, object *env);
object *is_number_equal_proc(object *arguments, object *env);
object *is_number_lt_proc(object *arguments, object *env);
object *is_number_gt_proc(object *arguments, object *env);
object *is_number_lteq_proc(object *arguments, object *env);
object *is_number_gteq_proc(object *arguments, object *env);

object *load_proc(object *arguments, object *env);

object *open_input_file_proc(object *arguments, object *env);
object *open_output_file_proc(object *arguments, object *env);
object *is_input_port_proc(object *arguments, object *env);
object *is_output_port_proc(object *arguments, object *env);

object *is_string_equal_proc(object *arguments, object *env);
object *is_string_proc(object *arguments, object *env);
object *make_string_proc(object *arguments, object *env);
object *string_length_proc(object *arguments, object *env);
object *string_ref_proc(object *arguments, object *env);
object *string_set_proc(object *arguments, object *env);
object *number_to_string_proc(object *arguments, object *env);
object *list_to_string_proc(object *arguments, object *env);
object *string_to_list_proc(object *arguments, object *env);

object *is_char_proc(object *arguments, object *env);
object *is_character_equal_proc(object *arguments, object *env);
object *char_to_int_proc(object *arguments, object *env);
object *int_to_char_proc(object *arguments, object *env);

object *error_proc(object *arguments, object *env);
object *quit_proc(object *arguments, object *env);
object *exit_proc(object *arguments, object *env);

object *pretty_print_structure_proc(object *arguments, object *env);

object *load_dynlib_proc(object *arguments, object *env);

object *re_pattern_proc(object *arguments, object *env);
object *re_match_proc(object *arguments, object *env);

object *is_vector_proc(object *arguments, object *env);
object *make_vector_proc(object *arguments, object *env);
object *vector_length_proc(object *arguments, object *env);
object *vector_ref_proc(object *arguments, object *env);
object *vector_set_proc(object *arguments, object *env);

object *make_socket_proc(object *arguments, object *env);
object *socket_bind_proc(object *arguments, object *env);
object *socket_listen_proc(object *arguments, object *env);
object *socket_accept_proc(object *arguments, object *env);
object *close_socket_proc(object *arguments, object *env);
object *is_socket_proc(object *arguments, object *env);

object *select_proc(object *arguments, object *env);
object *sleep_proc(object *arguments, object *env);

object *write_char_proc(object *arguments, object *env);
object *read_char_proc(object *arguments, object *env);
object *peek_char_proc(object *arguments, object *env);
object *is_eof_proc(object *arguments, object *env);

object *write_proc(object *arguments, object *env);
object *read_proc(object *arguments, object *env);

object *make_queue_proc(object *arguments, object *env);
object *read_queue_proc(object *arguments, object *env);
object *write_queue_proc(object *arguments, object *env);

