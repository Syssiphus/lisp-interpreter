#include "globals.h"
#include "memory.h"

object *make_environment(void);

void init(void)
{
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

    /* Global symbols */
    symbol_table     = the_empty_list;
    define_symbol    = make_symbol("define");
    set_symbol       = make_symbol("set!");
    quote_symbol     = make_symbol("quote");
    let_symbol       = make_symbol("let");
    ok_symbol        = make_symbol("ok");
    lambda_symbol    = make_symbol("lambda");
    begin_symbol     = make_symbol("begin");

    /* Conditionals */
    if_symbol        = make_symbol("if");
    cond_symbol      = make_symbol("cond");
    else_symbol      = make_symbol("else");

    /* Booleans */
    and_symbol       = make_symbol("and");
    or_symbol        = make_symbol("or");

    /* The global environment */
    the_empty_environment = the_empty_list;
    the_global_environment = make_environment();
}

