#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "read.h"
#include "write.h"
#include "eval.h"
#include "init.h"
#include "globals.h"

#include "libgen.h"

/* From environment.c */
object *load_file(char *filename);

int main(int argc, char** argv)
{
    const char * stdlibName = "stdlib.scm";
    const char * programPath = dirname(argv[0]);
    char * fullStdLibPath = NULL;

    if (programPath == NULL)
    {
        perror("Error:");
        return 1;
    }

    fullStdLibPath = malloc(strlen(stdlibName) + strlen(programPath) + 2);

    if (fullStdLibPath == NULL)
    {
        fprintf(stderr, "Error allocating memory.\n");
        return 1;
    }

    sprintf(fullStdLibPath, "%s/%s", programPath, stdlibName);

    fprintf(stdout, "Program path: '%s'", fullStdLibPath);

    fprintf(stdout, "Welcome to Scheme. (sort of, not really finished yet.)\n"
            "Written by B.Paschen (bpaschen@googlemail.com)\n"
            "Use Ctrl-C to exit.\n");

    init(argc, argv);
    
    /* Read in the standard lib functions */
    fprintf(stdout, "Loading 'stdlib.scm'... ");
    scheme_write(stdout, load_file(fullStdLibPath));
    putc('\n', stdout);

    while (1)
    {
        fprintf(stdout, "> ");
        scheme_write(stdout, 
                     eval(scheme_read(stdin), 
                          the_global_environment));
        putc('\n', stdout);
    }

    return 0;
}

/** MUSIC **
 * Nectarine Demoscene Radio: http://www.scenemusic.net
 */
