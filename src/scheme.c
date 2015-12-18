#include <string.h>
#include <errno.h>

#include "read.h"
#include "write.h"
#include "eval.h"
#include "init.h"
#include "globals.h"

/* From environment.c */
object *load_file(char *filename);

int main(int argc, char** argv)
{
    fprintf(stdout, "Welcome to Scheme. (soon to be R5RS compliant)\n"
            "Written by B.Paschen (bpaschen@googlemail.com)\n"
            "Use Ctrl-C to exit.\n");

    init(argc, argv);
    
    /* Read in the standard lib functions */
    fprintf(stdout, "Loading 'stdlib.scm'... ");
    write(stdout, load_file("stdlib.scm"));
    putc('\n', stdout);

    while (1)
    {
        fprintf(stdout, "> ");
        write(stdout, eval(read(stdin), the_global_environment));
        putc('\n', stdout);
    }

    return 0;
}

/** MUSIC **
 * Nectarine Demoscene Radio: http://www.scenemusic.net
 */
