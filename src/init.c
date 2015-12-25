#include <unistd.h>

#include "globals.h"
#include "memory.h"
#include "object.h"
#include "builtins.h"

object *setup_environment(void);
void define_variable(object *symbol, object *value, object *env);

void init(int argc, char **argv)
{
    int i;
    object *args;

    /* Initialize the mempool (must be first call) */
    init_memory_pool();

    /* Boolean constants */
    true = alloc_object();
    true->type = BOOLEAN;
    true->data.boolean.value = 1;

    false = alloc_object();
    false->type = BOOLEAN;
    false->data.boolean.value = 0;

    /* The empty list */
    the_empty_list = alloc_object();
    the_empty_list->type = THE_EMPTY_LIST;

    /* The global environment */
    the_empty_environment = the_empty_list;
    the_global_environment = setup_environment();

    /* Global symbols */
    define_symbol    = make_symbol("define");
    set_symbol       = make_symbol("set!");
    quote_symbol     = make_symbol("quote");
    let_symbol       = make_symbol("let");
    ok_symbol        = make_symbol("ok");
    lambda_symbol    = make_symbol("lambda");
    begin_symbol     = make_symbol("begin");
    do_symbol        = make_symbol("do");

    /* Conditionals */
    if_symbol        = make_symbol("if");
    cond_symbol      = make_symbol("cond");
    else_symbol      = make_symbol("else");

    /* Booleans */
    and_symbol       = make_symbol("and");
    or_symbol        = make_symbol("or");

    /* Input/Output */
    with_output_to_file_symbol = make_symbol("with-output-to-file");
    current_input_port_symbol  = make_symbol("current-input-port");
    current_output_port_symbol = make_symbol("current-output-port");

    /* Input/Output ports */
    define_variable(current_input_port_symbol, 
            make_input_port(stdin),
            the_global_environment);
    define_variable(current_output_port_symbol, 
            make_output_port(stdout),
            the_global_environment);
    
    /* Argument list */
    argv_symbol = make_symbol("*argv*");
    args = the_empty_list;
    for (i = argc - 1; i >= 0; --i)
    {
        args = cons(make_string(argv[i]), args);
    }
    define_variable(argv_symbol, args, the_global_environment);
}

