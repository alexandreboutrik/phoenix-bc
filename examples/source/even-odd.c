#include <stdio.h>

int main(void) {

  int input;

  while (scanf("%d", &input) != EOF)
    printf("%d ", input % 2);
  printf("\e[1D\n");
}
