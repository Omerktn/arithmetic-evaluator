#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "arthm.h"

#define MAX_STACK 2056
#define MAX_CHAR_IN 4096
#define PI_CONST 3.14159265358979323846
#define EU_CONST 2.71828182845904523536

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[01;33m"
#define ANSI_COLOR_RESET "\x1b[0m"

#ifdef DEBUG
#define _DEBUG 1
#else
#define _DEBUG 0
#endif

int evalStatus;
char statusInfo[256];
long double formerAns = 0;

typedef struct {
  long double item[MAX_STACK];
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

int push(STACK *s, long double x) {
  if (isFull(s)) {
    return 0;
  }

  s->item[s->top] = x;
  s->top++;
  return 1;
}

int pop(STACK *s, long double *x) {
  if (isEmpty(s)) {
    return 0;
  }

  s->top--;
  *x = s->item[s->top];
  return 1;
}

int peek(STACK *s, long double *x) {
  if (isEmpty(s)) {
    return 0;
  }
  *x = s->item[s->top - 1];
  return 1;
}

void printStack(STACK *s) {
  int i;
  for (i = 0; i < s->top; i++) {
    printf("[%c-%d], ", (int)s->item[i], (int)s->item[i]);
  }
  printf("\n");
}

// Arithmetic eval functions

/* Return 1 if ch1 has higher priority */
int isPrior(char ch1, char ch2) {
  char chList[7] = {'(', '|', '^', '*', '/', '+', '-'};
  char chPrio[7] = {5, 4, 3, 2, 2, 1, 1};
  short prv1 = -1, prv2 = -1;
  int i = 0;

  while (i < 7) {
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

/* Return 1 if the string contains the char  */
int isContain(char ch, char *in) {
  int i = 0, ilen = strlen(in);
  while (i < ilen) {
    if (in[i] == ch) {
      return 1;
    }
    i++;
  }
  return 0;
}

/* Free all the elements of string array */
void deleteExpression(char **strlist, int len) {
  int i;
  for (i = 0; i < len; i++) {
    free(strlist[i]);
  }
  free(strlist);
}

/* Return math function number, returns -1 if no function name matched */
int getFunctionNum(char *fname) {
  char *funcNames[13] = {"sin",    "cos",    "tan",    "cot",
                        "arctan", "arcsin", "arccos", "sqrt",
                         "floor", "round", "ceil", "log", "ln"};
  int i = 0;
  // Check for functions
  while (i < 13) {
    if (!strcasecmp(fname, funcNames[i])) {
      return i;
    }
    i++;
  }
  printf("Can't determined:{%s}\n", fname);
  return -1;
}

int getConstant(char *fname, long double *constVal) {
  char *constNames[3] = {"pi", "e", "m"};
  long double constVals[3] = {PI_CONST, EU_CONST, formerAns};
  int i = 0;
  // Check for constants
  while (i < 3) {
    if (!strcasecmp(fname, constNames[i])) {
      *constVal = constVals[i];
      return 1;
    }
    i++;
  }

  return 0;
}

int isnumeric(char ch) {
  if (isdigit(ch) || ch == '.') {
    return 1;
  }
  return 0;
}
int isOperator(char ch) {
  char oplist[10] = "+-*/^()";
  char it;
  int i;
  for (i = 0, it = oplist[0]; it != '\0'; i++, it = oplist[i]) {
    if (it == ch) {
      return 1;
    }
  }
  return 0;
}

/* Takes a string, returns an array of strings,
 * e.g. "260+(4)" -> {"260","+","(","4",")"} */
char ** stringToArray(char *in, int slen, int *ilen) {
  enum state { Text, Numeric };
  char ** str = (char**)malloc(sizeof(char*) * slen * 2);
  char tmp[MAX_CHAR_IN];
  int i=0, k, arrIndex=0, funcNum;
  long double constVal;

  while (i < slen) {
    strcpy(tmp, "");
    k = 0;

    if (isnumeric(in[i])) {
      // Integers and floating points
      while(isnumeric(in[i])) {
        tmp[k++] = in[i++];
      }

    } else if (isalpha(in[i])) {
      // Alpha chars e.g. sin, pi, m
      while(isalpha(in[i])) {
        tmp[k++] = in[i++];
      }
      tmp[k++] = '\0';

      funcNum = getFunctionNum(tmp);
      if (funcNum >= 0) {
        // Valid function detected
        sprintf(tmp, "%d", funcNum);
        str[arrIndex++] = strdup(tmp);
        str[arrIndex++] = strdup("|");
        continue;
      } else if (getConstant(tmp, &constVal)) {
        // Valid constant detected
        sprintf(tmp, "%Lf", constVal);
        str[arrIndex++] = strdup(tmp);
        continue;
      } else {
        // Neither constant nor function
        evalStatus = 2;
        sprintf(statusInfo, "Unknown function or constant: '%s'", tmp);
        break;
      }

    } else if (isOperator(in[i])) {
      // Operator chars
      if (i > 0 && isContain(in[i], "+-") && isContain(in[i-1], "*/")) {
        str[arrIndex++] = strdup("-1");
        str[arrIndex++] = strdup("*");
        i++;
        continue;
      }
      tmp[k++] = in[i++];

    }else {
      evalStatus = 2;
      sprintf(statusInfo, "Unknown input character: '%c'", in[i]);
      break;
    }

    tmp[k] = '\0';
    str[arrIndex++] = strdup(tmp);
  }

  *ilen = arrIndex;
  str = (char**) realloc (str, sizeof(char*)*arrIndex+16);
  return str;
}

char **infixToPostfix(char **in, int ilen, int *plen) {
  char **postf = (char **)malloc(sizeof(char *) * ilen);
  STACK *s = (STACK *)malloc(sizeof(STACK));
  int i, p = 0;
  long double tmp;
  char op[MAX_CHAR_IN];
  initStack(s);

  for (i = 0; i < ilen; i++) {
    strcpy(op, in[i]);

    if (isdigit(op[0]) || (strlen(op) > 2 && op[0] == '-')) {
      // A digit
      postf[p] = (char *)malloc(sizeof(char) * strlen(op));
      strcpy(postf[p++], op);

    } else if (!strcmp(op, ")")) {
      peek(s, &tmp);
      while (!isEmpty(s) && (char)tmp != '(') {
        pop(s, &tmp);
        postf[p] = (char *)malloc(sizeof(char) * 16);
        postf[p][0] = tmp;
        postf[p++][1] = '\0';
        peek(s, &tmp);
      }
      pop(s, &tmp);
    } else if (!isEmpty(s)) {
      peek(s, &tmp);
      while (isPrior((char)tmp, op[0]) && !isEmpty(s) && (char)tmp != '(') {
        pop(s, &tmp);
        postf[p] = (char *)malloc(sizeof(char) * 16);
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
    postf[p] = (char *)malloc(sizeof(char) * 16);
    postf[p][0] = tmp;
    postf[p++][1] = '\0';
  }

  free(s);
  *plen = p;
  return postf;
}

long double mathFunction(int func_num, long double input) {
  long double ans;

  switch (func_num) {
  case 0:
    // 0:Sin 1:Cos 2:Tan 3:Cos
    ans = sin(input);
    break;
  case 1:
    ans = cos(input);
    break;
  case 2:
    ans = tan(input);
    break;
  case 3:
    ans = cos(input) / sin(input);
    break;
  case 4:
    // 4:Arc-tangent, 5:Arc-sinus, 6:Arc-cosinus
    ans = atan(input);
    break;
  case 5:
    ans = asin(input);
    break;
  case 6:
    ans = acos(input);
    break;
  case 7:
    // Square root
    ans = sqrt(input);
    break;
  case 8:
    // 8: Floor, 9: Round, 10: Ceil
    ans = floor(input);
    break;
  case 9:
    ans = round(input);
    break;
  case 10:
    ans = ceil(input);
    break;
  case 11:
    // log base 10
    ans = log10(input);
    break;
  case 12:
    // ln
    ans = log(input);
    break;
  default:
    ans = 0;
    break;
  }

  return ans;
}

long double perform(char op, long double op1, long double op2) {
  long double ans;

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
  case '|':
    ans = mathFunction(op1, op2);
    break;
  default:
    ans = 0;
    evalStatus = 3;
    strcpy(statusInfo, "Unknown expression");
    break;
  }
  return ans;
}

/* Evaluate the operations, with using stack */
long double evalPostfix(char **postf, int plen) {
  STACK *s = (STACK *)malloc(sizeof(STACK));
  initStack(s);
  int i;
  long double op1, op2;

  for (i = 0; i < plen; i++) {
    if (isdigit(postf[i][0]) || (strlen(postf[i]) > 2 && postf[i][0] == '-')) {
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
void preProcess(char *infix, char *clean) {
  int i = 0, c = 0;

  // Put zero to beginning, if starts with "-"
  if (infix[0] == '-') {
    clean[c++] = '0';
  }

  while (infix[i] != '\0') {
    // Put zero before negative leftsided numbers
    if (i > 0 && infix[i] == '-' && (infix[i - 1] == '(')) {
      clean[c++] = '0';
    }

    // Acepts all chars but space
    if (infix[i] != ' ') {
      clean[c++] = infix[i];
    }
    i++;
  }
  clean[c] = '\0';
}

/* Returns 1 if the expression has balanced pharentheses*/
int validCheck(char *in, int ilen) {
  STACK *s = (STACK *)malloc(sizeof(STACK));
  long double ch;
  int i;
  initStack(s);

  for (i = 0; i < ilen; i++) {
    if (in[i] == '(') {
      push(s, in[i]);
    } else {
      if (in[i] == ')') {
        if (!isEmpty(s)) {
          pop(s, &ch);
        } else {
          free(s);

          evalStatus = 1;
          strcpy(statusInfo, "Syntax Error: Unbalanced Pharentheses\n");
          return 0;
        }
      }
    }
  }
  if (!isEmpty(s)) {
    free(s);

    evalStatus = 1;
    strcpy(statusInfo, "Syntax Error: Unbalanced Pharentheses\n");
    return 0;
  }

  free(s);
  return 1;
}

/* Evaluates and returns the answer. status will be set to -1 if any error
 * occurs */
long double evalExpression(char *exp) {
  char **postf, **infix;
  int ilen, plen;
  long double ans;

  // Preprocess
  char *clean = (char *)malloc(sizeof(char) * MAX_CHAR_IN);
  preProcess(exp, clean);
  strcpy(exp, clean);
  // Validation check
  if (!validCheck(exp, strlen(exp))) {
    return 0;
  }

  infix = stringToArray(exp, strlen(exp), &ilen);
  /* DEBUG */
  if (_DEBUG) {
    int k;
    printf("Infix:: ");
    for (k = 0; k < ilen; k++) {
      printf("[%s] ", infix[k]);
    }
    printf("\n");
  }

  postf = infixToPostfix(infix, ilen, &plen);
  /* DEBUG */
  if (_DEBUG) {
    int k;
    printf("Postfix:: ");
    for (k = 0; k < plen; k++) {
      printf("[%s] ", postf[k]);
    }
    printf("\n");
  }
  ans = evalPostfix(postf, plen);

  free(clean);
  deleteExpression(infix, ilen);
  deleteExpression(postf, plen);
  return ans;
}

#ifdef STANDALONE
int main() {
  char in[MAX_CHAR_IN];
  int status;
  long double ans;
  printf(ANSI_COLOR_RESET);
  printf("[Arithmetic Expression Evaluation]\n<Type 'q' to quit.>\n");

  do {
    evalStatus = 0;
    printf(ANSI_COLOR_RESET);
    printf("E:");
    if(!fgets(in, MAX_CHAR_IN, stdin)) {
      exit(5);
    }
    in[strlen(in) - 1] = '\0';

    ans = evalExpression(in);

    if (!evalStatus) {
      printf("Ans:>");
      printf(ANSI_COLOR_YELLOW);

      if (floor((double)ans) == (ans)) {
        printf("%ld\n", (long signed int)ans);
      } else {

        printf("%Lf\n", ans);
      }
      formerAns = ans;

    } else {
      printf(ANSI_COLOR_RED);
      printf("%s\n", statusInfo);
      printf(ANSI_COLOR_RESET);
    }

  } while (strcmp(in, "q"));

  return 0;
}
#endif
