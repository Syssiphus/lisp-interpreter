
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "eval.h"
#include "environment.h"
#include "builtins.h"

char is_self_evaluating(object *obj);
char is_tagged_list(object *obj, object *sym);
char is_quoted(object *obj);
char is_definition(object *obj);
char is_lambda(object *obj);
char is_begin(object *obj);
char is_if(object *obj);
char is_cond(object *obj);
char is_let(object *obj);
char is_assignment(object *obj);
object *lambda_parameters(object *obj);
object *lambda_body(object *obj);
object *text_of_quotation(object *obj);

object *let_parameters(object *obj);
object *let_body(object *obj);
object *let_arguments(object *obj);

object *eval_assignment(object *obj, object *env);
object *eval_cond(object *obj, object *env);

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
    else if (is_assignment(expr))
    {
        return eval_assignment(expr, env);
    }
    else if (is_definition(expr))
    {
        define_variable(definition_variable(expr),
                eval(definition_value(expr), env),
                env);
        return ok_symbol;
    }
    else if (is_let(expr))
    {
        expr = cons(
                make_lambda(let_parameters(expr),
                    let_body(expr)), 
                let_arguments(expr));
        goto tailcall;
    }
    else if (is_if(expr))
    {
        expr = is_true(eval(cadr(expr), env))
            ? caddr(expr)
            : cadddr(expr);
        goto tailcall;
    }
    else if (is_cond(expr))
    {
        expr = eval_cond(expr, env);
        goto tailcall;
    }
    else if (is_lambda(expr))
    {
        return make_compound_proc(lambda_parameters(expr), 
                lambda_body(expr), env);
    }
    else if (is_begin(expr))
    {
        expr = cdr(expr);
        while ( ! is_the_empty_list(cdr(expr)))
        {
            eval(car(expr), env);
            expr = cdr(expr);
        }
        expr = car(expr);
        goto tailcall;
    }
    else if (is_tagged_list(expr, or_symbol))
    {
        expr = cdr(expr);
        if (is_the_empty_list(expr))
        {
            return false;
        }
        while ( ! is_the_empty_list(cdr(expr)))
        {
            if (is_true(eval(car(expr), env)))
            {
                return true;
            }
            expr = cdr(expr);
        }
        expr = car(expr);
        goto tailcall;
    }
    else if (is_tagged_list(expr, and_symbol))
    {
        expr = cdr(expr);
        if (is_the_empty_list(expr))
        {
            return true;
        }
        while ( ! is_the_empty_list(cdr(expr)))
        {
            if (is_false(eval(car(expr), env)))
            {
                return false;
            }
            expr = cdr(expr);
        }
        expr = car(expr);
        goto tailcall;
    }
    else if (is_pair_object(expr))
    {
        procedure = eval(car(expr), env);
        arguments = list_of_values(cdr(expr), env);

        if (is_primitive_proc_object(procedure))
        {
            return (procedure->data.primitive_proc.fn)(arguments);
        }
        else if (is_compound_proc_object(procedure))
        {
            env = extend_environment(procedure->data.compound_proc.parameters,
                    arguments, procedure->data.compound_proc.env);
            expr = make_begin(procedure->data.compound_proc.body);
            goto tailcall;
        }

        return make_error("Unknown procedure '%s'.", 
                get_symbol_value(car(expr)));
    }
    else if (is_eof_object(expr))
    {
        if (get_eof_stream(expr) == stdin)
        {
            fprintf(stderr, "\nExiting.\n");
            exit(0);
        }
        else
        {
            return ok_symbol;
        }
    }
    return make_error("No operation, unknown object type involved.");
}

char is_self_evaluating(object *obj)
{
    return is_fixnum_object(obj)
        || is_realnum_object(obj)
        || is_character_object(obj)
        || is_string_object(obj)
        || is_boolean_object(obj)
        || is_the_empty_list(obj)
        || is_error_object(obj)
        || is_primitive_proc_object(obj)
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

char is_lambda(object *obj)
{
    return is_tagged_list(obj, lambda_symbol);
}

object *lambda_parameters(object *obj)
{
    return cadr(obj);
}

object *lambda_body(object *obj)
{
    return cddr(obj);
}

char is_begin(object *obj)
{
    return is_tagged_list(obj, begin_symbol);
}

char is_definition(object *obj)
{
    return is_tagged_list(obj, define_symbol);
}

char is_if(object *obj)
{
    return is_tagged_list(obj, if_symbol);
}

char is_let(object *obj)
{
    return is_tagged_list(obj, let_symbol);
}

object *extract_let_parameters(object *obj)
{
    return is_the_empty_list(obj)
        ? the_empty_list
        : cons(caar(obj),
                extract_let_parameters(cdr(obj)));
}

object *extract_let_arguments(object *obj)
{
    return is_the_empty_list(obj)
        ? the_empty_list
        : cons(cadar(obj),
                extract_let_arguments(cdr(obj)));
}

object *let_parameters(object *obj)
{
    object *parameterlist = cadr(obj);
    return extract_let_parameters(parameterlist);
}

object *let_body(object *obj)
{
    return cddr(obj);
}

object *let_arguments(object *obj)
{
    object *argumentlist = cadr(obj);
    return extract_let_arguments(argumentlist);
}

char is_assignment(object *obj)
{
    return is_tagged_list(obj, set_symbol);
}

object *eval_assignment(object *obj, object *env)
{
    return set_variable(cadr(obj), eval(caddr(obj), env), env);
}

char is_cond(object *obj)
{
    return is_tagged_list(obj, cond_symbol);
}

object *make_cond(object *cond)
{
    object *predicate;
    object *expression;

    if (is_the_empty_list(cond))
    {
        return the_empty_list;
    }

    predicate = caar(cond);
    expression = cdar(cond);

    if (predicate == else_symbol && ! is_the_empty_list(cdr(cond)))
    {
        return make_error("'else' keyword is not the last condition in "
                "cond statement");
    }

    if (predicate == else_symbol)
    {
        predicate = true;
    }

    return cons(if_symbol, 
            cons(predicate,  /* Predicate */
                cons(make_begin(expression), /* True */
                    cons(make_cond(cdr(cond)), /* False */
                        the_empty_list))));
}

object *eval_cond(object *obj, object *env)
{
    return make_cond(cdr(obj));
}


