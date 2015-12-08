
#include <stdlib.h>

#include "write.h"
#include "builtins.h"

void write(FILE *out, object *obj);
void write_pair(FILE *out, object *obj);

void write(FILE *out, object *obj)
{
    if (is_fixnum_object(obj))
    {
        fprintf(out, "%ld", get_fixnum_value(obj));
    }
    else if (is_realnum_object(obj))
    {
        fprintf(out, "%f", get_realnum_value(obj));
    }
    else if (is_character_object(obj))
    {
        int c = get_character_value(obj);

        switch (c)
        {
            case ' ':
                fprintf(out, "#\\space");
                break;
            case '\n':
                fprintf(out, "#\\newline");
                break;
            default:
                fprintf(out, "#\\%c", c);
                break;
        }
    }
    else if (is_string_object(obj))
    {
        fprintf(out, "\"%s\"", get_string_value(obj));
    }
    else if (is_boolean_object(obj))
    {
        fprintf(out, "#%c", is_false(obj) ? 'f' : 't');
    }
    else if (is_symbol_object(obj))
    {
        fprintf(out, "%s", get_symbol_value(obj));
    }
    else if (is_the_empty_list(obj))
    {
        fprintf(out, "()");
    }
    else if (is_pair_object(obj))
    {
        fprintf(out, "(");
        write_pair(out, obj);
        fprintf(out, ")");
    }
    else if (is_error_object(obj))
    {
        fprintf(out, "Error: %s", get_error_message(obj));
    }
    else if (is_primitive_proc_object(obj))
    {
        fprintf(out, "<primitive procedure %p>", 
                get_primitive_proc_value(obj));
    }
    else if (is_eof_object(obj))
    {
        ; /* Nothing */
    }
    else if (is_input_port_object(obj))
    {
        FILE * port = get_input_port_stream(obj);
        if (port == stdin)
        {
            fprintf(out, "<input port stdin>");
        }
        else
        {
            fprintf(out, "<input port %p>", port);
        }
    }
    else if (is_output_port_object(obj))
    {
        FILE * port = get_output_port_stream(obj);
        if (port == stdout)
        {
            fprintf(out, "<input port stdout>");
        }
        else
        {
            fprintf(out, "<input port %p>", port);
        }
    }
    else
    {
        fprintf(stderr, "%s, %d: Cannot write unknown object '%s'\n",
                __FILE__, __LINE__, __func__);
        exit(1);
    }
}

void write_pair(FILE *out, object *obj)
{
    object *car_obj = car(obj);
    object *cdr_obj = cdr(obj);

    write(out, car_obj);

    if (is_pair_object(cdr_obj))
    {
        fprintf(out, " ");
        write_pair(out, cdr_obj);
    }
    else if (! is_the_empty_list(cdr_obj))
    {
        fprintf(out, " . ");
        write(out, cdr_obj);
    }
}


