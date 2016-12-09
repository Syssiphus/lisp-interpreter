
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "write.h"
#include "builtins.h"

void scheme_write(FILE *out, object *obj);
void write_pair(FILE *out, object *obj);

void scheme_write(FILE *out, object *obj)
{
    if (is_fixnum_object(obj))
    {
        fprintf(out, "%ld", get_fixnum_value(obj));
    }
    else if (is_realnum_object(obj))
    {
        fprintf(out, "%f", get_realnum_value(obj));
    }
    else if (is_complexnum_object(obj))
    {
        fprintf(out, "%f%+fi", 
                get_complexnum_real_value(obj), 
                get_complexnum_imag_value(obj));
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
    else if (is_vector_object(obj))
    {
        unsigned long i;
        unsigned long length = get_fixnum_value(vector_length(obj)); 
        fprintf(out, "#(");
        for (i = 0; i < length; ++i)
        {
            scheme_write(out, get_vector_item(obj, i));
            if (i < length - 1)
            {
                fprintf(out, " ");
            }
        }
        fprintf(out, ")");
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
    else if (is_compound_proc_object(obj))
    {
        fprintf(out, "<compound procedure>");
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
            fprintf(out, "<output port stdout>");
        }
        else
        {
            fprintf(out, "<output port %p>", port);
        }
    }
    else if (is_socket_object(obj))
    {
        struct sockaddr_in address;
        socklen_t address_len = sizeof(address);
        char *address_str = "Unknown";
        char addrstr[INET_ADDRSTRLEN];
        unsigned short port = 0;

        if (getsockname(get_socket_fd(obj), 
                        (struct sockaddr *)&address, 
                        &address_len) != -1)
        {
            if ( ! inet_ntop(address.sin_family, 
                             &(address.sin_addr), 
                             addrstr, INET_ADDRSTRLEN))
            {
                address_str = strerror(errno);
            }
            else
            {
                address_str = addrstr;
                port = ntohs(address.sin_port);
            }
        }

        fprintf(out, "<socket address \"%s\" port %d>", address_str, port);
    }
    else if (is_re_pattern_object(obj))
    {
        fprintf(out, "<regex pattern: \"%s\">", 
                get_re_pattern_string(obj));
    }
    else if (is_queue_object(obj))
    {
        fprintf(out, "<QUEUE object>");
    }
    else
    {
        fprintf(stderr, "%s, %d: Cannot write unknown object. '%s'\n",
                __FILE__, __LINE__, __func__);
        exit(1);
    }
}

void write_pair(FILE *out, object *obj)
{
    object *car_obj = car(obj);
    object *cdr_obj = cdr(obj);

    scheme_write(out, car_obj);

    if (is_pair_object(cdr_obj))
    {
        fprintf(out, " ");
        write_pair(out, cdr_obj);
    }
    else if (! is_the_empty_list(cdr_obj))
    {
        fprintf(out, " . ");
        scheme_write(out, cdr_obj);
    }
}


