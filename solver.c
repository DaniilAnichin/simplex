#include "solver.h"

void canonize(struct functionData *system){
    int i = 0, j = 0;
    int nextVar = system->nVariables;

    for(i = 0; i < system->nLimitations; ++i)
    {
        switch(system->limitationSigns[i]){
        case LS:
            system->limitationSigns[i] = LSEQ;
            system->freeMembers[i] -= 10e-10;
        case LSEQ:
            system->system[i][nextVar] = 1;
            system->basisIds[i] = nextVar;
            nextVar += 1;
            break;
        case EQ:
            system->system[i][nextVar] = 1;
            system->function[nextVar] = (system->direction == MINIM) ? M : -M;
            system->basisIds[i] = nextVar;
            nextVar += 1;
            break;
        case GR:
            system->limitationSigns[i] = GREQ;
            system->freeMembers[i] += 10e-10;
        case GREQ:
            system->system[i][nextVar] = -1;
            nextVar += 1;
            system->system[i][nextVar] = 1;
            system->basisIds[i] = nextVar;
            system->function[nextVar] = (system->direction == MINIM) ? M : -M;
            nextVar += 1;
            break;
        }
    }
    system->nFullVars = nextVar;

    for(i = 0; i < system->nFullVars; ++i)
    {
        system->system[system->nLimitations][i] = -system->function[i];
        for(j = 0; j < system->nLimitations; ++j)
            system->system[system->nLimitations][i] +=
                    system->function[system->basisIds[j]] * system->system[j][i];
    }

    for(j = 0; j < system->nLimitations; ++j)
        system->freeMembers[system->nLimitations] +=
                system->function[system->basisIds[j]] * system->freeMembers[j];
}

short optimal(struct functionData system){
    int i = 0;
    int sign = system.direction == MINIM ? -1 : 1;
    for (i = 0; i < system.nFullVars; ++i)
        if(system.system[system.nLimitations][i] * sign < 0)
            return 0;
    return 1;
}

short findColumn(struct functionData system){
    int i = 0;
    int sign = system.direction == MINIM ? -1 : 1;
    short column = -1;
    double value = 0;
    double newValue = 0;

    for (i = 0; i < system.nFullVars; ++i)
    {
        newValue = system.system[system.nLimitations][i] * sign;
        if(newValue < value)
        {
            value = newValue;
            column = i;
        }
    }

    return column;
}

short findRow(struct functionData system, short column){
    int i = 0;
    int row = -1;
    double value = 0;
    double newValue = 0;

    for (i = 0; i < system.nLimitations; ++i)
        if(system.system[i][column] > 0)
        {
            newValue = system.system[i][column] / system.freeMembers[i];
            if(newValue > value)
            {
                value = newValue;
                row = i;
            }
        }

    return row;
}

short iterate(struct functionData *system, short column, short row){
    int i, j;
    double** sys = system->system;
    double center = sys[row][column];
    double tmp = 0;

    // New basis line
    system->freeMembers[row] /= center;
    for(j = 0; j < system->nFullVars; ++j)
    {
        sys[row][j] /= center;
    }

    // New system
    for(i = 0; i < system->nLimitations + 1; ++i)
        if(i != row)
        {
            tmp = sys[i][column];
            system->freeMembers[i] -= tmp * system->freeMembers[row];
            for(j = 0; j < system->nFullVars; ++j)
                sys[i][j] -= tmp * sys[row][j];
        }

    system->basisIds[row] = column;
    return 0;
}



struct functionData inputFunction(){
    int i = 0, j = 0, valid = 0;
    struct functionData userData;
    do{
        printf("Enter limitations number: ");
        scanf("%d", &userData.nLimitations);
        if(getchar() != '\n')
        {
            fflush(stdin);
            printError(ERROR);
        }
        else if(userData.nLimitations < 2)
        {
            printError(LLIMS);
        }
        else if(userData.nLimitations > 10)
        {
            printError(MLIMS);
        }
        else
        {
            valid=1;
        }
    }while(!valid);

    valid = 0;
    do{
        printf("Enter variables number: ");
        scanf("%d", &userData.nVariables);
        if(getchar()!='\n')
        {
            fflush(stdin);
            printError(ERROR);
        }
        else if(userData.nVariables < 2)
        {
            printError(LVARS);
        }
        else if(userData.nVariables > 10)
        {
            printError(MVARS);
        }
        else{
            valid=1;
        }
    }while(!valid);

