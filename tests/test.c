#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arthm.h"

extern long double evalExpression(char *);

int main() {
  char* exp = strdup("16+4");
  long double var = evalExpression(exp);
  printf("I am working and answer is: %Lf\n", var);
  return 0;
}
