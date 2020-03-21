#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK 2056
#define MAX_CHAR_IN 2056
#define PI 3.14159265358979323846
#define EU 2.71828182845904523536

#define MIN(X, Y) (((X) > (Y)) ? (Y) : (X))

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[01;33m" //\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct {
  float item[MAX_STACK];
  int top;
} STACK;

// Stack functions

void initStack(STACK *s) { s->top = 0; }

int isEmpty(STACK *s) {
  if (!s->top) {
    return 1;
  } else {
    return 0;
  }
}

int isFull(STACK *s) {
  if (s->top == MAX_STACK) {
    return 1;
  } else {
    return 0;
  }
}

int push(STACK *s, float x) {
  if (isFull(s)) {
    return 0;
  }

  s->item[s->top] = x;
  s->top++;
  return 1;
}

int pop(STACK *s, float *x) {
  if (isEmpty(s)) {
    return 0;
  }

  s->top--;
  *x = s->item[s->top];
  return 1;
}

int peek(STACK *s, float *x) {
  if (isEmpty(s)) {
    return 0;
  }
  *x = s->item[s->top - 1];
  return 1;
}

void printStack(STACK *s) {
  int i;
  for (i = 0; i < s->top; i++) {
    printf("%c, ", (int)s->item[i]);
  }
  printf("\n");
}

// Arithmetic eval functions

/* Return 1 if ch1 has higher priority */
int isPrior(char ch1, char ch2) {
  char chList[6] = {'(', '^', '*', '/', '+', '-'};
  char chPrio[6] = {4, 3, 2, 2, 1, 1};
  short prv1 = -1, prv2 = -1;
  int i = 0;

  while (i < 6) {
    if (ch1 == chList[i]) {
      prv1 = chPrio[i];
    }
    if (ch2 == chList[i]) {
      prv2 = chPrio[i];
    }
    i++;
  }

  printf(ANSI_COLOR_RED);
  if (prv1 == -1) {
    printf("Unrecognized symbol '%c'\n", ch1);
  } else if (prv2 == -1) {
    printf("Unrecognized symbol '%c'\n", ch2);
  }
  printf(ANSI_COLOR_RESET);

  return (prv1 >= prv2);
}

int isContain(char ch, char *in) {
  int i=0, ilen=strlen(in);
  while(i < ilen) {
    if (in[i] == ch) {
      return 1;
    }
    i++;
  }
  return 0;
}

/* Takes a string, returns an array of strings, e.g. "260+(4)" -> {"260","+","(","4",")"} */
char **strToArray(char *in, int slen, int *ilen, float formerAns) {
  char **str = (char **)malloc(sizeof(char *) * slen);
  char tmp[MAX_CHAR_IN];
  int i = 0, arrIndex = 0, t = 0;

  for (i = 0; i < slen; i++) {
    if (isdigit(in[i]) || in[i] == '.') {
      tmp[t++] = in[i];
    }
    else if (isContain(tolower(in[i]), "mpe")){
      if (t == 0) {
        switch(tolower(in[i])) {
        case 'm':
          sprintf(tmp, "%f", formerAns);
          break;
        case 'e':
          sprintf(tmp, "%f", EU);
          break;
        case 'p':
          sprintf(tmp, "%f", PI);
          break;
        }
        str[arrIndex] = (char *)malloc(sizeof(char) * (strlen(tmp)+1));
        strcpy(str[arrIndex++], tmp);
        t = 0;
      }
      else {
        printf(ANSI_COLOR_RED);
        printf("Syntax error near ");
        printf(ANSI_COLOR_RESET);
        printf("%c\n",in[i]);
      }
    }
    else {
      if (t) {
        tmp[t++] = '\0';
        str[arrIndex] = (char *)malloc(sizeof(char) * (t + 1));
        strcpy(str[arrIndex++], tmp);
      }

      str[arrIndex] = (char *)malloc(sizeof(char) * 16);
      str[arrIndex][0] = in[i];
      str[arrIndex][1] = '\0';
      arrIndex++;
      t = 0;
    }
  }
  if (t) {
    tmp[t] = '\0';
    str[arrIndex] = (char *)malloc(sizeof(char) * (t + 1));
    strcpy(str[arrIndex++], tmp);
  }

  *ilen = arrIndex;
  str = (char **)realloc(str, sizeof(char *) * arrIndex);
  return str;
}