    userData.nMaxVariables = userData.nLimitations * 2 + userData.nVariables;

    userData.function = (double*)calloc(userData.nMaxVariables, sizeof(double));
    userData.limitationSigns = (int*)calloc(userData.nLimitations, sizeof(int));
    userData.freeMembers = (double*)calloc(userData.nLimitations + 1, sizeof(double));
    userData.basisIds = (int*)calloc(userData.nLimitations, sizeof(int));
    userData.system = (double**)calloc(userData.nLimitations + 1, sizeof(double*));
    for(i = 0; i < userData.nLimitations + 1; ++i)
        userData.system[i] = (double*)calloc(userData.nMaxVariables, sizeof(double));

    valid = 0;
    printf("Enter function coefficients: \n");
    for(i = 0; i < userData.nVariables; ++i){
        do{
            printf("Enter coefficient of x%d: ", i + 1);
            scanf("%lf", &userData.function[i]);
            if(getchar() != '\n' || fabs(userData.function[i]) >= 100000)
            {
                fflush(stdin);
                printError(ERROR);
            }
            else
            {
                valid = 1;
            }
        }while(!valid);
    }
    valid = 0;
    do{
        printf("Enter function direction (min -> %d, max -> %d): ", MINIM, MAXIM);
        scanf("%d", &userData.direction);
        if(getchar()!='\n' || (userData.direction != MINIM &&
                               userData.direction != MAXIM))
        {
            fflush(stdin);
            printError(ERROR);
        }
        else
        {
            valid = 1;
        }
    }while(!valid);


    printf("Enter limitations system: \n");

    for(i = 0; i < userData.nLimitations; ++i)
    {
        printf("Enter %d limitation: \n", i + 1);
        for(j = 0; j < userData.nVariables; ++j)
        {
            valid = 0;
            do{
                printf("x%d: ", j + 1);
                scanf("%lf", &userData.system[i][j]);
                if(getchar() != '\n' || fabs(userData.system[i][j]) >= 100000)
                {
                    fflush(stdin);
                    printError(ERROR);
                }
                else
                {
                    valid=1;
                }
            }while(!valid);
        }

        valid = 0;
        do{
            printf("Enter the %d limitation sign(<= - %d, < - %d, = - %d, > - %d, >= - %d): ",
                   i + 1, LSEQ, LS, EQ, GR, GREQ);
            scanf("%d", &userData.limitationSigns[i]);
            if(getchar()!='\n' || userData.limitationSigns[i] < LSEQ
                    || userData.limitationSigns[i] > GREQ)
            {
                fflush(stdin);
                printError(ERROR);
            }
            else
            {
                valid=1;
            }
        }while(!valid);

        valid = 0;
        do{
            printf("Enter the %d limitation free member: ", i + 1);
            scanf("%lf", &userData.freeMembers[i]);
            if(getchar() != '\n' || fabs(userData.freeMembers[i]) >= 100000)
            {
                fflush(stdin);
                printError(ERROR);
            }
            else
            {
                valid = 1;
            }
        }while(!valid);
    }
    return userData;
}

void printError(int err_number)
{
    printf("Error: ");
    switch(err_number){
    case ERROR:
        printf("Incorrect value entered\n");
        break;
    case LLIMS:
        printf("Not enough limitations\n");
        break;
    case MLIMS:
        printf("Too many limitations\n");
        break;
    case LVARS:
        printf("Not enough variables\n");
        break;
    case MVARS:
        printf("Too many variables\n");
        break;
    case UNLIM:
        printf("Function unlimited\n");
        break;
    }
}

void freeFunction(struct functionData data){
    int i;
    for(i = 0; i < data.nLimitations + 1; ++i)
        free(data.system[i]);
    free(data.system);
    free(data.function);
    free(data.basisIds);
    free(data.limitationSigns);
    free(data.freeMembers);
}

short solve(struct functionData system){
    int column = 0, row = 0, result = 0;
    canonize(&system);
    while(!optimal(system)){
        column = findColumn(system);
        if(column == -1)
        {
            printError(ERROR);
            return -1;
        }

        row = findRow(system, column);

        if(row == -1)
        {
            printError(UNLIM);
            return -1;
        }
        result = iterate(&system, column, row);
    }
    printf("Optimal result is %lf; \n", system.freeMembers[system.nLimitations]);
    freeFunction(system);

    return 0;
}

