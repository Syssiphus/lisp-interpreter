
#include <stdlib.h>

#include "write.h"

void write(FILE *out, object * obj)
{
    if (is_fixnum_object(obj))
    {
        fprintf(out, "%ld", get_fixnum_value(obj));
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
    else
    {
        fprintf(stderr, "%s, %d: Cannot write unknow object '%s'\n",
                __FILE__, __LINE__, __func__);
        exit(1);
    }
}

