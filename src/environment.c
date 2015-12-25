#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "globals.h"
#include "environment.h"
#include "builtins.h"

#include "uthash.h"
#include "gc.h"

/* Redefine the malloc/free calls of uthash */
#undef uthash_malloc
#undef uthash_free
#define uthash_malloc(sz) GC_malloc(sz)
#define uthash_free(sz, ptr) /* Not needed, memory is collected automatically */

object *make_frame(object *vars, object *vals);
object *frame_variables(object *frame);
object *frame_values(object *frame);
object *first_frame(object *env);

void populate_environment(object *env);

/* Environment */
object *setup_environment(void)
{
    object *obj = extend_environment(the_empty_list, the_empty_list,
            the_empty_environment);
    populate_environment(obj);
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
    
    add_procedure("vector?"      , is_vector_proc);
    add_procedure("make-vector"  , make_vector_proc);
    add_procedure("vector-length", vector_length_proc);
    add_procedure("vector-ref"   , vector_ref_proc);
    add_procedure("vector-set!"  , vector_set_proc);

    add_procedure("+"        , add_proc);
    add_procedure("-"        , sub_proc);
    add_procedure("*"        , mul_proc);
    add_procedure("/"        , quotient_proc); /* TODO: implement own function */
    add_procedure("quotient" , quotient_proc);
    add_procedure("remainder", remainder_proc);
    add_procedure("modulo"   , modulo_proc);
    add_procedure("floor"    , floor_proc);

    add_procedure("make-rectangular", make_rectangular_proc);
    add_procedure("magnitude"       , magnitude_proc);

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
    add_procedure("list->string"    , list_to_string_proc);
    add_procedure("string->list"    , string_to_list_proc);

    add_procedure("error"   , error_proc);
    add_procedure("quit"    , quit_proc);
    add_procedure("exit"    , exit_proc);
    
    add_procedure("re-pattern", re_pattern_proc);
    add_procedure("re-match"  , re_match_proc);

    add_procedure("make-socket"  , make_socket_proc);
    add_procedure("socket-bind"  , socket_bind_proc);
    add_procedure("socket-listen", socket_listen_proc);
    add_procedure("socket-accept", socket_accept_proc);
    add_procedure("socket-close" , close_socket_proc);
    add_procedure("socket?"      , is_socket_proc);
    
    add_procedure("select" , select_proc);
    
    add_procedure("sleep", sleep_proc);
    
    add_procedure("write-char", write_char_proc);
    add_procedure("read-char" , read_char_proc);
    
    add_procedure("write", write_proc);
    add_procedure("read", read_proc);

    add_procedure("print-structure" , pretty_print_structure_proc);
    add_procedure("load-dynlib"     , load_dynlib_proc);
}

env_entry *add_symbol_to_env(char *symbol, object *obj, env_entry *env)
{
    env_entry *entry = GC_malloc(sizeof(env_entry));
    if ( ! entry)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    
    entry->symbol = GC_malloc(strlen(symbol) + 1);
    if ( ! entry->symbol)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    strncpy(entry->symbol, symbol, strlen(symbol));
    
    entry->obj = obj;
    HASH_ADD_STR(env, symbol, entry);
    return entry;
}

object *extend_environment(object *vars, object *vals, object *base_env)
{
    env_entry *new_env = NULL;
    
    while ( ! is_the_empty_list(vars))
    {
        new_env = add_symbol_to_env(get_symbol_value(car(vars)),
                                    car(vals),
                                    new_env);
        vars = cdr(vars);
        vals = cdr(vals);
    }

    return cons(make_environment(new_env), base_env);
}

void define_variable(object *symbol, object *value, object *env)
{
    char *symbol_name = get_symbol_value(symbol);
    object *current_env = car(env);
    env_entry *env_obj = get_environment_obj(current_env);
    env_entry *env_symbol;

    HASH_FIND_STR(env_obj, symbol_name, env_symbol);
    if (env_symbol != NULL) return; /* already exists */

    env_obj = add_symbol_to_env(symbol_name, value, env_obj);
    
    set_environment_obj(current_env, env_obj);
}

object *set_variable(object *symbol, object *value, object *env)
{
    char *symbol_name = get_symbol_value(symbol);

    while ( ! is_the_empty_list(env))
    {
        env_entry *env_obj = get_environment_obj(car(env));
        env_entry *env_symbol;

        HASH_FIND_STR(env_obj, symbol_name, env_symbol);
        if (env_symbol != NULL)
        {
            env_symbol->obj = value;
            return ok_symbol;
        }

        env = cdr(env);
    }

    return make_error("Unknown symbol '%s'", get_symbol_value(symbol));
}

object *find_variable(object *symbol, object *env)
{
    char *symbol_name = get_symbol_value(symbol);

    while ( ! is_the_empty_list(env))
    {
        env_entry *env_obj = get_environment_obj(car(env));
        env_entry *env_symbol;

        HASH_FIND_STR(env_obj, symbol_name, env_symbol);
        if (env_symbol != NULL)
        {
            return env_symbol->obj;
        }

        env = cdr(env);
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
    return load_proc(cons(make_string(filename), the_empty_list), 
                     the_global_environment);
}
