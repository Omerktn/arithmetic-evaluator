#ifndef ARTHM_H
#define ARTHM_H

extern int evalStatus;
extern char statusInfo[256];
extern long double formerAns;

long double evalExpression(char *exp);

#endif // ARTHM_H
