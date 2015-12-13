
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

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
char is_write_char(object *obj);
char is_with_output_to_file(object *obj);
char is_or(object *obj);
char is_and(object *obj);
char is_do(object *obj);

object *lambda_parameters(object *obj);
object *lambda_body(object *obj);
object *text_of_quotation(object *obj);

object *let_parameters(object *obj);
object *let_body(object *obj);
object *let_arguments(object *obj);

object *eval_assignment(object *obj, object *env);
object *eval_cond(object *obj, object *env);

object *do_variables(object *expr);
object *do_initializers(object *expr);
object *do_steps(object *expr);
object *do_termination(object *expr);
object *do_body(object *expr);

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
    else if (is_or(expr))
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
    else if (is_and(expr))
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
    else if (is_write_char(expr))
    {
        /* Implemented here because we need the environment */
        object *port = find_variable(current_output_port_symbol, env);
        object *character = eval(cadr(expr), env);

        if(cddr(expr) != the_empty_list)
        {
            port = eval(caddr(expr), env);
        }

        fprintf(get_output_port_stream(port), "%c", 
                get_character_value(character));

        return ok_symbol;
    }
    else if (is_with_output_to_file(expr))
    {
        FILE *outputfile;
        object *vars, *vals, *compound;
        object *old_port, *new_port, *result;
        object *filename;

        arguments = cdr(expr);

        filename = eval(car(arguments), env);

        if ( ! is_string_object(filename))
        {
            return make_error("Wrong argument type for argument 1.");
        }

        outputfile = fopen(get_string_value(filename), "w");
        if ( ! outputfile)
        {
            return make_error("Unable to open file '%s' (%s)",
                    get_string_value(car(arguments)),
                    strerror(errno));
        }

        /* - Save old output port
         * - Set new output port
         * - Create lambda from body
         * - Add resulting compound procedure to env
         * - Call procedure
         */
        compound = cons(
                eval(make_lambda(the_empty_list, cddr(expr)), env),
                the_empty_list);

        vars = cons(make_symbol("__anonymous_func__"), the_empty_list);
        vals = compound;
        env = extend_environment(vars, vals, env);

        expr = cons(make_symbol("__anonymous_func__"), the_empty_list);

        old_port = find_variable(current_output_port_symbol, env);
        if (is_error_object(old_port))
        {
            return old_port; /* The error */
        }

        new_port = make_output_port(outputfile);
        result = set_variable(
                current_output_port_symbol, 
                new_port,
                env);
        if (is_error_object(result))
        {
            return result; /* The error */
        }

        result = eval(expr, env);

        set_variable(
                current_output_port_symbol, 
                old_port,
                env); /* Set value back to the old port */
        close_output_port(new_port);

        return result;
    }
    else if (is_do(expr))
    {
        object *steps;
        object *termination;
        object *body;
        object *result;

        /* Initialize the 'local' variables */
        env = extend_environment(
                do_variables(expr),
                do_initializers(expr),
                env);

        /* Extract infos */
        steps = do_steps(expr);
        termination = do_termination(expr);
        body = do_body(expr);

        while(is_false(eval(make_begin(termination), env)))
        {
            /* Do loop */
            /* - Execute the body of the loop */
            result = eval(make_begin(body), env);

            /* - Execute the steppers */
            eval(make_begin(steps), env);
        }
        return result;
    }
    else if (is_pair_object(expr))
    {
        procedure = eval(car(expr), env);
        if (is_error_object(procedure))
        {
            return procedure; /* return the error */
        }

        arguments = list_of_values(cdr(expr), env);

        if (is_primitive_proc_object(procedure))
        {
            return (procedure->data.primitive_proc.fn)(arguments);
        }
        else if (is_compound_proc_object(procedure))
        {
            object *vars = the_empty_list;
            object *vals = the_empty_list;
            object *parameters = procedure->data.compound_proc.parameters;

            /* Scheme R5RS page 9, procedures:
             * - If the procedure has only one argument defined then all the 
             *   given arguments are supplied as a list bound to that defined
             *   argument. (FIXME: this does not seem to be true, at least 
             *   chicken scheme behaves different)
             * - If the procedure has n arguments defined the procedure has a
             *   fixed number of arguments and the supplied values are bound
             *   to the arguments normally
             * - If the argument list is a dotted list then all arguments
             *   which could not be bound to available arguments are bound as 
             *   a list to the dotted part of the argument list.
             */
            while (1)
            {
                if (is_the_empty_list(parameters))
                {
                    break;
                }
                else if (is_pair_object(parameters))
                {
                    vars = cons(car(parameters), vars);
                    vals = cons(car(arguments), vals);
                    parameters = cdr(parameters);
                    arguments = cdr(arguments);
                }
                else
                {
                    vars = cons(parameters, vars);
                    vals = cons(arguments, vals);
                    break;
                }
            }

            env = extend_environment(vars, vals, 
                    procedure->data.compound_proc.env);
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

char is_write_char(object *obj)
{
    return is_tagged_list(obj, write_char_symbol);
}

char is_with_output_to_file(object *obj)
{
    return is_tagged_list(obj, with_output_to_file_symbol);
}

char is_or(object *obj)
{
    return is_tagged_list(obj, or_symbol);
}

char is_and(object *obj)
{
    return is_tagged_list(obj, and_symbol);
}

char is_do(object *obj)
{
    return is_tagged_list(obj, do_symbol);
}

object *do_variables(object *expr)
{
    object *obj = cadr(expr);
    object *vars = the_empty_list;

    while ( ! is_the_empty_list(obj))
    {
        vars = cons(caar(obj), vars);
        obj = cdr(obj);
    }
    return vars;
}

object *do_initializers(object *expr)
{
    object *obj = cadr(expr);
    object *inits = the_empty_list;

    while ( ! is_the_empty_list(obj))
    {
        inits = cons(cadar(obj), inits);
        obj = cdr(obj);
    }
    return inits;
}

object *do_steps(object *expr)
{
    object *obj = cadr(expr);
    object *steps = the_empty_list;

    while ( ! is_the_empty_list(obj))
    {
        /* Construct a 'set!' statement with the
         * identifier and the expression to use on the identifier */
        steps = cons(make_assignment(caar(obj), caddar(obj)), steps);
        obj = cdr(obj);
    }
    return steps;
}

object *do_termination(object *expr)
{
    return caddr(expr);
}

object *do_body(object *expr)
{
    return cdddr(expr);
}


