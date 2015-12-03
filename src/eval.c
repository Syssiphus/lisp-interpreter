
#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "eval.h"
#include "environment.h"
#include "builtins.h"

char is_self_evaluating(object *obj);
char is_tagged_list(object *obj, object *sym);
char is_quoted(object *obj);
char is_definition(object *obj);
object *text_of_quotation(object *obj);

object *eval(object *expr, object *env)
{
    object *procedure = the_empty_list;
    object *arguments = the_empty_list;

/* Goto target for tailcalls */
tailcall:

    if (is_self_evaluating(expr))
    {
        return expr;
    }
    else if (is_symbol_object(expr))
    {
        /* Look for symbol in the environment */
        return find_variable(expr, env);
    }
    else if (is_quoted(expr))
    {
        return text_of_quotation(expr);
    }
    else if (is_definition(expr))
    {
        define_variable(definition_variable(expr),
                eval(definition_value(expr), env),
                env);
        return ok_symbol;
    }
    else if (is_pair_object(expr))
    {
        procedure = eval(car(expr), env);
        arguments = list_of_values(cdr(expr), env);

        if (is_primitive_proc_object(procedure))
        {
            return (procedure->data.primitive_proc.fn)(arguments);
        }

        return make_error("Unknown procedure '%s'.", 
                get_symbol_value(car(expr)));
    }
    else if (is_primitive_proc_object(expr))
    {
        return expr;
    }
    else if (is_eof_object(expr))
    {
        fprintf(stderr, "\nExiting.\n");
        exit(0);
    }
    return make_error("No operation, unknown object type involved.");
}

char is_self_evaluating(object *obj)
{
    return is_fixnum_object(obj)
        || is_character_object(obj)
        || is_string_object(obj)
        || is_boolean_object(obj)
        || is_the_empty_list(obj)
        || is_error_object(obj)
        ;
}

object *list_of_values(object *expr, object *env)
{
    if (is_pair_object(expr))
    {
        return cons(eval(car(expr), env),
                list_of_values(cdr(expr), env));
    }

    return the_empty_list;
}

char is_tagged_list(object *obj, object *sym)
{
    return is_pair_object(obj) 
        && is_symbol_object(car(obj)) 
        && car(obj) == sym;
}

char is_quoted(object *obj)
{
    return is_tagged_list(obj, quote_symbol);
}

object *text_of_quotation(object *obj)
{
    return cadr(obj);
}

char is_definition(object *obj)
{
    return is_tagged_list(obj, define_symbol);
}


