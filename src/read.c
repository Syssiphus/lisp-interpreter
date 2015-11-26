
#include <stdlib.h>
#include <ctype.h>

#include "read.h"

void skip_whitespace(FILE *in);

/**
 * Reads a stream and creates an object.
 */
object *read(FILE *in)
{
    int c;
    short sign = 1;

    skip_whitespace(in);

    c = getc(in);

    if (isdigit(c) || (c == '-' && (isdigit(peek(in)))))
    {
        /* Numbers, TODO: implement more than fixnums*/
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

