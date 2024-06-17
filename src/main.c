/*
 * Phoenix Behavior Checker
 *  by Alexandre Boutrik
 *
 * Read LICENSE for licensing information
 */

/*
 * there should be one test per input
 * controls:
 *  - exe N Nb...       - N executable path of Nb bytes to execute with the input
 * +- description Nl    - Nl lines of description text
 *
 *  - stdin Nb          - Nb bytes given as the stdin
 *  - filein Nb         - read Nb bytes of a file to serve as the stdin
 *
 *  - stdout N Nb...    - N expected outputs of Nbi bytes each
 *  - fileout N Nb...   - read Nb bytes of N files to serve as the stdout
 *
 *  - return N          - expected return code
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "check.h"

int
main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "c:")) != -1)
    {
        switch(opt)
        {
            case 'C':
            case 'c':
                assign_test(optarg);
                break;

            default:
                fprintf(stderr, "Usage: %s [-c .test]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}
