
#include <stdlib.h>
#include <ctype.h>

#include "read.h"
#include "globals.h"
#include "object.h"
#include "memory.h"

void skip_whitespace(FILE *in);
char is_delimiter(int c);
int peek(FILE *in);
char read_character(FILE *in);
void eat_string(FILE *in, char *str);
char *read_string(FILE *in);

/**
 * Reads a stream and creates an object.
 */
object *read(FILE *in)
{
    int c;

    skip_whitespace(in);

    c = getc(in);

    if (c == '#')
    {
        /* A character value */
        c = getc(in);

        switch (c)
        {
            case '\\': return make_character(read_character(in));
            case 't': return true;
            case 'f': return false;
            default:
                fprintf(stderr, "%s, %d: Syntax error, illegal character '%c' "
                        "\n", __FILE__, __LINE__, c);
                exit(1);
        }
    }
    else if (isdigit(c) || (c == '-' && (isdigit(peek(in)))))
    {
        /* Numbers, TODO: implement more than fixnums*/
        short sign = 1;
        long num = 0;

        if (c == '-')
        {
            sign = -1; /* Number is negative */
        }
        else
        {
            ungetc(c, in);
        }

        while (isdigit(c = getc(in)))
        {
            num = (num * 10) + (c - '0');
        }

        num *= sign;

        if (is_delimiter(c))
        {
            ungetc(c, in);
            return make_fixnum(num);
        }
        else
        {
            fprintf(stderr, "Number not followed by delimiter\n");
            exit(1); /* TODO: Error handling */
        }
    }
    else if (c == '"')
    {
        char   *str = read_string(in);
        object *obj = make_string(str);
        free(str); /* Get's allocated in read_string(), 
                      needs to be freed here */
        return obj;
    }
    else
    {
        fprintf(stderr, "%s, %d: Couldn't parse input.\n", __FILE__, __LINE__);
        return false;
    }

    fprintf(stderr, "%s, %d: Illegal state in '%s'", 
            __FILE__, __LINE__, __func__);
    exit(1);
}

/**
 * Skips whitespace and comments.
 */
void skip_whitespace(FILE *in)
{
    int c;

    while ((c = getc(in)) != EOF)
    {
        if (isspace(c))
        {
            continue;
        }
        else if (c == ';') /* Skip comments till the end of the line */
        {
            while(((c = getc(in)) != EOF) && (c != '\n'));
            continue;
        }

        ungetc(c, in);
        break;
    }
}

/**
 * Peek into the stream and return the character which comes next.
 */
int peek(FILE *in)
{
    int c;

    c = getc(in);
    ungetc(c, in);

    return c;
}

/**
 * Is it a delimiter?
 */
char is_delimiter(int c)
{
    return isspace(c) || c == '(' || c == ')'
        || c == EOF || c == '"' || c == ';';
}

/**
 * Read a scheme character value from the stream.
 * This includes the keywords 'space' and 'newline' which are mapped to
 * ' ' and '\n' respectively.
 */
char read_character(FILE *in)
{
    char c;
    c = getc(in);

    if (c == 's')
    {
        if (peek(in) == 'p')
        {
            eat_string(in, "pace");
            c = ' ';
        }
    }
    else if (c == 'n')
    {
        if (peek(in) == 'e')
        {
            eat_string(in, "ewline");
            c = '\n';
        }
    }

    if(! is_delimiter(peek(in)))
    {
        fprintf(stderr, "Missing delimiter.\n");
        exit(1);
    }

    return c;
}

/**
 * Eat a string value from the stream.
 */
void eat_string(FILE *in, char *str)
{
    int c;

    while (*str != '\0')
    {
        if ((c = getc(in)) != *str)
        {
            fprintf(stderr, "%s, %d: Unexpected character '%c' in '%s'.\n",
                    __FILE__, __LINE__, c, __func__);
            exit(1);
        }
        ++str;
    }
}

/**
 * Read a string value from the stream.
 */
char *read_string(FILE *in)
{
#define STRING_BLOCK_SIZE 128
    int c, count = 0;
    char *buffer;
    size_t buffer_size = STRING_BLOCK_SIZE;

    buffer = malloc(STRING_BLOCK_SIZE);

    if (buffer == NULL)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }

    while ((c = getc(in)) != '"')
    {
        if (count == buffer_size - 1)
        {
            buffer_size += STRING_BLOCK_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL)
            {
                fprintf(stderr, "Out of memory.\n");
                exit(1);
            }
        }

        if (c == '\\')
        {
            int cc = peek(in);
            switch (cc)
            {
                case '"':
                    /* Escaped double quote */
                    c = getc(in);
                    break;
                case 'n':
                    /* Newline */
                    cc = getc(in);
                    c = '\n';
                    break;
                case 't':
                    /* Tab character */
                    cc = getc(in);
                    c = '\t';
                    break;
                case '\\':
                    c = getc(in);
                    break;
                default:
                    fprintf(stderr, "Unknown escaped character '%c'\n", cc);
                    exit(1);
            }
        }

        buffer[count++] = c;
    }

    if ( ! is_delimiter(peek(in)))
    {
        fprintf(stderr, "Missing delimiter after string.\n");
        exit(1);
    }

    buffer[count] = '\0';
    return buffer;
}

