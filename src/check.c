#include "check.h"

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "data.h"

inline static   void          skip_newline      (FILE *);

inline static   char        **read_n_nbytes     (FILE *, char **, size_t *);
inline static   char         *read_nbytes       (FILE *, char *);

inline static   void          opt_exe           (t_info *, FILE *);
inline static   void          opt_description   (t_info *, FILE *);
inline static   void          opt_stdin         (t_info *, FILE *);
inline static   void          opt_filein        (t_info *, FILE *);

inline static   t_info       *fill_data         (FILE *);

inline static void
skip_newline(FILE *fp)
{ fgetc(fp); }

inline static char **
read_n_nbytes(FILE *fp, char **dst, size_t *dst_size)
{
    int *int_input_2;
    int  i, j;

    if (fscanf(fp, "%ld", dst_size) == EOF)
        err(EINVAL, "Invalid expression");

    if ((dst = (char **) malloc(*dst_size * sizeof(char *))) == NULL)
        err(errno, "fill_data: malloc data->exe");

    if ((int_input_2 = (int *) malloc(*dst_size * sizeof(int))) == NULL)
        err(errno, "fill_data: malloc int_input_2");

    for (i = 0; i < *dst_size; i++)
    {
        if (fscanf(fp, "%d", &(int_input_2[i])) == EOF)
            err(EINVAL, "Invalid expression");

        if ((dst[i] = (char *) malloc((int_input_2[i] + 1) * sizeof(char))) == NULL)
            err(errno, "fill_data: malloc data->exe[i]");
    }

    for (skip_newline(fp), i = 0; i < *dst_size; i++)
    {
        for (j = 0; j < int_input_2[i]; j++)
            if ((dst[i][j] = fgetc(fp)) == EOF)
                err(EINVAL, "Invalid expression");

        dst[i][j+1] = '\0';
    }

    return dst;
}

inline static char *
read_nbytes(FILE *fp, char *dst)
{
    int int_input;
    int i;

    if (fscanf(fp, "%d", &int_input) == EOF)
        err(EINVAL, "Invalid expression");

    if ((dst = (char *) malloc((int_input + 1) * sizeof(char))) == NULL)
        err(errno, "read_nbytes: malloc");

    for (skip_newline(fp), i = 0; i < int_input; i++)
        if ((dst[i] = fgetc(fp)) == EOF)
            err(EINVAL, "Invalid expression");
    dst[i+1] = '\0';

    return dst;
}


inline static void
opt_exe(t_info *data, FILE *fp)
{ data->exe = read_n_nbytes(fp, data->exe, &data->exe_size); }

inline static void
opt_description(t_info *data, FILE *fp)
{ data->description = read_nbytes(fp, data->description); }

inline static void
opt_stdin(t_info *data, FILE *fp)
{ data->input = read_nbytes(fp, data->input); }

inline static void
opt_filein(t_info *data, FILE *fp)
{
    FILE *fp_input;
    char  filename[4096];
    int   i;

    data->input = read_nbytes(fp, data->input);
    strncpy(filename, data->input, 4096 * sizeof(char));

    if ((fp_input = fopen(filename, "r")) == NULL)
        err(errno, "opt_filein: fopen");

    for (i = 0; (data->input[i] = fgetc(fp_input)) != EOF; i++);

    fclose(fp);
}

inline static void
opt_stdout(t_info *data, FILE *fp)
{ data->expected_output = read_n_nbytes(fp, data->expected_output, &data->expected_output_size); }

inline static t_info *
fill_data(FILE *fp)
{
    t_info *data;
    char    str_input[65536];

    if ((data = (t_info *) malloc(sizeof(t_info))) == NULL)
        err(errno, "fill_data: malloc data");

    while (fscanf(fp, "%s", &(str_input[0])) != EOF)
    {
        if (! strncmp(str_input, "-----", 5))
        {
            if (fscanf(fp, "%s", &(str_input[0])) == EOF)
                err(EINVAL, "Invalid expression");

            if (! strncmp(str_input, "exe", 3))
                opt_exe(data, fp);

            if (! strncmp(str_input, "description", 11))
                opt_description(data, fp);

            if (! strncmp(str_input, "stdin", 5))
                opt_stdin(data, fp);

            if (! strncmp(str_input, "filein", 6))
                opt_filein(data, fp);

            if (! strncmp(str_input, "stdout", 6))
                opt_stdout(data, fp);

            //if (! strncmp(str_input, "fileout", 7))
                //opt_fileout(data, fp);

            if (! strncmp(str_input, "return", 6))
                if (fscanf(fp, "%d", &(data->return_code)) == EOF)
                    err(EINVAL, "Invalid expression");
        }
    }

    return data;
}

extern t_info *
assign_test(const char *test_file)
{
    t_info *data;
    FILE   *fp;

    if ((fp = fopen(test_file, "r")) == NULL)
        err(errno, "assign_test: fopen");

    data = fill_data(fp);

    fclose(fp);

    printf("exe: %s-----\n", data->exe[0]);
    printf("description:\n%s-----\n", data->description);
    printf("input:\n%s-----\n", data->input);
    printf("output:\n%s-----\n", data->expected_output[0]);
    printf("return: %d-----\n", data->return_code);

    if ((fp = fopen("/tmp/pcb-input", "w")) == NULL)
        err(errno, "assign_test: fopen");
    for (int i = 0; data->input[i] != '\0'; i++)
        fprintf(fp, "%c", data->input[i]);
    fclose(fp);

    execl(data->exe[0], "< /tmp/pcb-input > /tmp/pcb-result", NULL);

    if ((fp = fopen("/tmp/pcb-result", "r")) == NULL)
        err(errno, "assign_test: fopen");

    char output[4096]; int i;
    for (i = 0; (output[i] = fgetc(fp)) != EOF; i++);
    output[i+1] = '\0';

    size_t          output_size = strlen(output);
    size_t expected_output_size = strlen(data->expected_output[0]);

    if (output_size == expected_output_size)
    {
        if (! strncmp(data->input, data->expected_output[0], output_size))
            printf("Passou nos testes !\n");
        else
            printf("NÃO passou nos testes !\n");
    }
    else
        printf("Entrada e Saída possuem tamanhos diferentes.\n");

    fclose(fp);

    free_tinfo(data);

    /* success */
    return data;
}
