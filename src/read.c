
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "read.h"
#include "globals.h"
#include "object.h"
#include "memory.h"
#include "builtins.h"

#ifdef DEBUGGING_ON
#define parser_state(p) _parser_state(p, __FILE__, __LINE__, __func__)
#else
#define parser_state(x)
#endif

typedef struct
{
    FILE *stream;
    char buffer[4096];
    size_t buffer_pos;
} parser_t;

typedef object*(*parser_function_t)(parser_t*);
typedef int(*func_t)(int);

int peek(FILE *in);
int get_char(parser_t *p);

void eat_whitespace(FILE *in);

void parser_rollback(parser_t *p);

char many_of(parser_t *p, func_t func);
char maybe_many_of(parser_t *p, func_t func);
char one_of(parser_t *p, func_t func);
char maybe_one_of(parser_t *p, func_t func);
char char_of(parser_t *p, const char c);
char any_char(parser_t *p);
char follows_one_of(parser_t *p, func_t func);
char follows_char_of(parser_t *p, const char c);

int is_delimiter(int c);
int is_sign(int c);
int is_special_initial(int c);
int is_subsequent(int c);
int is_initial(int c);
int is_special_subsequent(int c);

object *parse_symbol(parser_t *p);
object *parse_boolean(parser_t *p);
object *parse_integer(parser_t *p);
object *parse_double(parser_t *p);
object *parse_character(parser_t *p);
object *parse_string(parser_t *p);
object *parse_vector(parser_t *p);
object *parse_pair(parser_t *p);
object *parse_quote(parser_t *p);
object *parse_eof(parser_t *p);
object *read_pair(parser_t *p);

object *return_error(parser_t *p);

void _parser_state(parser_t *p, const char *f, unsigned int l, 
        const char *func);

/**
 * Reads a stream and creates an object.
 */
object *read(FILE *in)
{
    unsigned long i;
    parser_t p;
    parser_function_t parser_functions[] = {
        parse_symbol,
        parse_boolean,
        parse_double,
        parse_integer,
        parse_character,
        parse_string,
        parse_vector,
        parse_pair,
        parse_quote,
        parse_eof
    };

    p.stream = in;
    memset(p.buffer, 0x00, sizeof(p.buffer));
    p.buffer_pos = 0;

    eat_whitespace(in);

    for (i = 0; 
            i < (sizeof(parser_functions) / sizeof(parser_function_t)); 
            ++i)
    {
        object * o = parser_functions[i](&p);
        if (o) return o;
    }

    return return_error(&p);
}

object *return_error(parser_t *p)
{
    int c = getc(p->stream);
    if (fpurge(p->stream) == -1)
    {
        /* Purge error */
        fprintf(stderr, "Error during purge: %s\n", strerror(errno));
        exit(1);
    }
    return make_error("Parse error at '%c' (%d)", c, c);
}

int peek(FILE *in)
{
    int c = getc(in);
    ungetc(c, in);
    return c;
}

int get_char(parser_t *p)
{
    int c = getc(p->stream);
    p->buffer[p->buffer_pos++] = c; /* TODO: memory management */
    return c;
}

void eat_whitespace(FILE *in)
{
    int c;
    while ((c = getc(in)) != EOF)
    {
        if (isspace(c))
        {
            continue;
        }
        else if (c == ';')
        {
            /* comment till the end of the line */
            while ((c = getc(in)) != EOF && c != '\n');
            continue;
        }
        ungetc(c, in);
        break;
    }
}

void parser_rollback(parser_t *p)
{
    while (p->buffer_pos > 0)
    {
        ungetc(p->buffer[--p->buffer_pos], p->stream);
        p->buffer[p->buffer_pos] = '\0';
    }
}

char many_of(parser_t *p, func_t func)
{
    int c = peek(p->stream);
    if ( ! func(c))
    {
        return 0;
    }
    while (func(c))
    {
        get_char(p);
        c = peek(p->stream);
    }
    return 1;
}

char maybe_many_of(parser_t *p, func_t func)
{
    many_of(p, func);
    return 1;
}

char one_of(parser_t *p, func_t func)
{
    int c = peek(p->stream);
    if ( ! func(c))
    {
        return 0;
    }
    get_char(p);
    return 1;
}

