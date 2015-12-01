
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "read.h"
#include "globals.h"
#include "object.h"
#include "memory.h"

typedef struct
{
    FILE *stream;
    char buffer[4096];
    size_t buffer_pos;
} parser_t;

typedef int(*func_t)(int);

int peek(FILE *in);
int get_char(parser_t *p);

void eat_whitespace(FILE *in);

void parser_rollback(parser_t *p);

char maybe_one_of(parser_t *p, func_t func);
char many_of(parser_t *p, func_t func);
char one_of(parser_t *p, func_t func);
char char_of(parser_t *p, const char c);
char follows_one_of(parser_t *p, func_t func);

int is_delimiter(int c);
int is_sign(int c);
int is_special_initial(int c);
int is_subsequent(int c);
int is_initial(int c);
int is_special_subsequent(int c);

char parse_symbol(parser_t *p);
char parse_boolean(parser_t *p);
char parse_integer(parser_t *p);
char parse_character(parser_t *p);
char parse_string(parser_t *p);

/**
 * Reads a stream and creates an object.
 */
object *read(FILE *in)
{
    parser_t p;
    p.stream = in;
    memset(p.buffer, 0x00, sizeof(p.buffer));
    p.buffer_pos = 0;

    eat_whitespace(in);

    if (parse_symbol(&p))
    {
        return make_symbol(p.buffer);
    }
    else if (parse_boolean(&p))
    {
        if (p.buffer[p.buffer_pos - 1] == 't')
        {
            return true;
        }
        return false;
    }
    else if (parse_integer(&p))
    {
        return make_fixnum(atol(p.buffer));
    }
    else if (parse_character(&p))
    {
        return make_character(p.buffer[p.buffer_pos - 1]);
    }
    else if (parse_string(&p))
    {
        return make_string(p.buffer);
    }

    fprintf(stderr, "Unknown input at '%c'\n", get_char(&p));
    exit(1);
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
    }
}

char maybe_one_of(parser_t *p, func_t func)
{
    int c = peek(p->stream);
    if (func(c))
    {
        get_char(p);
    }
    return 1;
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

char follows_one_of(parser_t *p, func_t func)
{
    int c = peek(p->stream);
    return func(c) ? 1 : 0;
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

char string_of(parser_t *p, const char *str)
{
    size_t s = strlen(str);
    int i;

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

char parse_integer(parser_t *p)
{
    if (maybe_one_of(p, is_sign) && many_of(p, isdigit) 
            && follows_one_of(p, is_delimiter))
    {
        return 1;
    }

    parser_rollback(p);
    return 0;
}

char parse_string(parser_t *p)
{
    /* TODO: change this function so it fits the other parse functions
     * principles */
    int c = peek(p->stream);
    /* Check for string opener */
    if (c != '"')
    {
        return 0;
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
                    return 0;
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
        return 0;
    }

    return 1;
}

char parse_symbol(parser_t *p)
{
    if (one_of(p, is_initial) && many_of(p, is_subsequent)
            && follows_one_of(p, is_delimiter))
    {
        return 1;
    }

    parser_rollback(p);
    return 0;
}

char parse_boolean(parser_t *p)
{
    int c = getc(p->stream);
    if (c != '#')
    {
        ungetc(c, p->stream);
        return 0;
    }

    c = getc(p->stream);
    if ((c == 't' || c == 'f') && is_delimiter(peek(p->stream)))
    {
        p->buffer[p->buffer_pos++] = c;
        return 1;
    }

    ungetc(c, p->stream);
    ungetc('#', p->stream);
    return 0;
}

char parse_character(parser_t *p)
{
    if (char_of(p, '#') && char_of(p, '\\') 
            && (one_of(p, isalpha) || one_of(p, isdigit) 
                || char_of(p, ' ') || char_of(p, '\n')))
    {
        if (follows_one_of(p, is_delimiter))
        {
            /* Just the char value */
            return 1;
        }
        else if (p->buffer[p->buffer_pos - 1] == 's'
                && string_of(p, "pace")
                && follows_one_of(p, is_delimiter))
        {
            /* 'space' character code */
            strcpy(p->buffer, "#\\ ");
            p->buffer_pos = strlen(p->buffer);
            return 1;
        }
        else if (p->buffer[p->buffer_pos - 1] == 'n'
                && string_of(p, "ewline")
                && follows_one_of(p, is_delimiter))
        {
            /* 'newline' character code */
            strcpy(p->buffer, "#\\\n");
            p->buffer_pos = strlen(p->buffer);
            return 1;
        }
    }

    parser_rollback(p);
    return 0;
}







