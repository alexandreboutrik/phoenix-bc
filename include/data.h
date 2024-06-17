#ifndef _DATA_PHOENIX_BC_H
#define _DATA_PHOENIX_BC_H

#include <stdio.h>

/*
 * Test file information
 */
typedef struct t_info t_info;
struct t_info
{
    char    **exe;              /* (N, Nb_i) where i E [0; N[ */
    char     *description;      /* (Nb) */
    char     *input;            /* (Nb) */
    char    **expected_output;  /* (N, Nb_i) where i E [0; N[ */
    int       return_code;      /* (N) */

    char     *output;

    size_t    exe_size, expected_output_size;
};

extern  void        free_tinfo      (t_info *);

#endif /* _DATA_PHOENIX_BC_H */