char maybe_one_of(parser_t *p, func_t func)
{
    one_of(p, func);
    return 1;
}

char follows_one_of(parser_t *p, func_t func)
{
    int c = peek(p->stream);
    return func(c) ? 1 : 0;
}

char follows_char_of(parser_t *p, const char c)
{
    return peek(p->stream) == c ? 1 : 0;
}

char char_of(parser_t *p, const char c)
{
    int cc = peek(p->stream);
    if (cc == c)
    {
        get_char(p);
        return 1;
    }
    return 0;
}

char any_char(parser_t *p)
{
    get_char(p);
    return 1;
}

char string_of(parser_t *p, const char *str)
{
    size_t s = strlen(str);
    size_t i;

    for (i = 0; i < s; ++i)
    {
        if (peek(p->stream) != str[i])
        {
            return 0;
        }
        get_char(p);
    }

    return 1;
}

int is_delimiter(int c)
{
    return isspace(c) || c == '(' || c == ')' || c == '"' || c == ';';
}

int is_sign(int c)
{
    return c == '-' || c == '+';
}

int is_special_initial(int c)
{
    return c == '!' || c == '$' || c == '%' || c == '&' || c == '*'
        || c == '/' || c == ':' || c == '<' || c == '=' || c == '>'
        || c == '?' || c == '^' || c == '_' || c == '~';
}

int is_subsequent(int c)
{
    return is_initial(c) || isdigit(c) || is_special_subsequent(c);
}

int is_initial(int c)
{
    return isalpha(c) || is_special_initial(c);
}

int is_special_subsequent(int c)
{
    return c == '+' || c == '-' || c == '.' || c == '@';
}

object *parse_integer(parser_t *p)
{
    if (maybe_one_of(p, is_sign) && many_of(p, isdigit) 
            && follows_one_of(p, is_delimiter))
    {
        return make_fixnum(atol(p->buffer));
    }

    parser_rollback(p);
    return NULL;
}

object *parse_double(parser_t *p)
{
    if (maybe_one_of(p, is_sign) && maybe_many_of(p, isdigit) 
            && char_of(p, '.') && many_of(p, isdigit) 
            && follows_one_of(p, is_delimiter))
    {
        return make_realnum(atof(p->buffer));
    }

    parser_rollback(p);
    return NULL;
}

object *parse_string(parser_t *p)
{
    /* TODO: change this function so it fits the other parse functions
     * principles */
    int c = peek(p->stream);
    /* Check for string opener */
    if (c != '"')
    {
        return NULL;
    }
    c = getc(p->stream);

    /* Read string elements (zero elements are ok too)*/
    while ((c = getc(p->stream)) != '"' && c != EOF)
    {
        if (c == '\\')
        {
            c = getc(p->stream);
            switch (c)
            {
                case '"':
                    p->buffer[p->buffer_pos++] = '"';
                    break;
                case '\\':
                    p->buffer[p->buffer_pos++] = '\\';
                    break;
                default:
                    parser_rollback(p);
                    ungetc('"', p->stream);
                    return NULL;
            }
        }
        else
        {
            p->buffer[p->buffer_pos++] = c;
        }
    }

    if ((c != '"') && is_delimiter(peek(p->stream)))
    {
        ungetc(c, p->stream);
        parser_rollback(p);
        ungetc('"', p->stream);
        return NULL;
    }

    return make_string(p->buffer);
}

object *parse_symbol(parser_t *p)
{
    if (one_of(p, is_initial) && maybe_many_of(p, is_subsequent)
            && follows_one_of(p, is_delimiter))
    {
        return make_symbol(p->buffer);
    }
    parser_rollback(p);

    /* Else check for the 'peculiar' identifier ('+' | '-' | '...') */

    if ((char_of(p, '+') || char_of(p, '-') 
            || (char_of(p, '.') && char_of(p, '.') && char_of(p, '.')))
            && follows_one_of(p, is_delimiter))
    {
        return make_symbol(p->buffer);
    }

    parser_rollback(p);
    return NULL;
}