char **infixToPostfix(char **in, int ilen, int *plen) {
  char **postf = (char **)malloc(sizeof(char *) * ilen);
  STACK *s = (STACK *)malloc(sizeof(STACK));
  int i, p = 0;
  float tmp;
  char *op;
  initStack(s);

  for (i = 0; i < ilen; i++) {
    op = in[i];

    if (isdigit(op[0])) {
      // An operand
      postf[p] = (char *)malloc(sizeof(char) * strlen(op));
      strcpy(postf[p++], op);

    } else if (!strcmp(op, ")")) {
      peek(s, &tmp);
      while (!isEmpty(s) && (char)tmp != '(') {
        pop(s, &tmp);
        postf[p] = (char *)malloc(sizeof(char) * 3);
        postf[p][0] = tmp;
        postf[p++][1] = '\0';
        peek(s, &tmp);
      }
      pop(s, &tmp);
    } else if (!isEmpty(s)) {
      peek(s, &tmp);
      while (isPrior((char)tmp, op[0]) && !isEmpty(s) && (char)tmp != '(') {
        pop(s, &tmp);
        postf[p] = (char *)malloc(sizeof(char) * 3);
        postf[p][0] = tmp;
        postf[p++][1] = '\0';
        peek(s, &tmp);
      }
      push(s, (int)op[0]);
    } else {
      push(s, (int)op[0]);
    }
  }

  while (!isEmpty(s)) {
    pop(s, &tmp);
    postf[p] = (char *)malloc(sizeof(char) * 3);
    postf[p][0] = tmp;
    postf[p++][1] = '\0';
  }

  free(s);
  *plen = p;
  // postf = (char **)realloc(postf, sizeof(char) * p);
  return postf;
}

float perform(char op, float op1, float op2) {
  float ans;

  switch (op) {
  case '+':
    ans = op1 + op2;
    break;
  case '-':
    ans = op1 - op2;
    break;
  case '/':
    ans = op1 / op2;
    break;
  case '*':
    ans = op1 * op2;
    break;
  case '^':
    ans = pow(op1, op2);
    break;
  default:
    ans = 0;
    break;
  }
  return ans;
}

float evalPostfix(char **postf, int plen) {
  STACK *s = (STACK *)malloc(sizeof(STACK));
  int i;
  float op1, op2;

  for (i = 0; i < plen; i++) {
    if (isdigit(postf[i][0])) {
      push(s, atof(postf[i]));
    } else {
      pop(s, &op2);
      pop(s, &op1);
      push(s, perform(postf[i][0], op1, op2));
    }
  }

  pop(s, &op1);
  free(s);
  return op1;
}

/* Handles some minus operator cases, e.g."-4+1" */
char *preProcess(char *infix) {
  char *clean = (char *)malloc(sizeof(char) * MAX_CHAR_IN);
  int i = 0, c = 0;

  // Put zero to beginning, if starts with "-"
  if (infix[0] == '-') {
    clean[c++] = '0';
  }

  while (infix[i] != '\0') {
    // Put zero before negative leftsided numbers
    if (i > 0 && infix[i] == '-' && (infix[i-1] == '(')) {
      clean[c++] = '0';
    }
    // Aceppts all chars but space
    if (infix[i] != ' ') {
      clean[c++] = infix[i];
    }
    i++;
  }
  clean[c] = '\0';

  return clean;
}

/* Returns 1 if the expression has balanced pharentheses*/
int validCheck(char *in, int ilen) {
  STACK *s = (STACK *)malloc(sizeof(STACK));
  float ch;
  int i;
  printf(ANSI_COLOR_RED);

  for (i = 0; i < ilen; i++) {
    if (in[i] == '(') {
      push(s, in[i]);
    } else {
      if (in[i] == ')') {
        if (!isEmpty(s)) {
          pop(s, &ch);
        } else {
          printf("Syntax Error: Unbalanced Pharentheses\n");
          free(s);
          return 0;
        }
      }
    }
  }
  if (!isEmpty(s)) {
    printf("Syntax Error: Unbalanced Pharentheses\n");
    free(s);
    return 0;
  }

  printf(ANSI_COLOR_RESET);
  free(s);
  return 1;
}

int main() {
  char in[MAX_CHAR_IN], **postf;
  char **str;
  int i, plen, ilen;
  float ans, formerAns=0;
  printf(ANSI_COLOR_RESET);
  printf("[Arithmetic Expression Evaluation]\n<Type 'q' to quit.>\n");

  do {
    printf(ANSI_COLOR_RESET);
    printf("E:");
    fgets(in, MAX_CHAR_IN, stdin);
    in[strlen(in) - 1] = '\0';

    // Preprocess
    strcpy(in, preProcess(in));
    // Validation check
    if (!validCheck(in, strlen(in))) {
      continue;
    }

    str = strToArray(in, strlen(in), &ilen, formerAns);
    postf = infixToPostfix(str, ilen, &plen);

    printf("Ans:>");
    printf(ANSI_COLOR_YELLOW);

    ans = evalPostfix(postf, plen);
    if (floor(ans) == (ans)) {
      printf("%d\n", (int)ans);
    } else {
      printf("%f\n", ans);
    }
    formerAns = ans;
  } while (in[0] != 'q');

  return 0;
}
