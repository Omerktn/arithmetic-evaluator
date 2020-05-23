#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../arthm.h"

#define EPSILON 0.000001
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[0;32m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct TestCase {
  char* title;
  int sc;
  char** input;
  long double* target;
}TestCase;

TestCase * createCase(char* title, int maxSubCount) {
  TestCase *newcase = (TestCase *)malloc(sizeof(TestCase));
  newcase->title = strdup(title);
  newcase->sc = 0;
  newcase->input = (char**)malloc(sizeof(char*)*maxSubCount);
  newcase->target = (long double*)malloc(sizeof(long double)*maxSubCount);
  return newcase;
}

void addSubCase (TestCase* tcase, char* inp, long double tar) {
  tcase->input[tcase->sc] = strdup(inp);
  tcase->target[tcase->sc] = tar;
  tcase->sc++;
}

int nearlyEqual(long double ld1, long double ld2) {
  if (ld1 == ld2) {
    return 1;
  }
  if (fabsl(ld1 - ld2) < EPSILON) {
    return 1;
  }
  return 0;
}

void startTesting(TestCase *tcase) {
  int i;
  char failed = 0;
  long double out;
  for(i=0; i < tcase->sc; i++) {
    out = evalExpression(tcase->input[i]);

    if (!nearlyEqual(out,tcase->target[i])) {
      printf(ANSI_COLOR_RED);
      printf("Failed at ");
      printf(ANSI_COLOR_RESET);
      printf("[%s]\nExpression : [%s]\nExpected : %Lf\nGot : %Lf\n",
             tcase->title,
             tcase->input[i],
             tcase->target[i],
             out);
      failed = 1;
    }
  }
  if (!failed) {
    printf("> [%s] [", tcase->title);
    printf(ANSI_COLOR_GREEN);
    printf("Succesful");
    printf(ANSI_COLOR_RESET);
    printf("]\n");
  }
}

int main() {
  TestCase *case1 = createCase("Basic Arithmetic Operations", 10);
  addSubCase(case1, "16+4", 20);
  addSubCase(case1, "1 + 0+12", 13);
  addSubCase(case1, "45/2 + 1/2", 23);
  addSubCase(case1, "10 + 3 * (13-3*(2^2))+1-1", 13);
  addSubCase(case1, "((10) + ((5)*2)-9)", 11);
  addSubCase(case1, "10+2*-1", 8);

  TestCase *case2 = createCase("Functions and Constants", 10);
  addSubCase(case2, "sin(pi/2)", 1);
  addSubCase(case2, "sin(0.456)^2 + cos(0.456)^2", 1);
  addSubCase(case2, "log(10^9) - 2", 7);
  addSubCase(case2, "1 - ln(e*e*e*e) - 2", -5);

  TestCase *case3 = createCase("Many operations", 10);
  addSubCase(case3, "1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1-2-2-2-2-2-2-2-2-2", 0);
  addSubCase(case3, "(((((((((((((((15+(9))))))))))))))))-1+1+1+1+1+1+1+1", 30);
  addSubCase(case3, "10.1+20.2+30.3+40.4+50.5+60.6+7.70+8.80+9.90+10.10+11.11+12.12", 271.83);
  addSubCase(case3, "123+5+31*41+23*-(12/5423)*1234-493+3823*35-2", 134646.196386);
  addSubCase(case3, "123456.789+356543.66+34.2236+2423.44+6234.243/4", 484016.67335);

  startTesting(case1);
  startTesting(case2);
  startTesting(case3);
  return 0;
}