object *parse_boolean(parser_t *p)
{
    if (char_of(p, '#'))
    {
        if (char_of(p, 't') && follows_one_of(p, is_delimiter))
        {
            return true;
        }
        else if (char_of(p, 'f') && follows_one_of(p, is_delimiter))
        {
            return false;
        }
    }

    parser_rollback(p);
    return NULL;
}

object *parse_character(parser_t *p)
{
    if (char_of(p, '#') && char_of(p, '\\') && any_char(p))
    {
        if (follows_one_of(p, is_delimiter))
        {
            /* Just the char value */
            return make_character(p->buffer[p->buffer_pos - 1]);
        }
        else if (p->buffer[p->buffer_pos - 1] == 's'
                && string_of(p, "pace")
                && follows_one_of(p, is_delimiter))
        {
            /* 'space' character code */
            strcpy(p->buffer, "#\\ ");
            p->buffer_pos = strlen(p->buffer);
            return make_character(p->buffer[p->buffer_pos - 1]);
        }
        else if (p->buffer[p->buffer_pos - 1] == 'n'
                && string_of(p, "ewline")
                && follows_one_of(p, is_delimiter))
        {
            /* 'newline' character code */
            strcpy(p->buffer, "#\\\n");
            p->buffer_pos = strlen(p->buffer);
            return make_character(p->buffer[p->buffer_pos - 1]);
        }
    }

    parser_rollback(p);
    return NULL;
}

object *parse_eof(parser_t *p)
{
    if (char_of(p, EOF))
    {
        return make_eof(p->stream);
    }

    return NULL;
}

object *parse_vector(parser_t *p)
{
    if (char_of(p, '#') && char_of(p, '('))
    {
        object *list_obj;
        object *tmp_list_obj;
        object *vector_obj;
        unsigned long i = 0;
        unsigned long length = 0;
        
        /* Read the vector as a list and convert it to a 
           vector object */
        /* FIXME: Make this better :) */
        list_obj = read_pair(p);
        
        /* count items :( */
        tmp_list_obj = list_obj;
        while ( ! is_the_empty_list(tmp_list_obj))
        {
            length++;
            tmp_list_obj = cdr(tmp_list_obj);
        }
        
        /* Create vector and push list to vector */
        vector_obj = make_vector(length);
        while ( ! is_the_empty_list(list_obj))
        {
            set_vector_item(vector_obj, i, car(list_obj));
            i++;
            list_obj = cdr(list_obj);
        }
        return vector_obj;
    }

    parser_rollback(p);
    return NULL;
}

object *parse_pair(parser_t *p)
{
    if (char_of(p, '('))
    {
        return read_pair(p);
    }

    return NULL;
}

object *read_pair(parser_t *p)
{
    object * car_obj;
    object * cdr_obj;

    maybe_many_of(p, isspace);

    if (char_of(p, ')'))
    {
        return the_empty_list;
    }

    car_obj = read(p->stream);
    if (car_obj == NULL || is_error_object(car_obj))
    {
        return car_obj;
    }

    maybe_many_of(p, isspace);

    if (char_of(p, '.'))
    {
        /* Dotted list */
        if ( ! follows_one_of(p, is_delimiter))
        {
            return NULL;
        }
        cdr_obj = read(p->stream);
        if (cdr_obj == NULL || is_error_object(cdr_obj))
        {
            return cdr_obj;
        }
        maybe_many_of(p, isspace);
        if ( ! char_of(p, ')'))
        {
            return NULL;
        }
        return cons(car_obj, cdr_obj);
    }

    /* Proper list */
    cdr_obj = read_pair(p);
    if (cdr_obj == NULL || is_error_object(cdr_obj))
    {
        return cdr_obj;
    }
    return cons(car_obj, cdr_obj);
}

object *parse_quote(parser_t *p)
{
    if (char_of(p, '\''))
    {
        return cons(quote_symbol, cons(read(p->stream), the_empty_list));
    }
    return NULL;
}

void _parser_state(parser_t *p, const char *f, unsigned int l, 
        const char *func)
{
    fprintf(stderr, "%s, %d, %s() - "
            "parser state (\"%s\", %ld), next char ('%c')\n", f, l, func, 
            p->buffer, p->buffer_pos, peek(p->stream));
}


