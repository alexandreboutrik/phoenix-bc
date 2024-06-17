#include "data.h"

#include <stdlib.h>
#include <stdio.h>

extern void
free_tinfo(t_info *data)
{
    if (data == NULL)
        return;

    int i;

    if (data->exe != NULL)
    {
        for (i = 0; i < data->exe_size; i++)
            if (data->exe[i] != NULL)
                free(data->exe[i]);

        free(data->exe);
    }

    if (data->description != NULL)
        free(data->description);
    if (data->input != NULL)
        free(data->input);

    if (data->expected_output != NULL)
    {
        for (i = 0; i < data->expected_output_size; i++)
            if (data->expected_output[i] != NULL)
                free(data->expected_output[i]);

        free(data->expected_output);
    }

    if (data != NULL)
        free(data);
}
