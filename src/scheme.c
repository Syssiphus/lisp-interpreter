
#include "read.h"
#include "write.h"
#include "eval.h"
#include "init.h"
#include "globals.h"

int main(int argc, char** argv)
{
    fprintf(stdout, "Welcome to Scheme. (R5RS)\n"
            "Written by B.Paschen (bpaschen@googlemail.com)\n"
            "Use Ctrl-C or '(quit)' to exit.\n");

    init();

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
