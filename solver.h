#ifndef SOLVER_H
#define SOLVER_H

#define ERROR 1
#define LLIMS 2
#define MLIMS 3
#define LVARS 4
#define MVARS 5
#define UNLIM 6

#define MAXIM 0
#define MINIM 1
#define M 10e10

#define LSEQ 0
#define LS 1
#define EQ 2
#define GR 3
#define GREQ 4


#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_conio.h"


struct functionData{
    int nVariables;
    int nMaxVariables;
    int nFullVars;
    int nLimitations;
    int direction;
    int* basisIds;
    int* limitationSigns;
    double* function;
    double* freeMembers;
    double** system;
};

void canonize(struct functionData *system);
short optimal(struct functionData system);

short findColumn(struct functionData system);
short findRow(struct functionData system, short column);
short iterate(struct functionData *system, short column, short row);

void printError(int number);
struct functionData inputFunction();
void freeFunction(struct functionData data);

short solve(struct functionData system);

#endif // SOLVER_H
