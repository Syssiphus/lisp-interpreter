#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "environment.h"
#include "builtins.h"

object *make_frame(object *vars, object *vals);
object *frame_variables(object *frame);
object *frame_values(object *frame);
object *first_frame(object *env);
object *enclosing_environment(object *env);

/* Environment */
object *make_environment(void)
{
    object *obj = setup_environment();
    populate_environment(obj);
    return obj;
}

object *setup_environment(void)
{
    object *obj = extend_environment(the_empty_list, the_empty_list,
            the_empty_environment);
    return obj;
}

void populate_environment(object *env)
{
#define add_procedure(scheme_name, c_name)    \
    define_variable(make_symbol(scheme_name), \
            make_primitive_proc(c_name), env);

    add_procedure("apply"   , apply_fake_proc);
    add_procedure("eval"    , eval_fake_proc);

    add_procedure("cons"    , cons_proc);
    add_procedure("car"     , car_proc);
    add_procedure("cdr"     , cdr_proc);
    add_procedure("set-car!", set_car_proc);
    add_procedure("set-cdr!", set_cdr_proc);

    add_procedure("pair?"   , is_pair_proc);
    add_procedure("boolean?", is_boolean_proc);

    add_procedure("+"       , add_proc);
    add_procedure("-"       , sub_proc);
    add_procedure("*"       , mul_proc);
    add_procedure("/"       , quotient_proc); /* TODO: implement own function */

    add_procedure("quotient" , quotient_proc);
    add_procedure("remainder", remainder_proc);
    add_procedure("modulo"   , modulo_proc);

    add_procedure("make-rectangular", make_rectangular_proc);
    add_procedure("magnitude"       , magnitude_proc);

    add_procedure("floor"    , floor_proc);

    add_procedure("mem-usage", mem_usage_proc);

    add_procedure("number?"  , is_number_proc);
    add_procedure("complex?" , is_complex_proc);
    add_procedure("real?"    , is_real_proc);
    add_procedure("rational?", is_rational_proc);
    add_procedure("integer?" , is_integer_proc);

    add_procedure("eqv?"    , is_eqv_proc);
    add_procedure("eq?"     , is_eq_proc);
    add_procedure("symbol?" , is_symbol_proc);
    add_procedure("symbol=?", is_symbol_equal_proc);
    add_procedure("="       , is_number_equal_proc);
    add_procedure("<"       , is_number_lt_proc);
    add_procedure(">"       , is_number_gt_proc);
    add_procedure("<="      , is_number_lteq_proc);
    add_procedure(">="      , is_number_gteq_proc);

    add_procedure("load"      , load_proc);

    add_procedure("open-input-file" , open_input_file_proc);
    add_procedure("open-output-file", open_output_file_proc);
    add_procedure("input-port?"     , is_input_port_proc);
    add_procedure("output-port?"    , is_output_port_proc);

    add_procedure("string?"         , is_string_proc);
    add_procedure("string=?"        , is_string_equal_proc);
    add_procedure("make-string"     , make_string_proc);
    add_procedure("string-length"   , string_length_proc);
    add_procedure("string-ref"      , string_ref_proc);
    add_procedure("string-set!"     , string_set_proc);

    add_procedure("char?"           , is_char_proc);
    add_procedure("char=?"          , is_character_equal_proc);
    add_procedure("char->integer"   , char_to_int_proc);
    add_procedure("integer->char"   , int_to_char_proc);
    add_procedure("number->string"  , number_to_string_proc);

    add_procedure("error"   , error_proc);
    add_procedure("quit"    , quit_proc);
    add_procedure("exit"    , exit_proc);

    add_procedure("print-structure" , pretty_print_structure_proc);
}

object *extend_environment(object *vars, object *vals, object *base_env)
{
    return cons(make_frame(vars, vals), base_env);
}

object *make_frame(object *vars, object *vals)
{
    return cons(vars, vals);
}

object *frame_variables(object *frame)
{
    return car(frame);
}

object *frame_values(object *frame)
{
    return cdr(frame);
}

object *first_frame(object *env)
{
    return car(env);
}

object *enclosing_environment(object *env)
{
    return cdr(env);
}

void add_symbol_to_frame(object *var, object *val, object *frame)
{
    set_car(frame, cons(var, car(frame)));
    set_cdr(frame, cons(val, cdr(frame)));
}

void define_variable(object *symbol, object *value, object *env)
{
    object *frame, *vars, *vals;

    frame = first_frame(env);
    vars = frame_variables(frame);
    vals = frame_values(frame);
    while ( ! is_the_empty_list(vars))
    {
        if (symbol == car(vars))
        {
            set_car(vars, symbol);
            return;
        }
        vars = cdr(vars);
        vals = cdr(vals);
    }
    add_symbol_to_frame(symbol, value, frame);
}

object *set_variable(object *symbol, object *value, object *env)
{
    while ( ! is_the_empty_list(env))
    {
        object *frame = first_frame(env);
        object *vars = frame_variables(frame);
        object *vals = frame_values(frame);
        while ( ! is_the_empty_list(vars))
        {
            if (strcmp(get_symbol_value(symbol), 
                        get_symbol_value(car(vars))) == 0)
            {
                set_car(vals, value);
                return ok_symbol;
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = enclosing_environment(env);
    }

    return make_error("Unknown symbol '%s'", get_symbol_value(symbol));
}

object *find_variable(object *symbol, object *env)
{
    while ( ! is_the_empty_list(env))
    {
        object *frame = first_frame(env);
        object *vars = frame_variables(frame);
        object *vals = frame_values(frame);
        while ( ! is_the_empty_list(vars))
        {
            if (strcmp(get_symbol_value(symbol), 
                        get_symbol_value(car(vars))) == 0)
            {
                return car(vals);
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = enclosing_environment(env);
    }

    return make_error("Unknown symbol '%s'", get_symbol_value(symbol));
}

object *definition_variable(object *exp)
{
    if (is_symbol_object(cadr(exp)))
    {
        return cadr(exp);
    }
    else
    {
        return caadr(exp);
    }
}

object *definition_value(object *exp)
{
    if (is_symbol_object(cadr(exp)))
    {
        return caddr(exp);
    }
    else
    {
        return make_lambda(cdadr(exp), cddr(exp));
    }
}

object *load_file(char *filename)
{
    return load_proc(cons(make_string(filename), the_empty_list));
}
