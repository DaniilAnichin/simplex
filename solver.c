#include "solver.h"

void canonize(struct functionData system){
    int i = 0, j = 0, nextVar = system.nVariables;


    for(i = 0; i < system.nLimitations; ++i)
    {
        switch(system.limitationSigns[i]){
        case LS:
            system.limitationSigns[i] = LSEQ;
            system.freeMembers[i] -= 10e-10;
        case LSEQ:
            system.system[i][nextVar] = 1;
            system.basisIds[i] = nextVar;
            nextVar += 1;
            break;
        case EQ:
            system.system[i][nextVar] = 1;
            system.function[nextVar] = (system.direction == MINIM) ? M : -M;
            system.basisIds[i] = nextVar;
            nextVar += 1;
            break;
        case GR:
            system.limitationSigns[i] = GREQ;
            system.freeMembers[i] += 10e-10;
        case GREQ:
            system.system[i][nextVar] = -1;
            nextVar += 1;
            system.system[i][nextVar] = 1;
            system.basisIds[i] = nextVar;
            system.function[nextVar] = (system.direction == MINIM) ? M : -M;
            break;
        }
    }
    system.nFullVars = nextVar;

    for(i = 0; i < system.nFullVars; ++i)
    {
        system.system[system.nLimitations][i] = -system.function[i];
        for(j = 0; j < system.nLimitations; ++j)
            system.system[system.nLimitations][i] +=
                    system.function[system.basisIds[j]] * system.system[j][i];
    }

    for(j = 0; j < system.nLimitations; ++j)
        system.freeMembers[system.nLimitations] +=
                system.function[system.basisIds[j]] * system.freeMembers[j];
}

short optimal(struct functionData system){
    int i = 0;
    int sign = system.direction == MINIM ? -1 : 1;
    for (i = 0; i < system.nFullVars; ++i)
        if(system.system[system.nLimitations][i] * sign > 0)
            return 0;
    return 1;
}

short findColumn(struct functionData system){
    int i = 0;
    int sign = system.direction == MINIM ? -1 : 1;
    short column = 0;
    double value = 0;
    double newValue = 0;

    for (i = 0; i < system.nFullVars; ++i)
        newValue = system.system[system.nLimitations][i] * sign;
        if(newValue > value)
        {
            value = newValue;
            column = i;
        }

    return column;
}

short findRow(struct functionData system, short column){
    int i = 0;
    int sign = system.direction == MINIM ? -1 : 1;
    int row = 0;
    double value = 0;
    double newValue = 0;

    for (i = 0; i < system.nFullVars; ++i)
        newValue = system.system[system.nLimitations][i] * sign;
        if(newValue > value)
        {
            value = newValue;
            row = i;
        }

    return row;
}

short iterate(struct functionData system, short column, short row){
//void generatePlane(struct count_data countData, struct data beginData)
//    int i, j, it_num = 0;
//    double A, B;
//    double tmp;
//    double *tmp_bv, *tmp_istr, **tmp_sv;

//    while (!validPlane(countData,beginData) && undefiendFunction(countData,beginData))
//    {
//        A = countData.bv[countData.i_lrow][1];                                              //вычисление А и Б прямоугольника
//        B = countData.istr[countData.i_lcol];

//        countData.func -= A * B / countData.alm;                                            //вычисление функции

//        tmp_bv=(double*)calloc(beginData.num_l,sizeof(double));                             //для формирования нового плана введём временный массив

//        countData.bv [countData.i_lrow][0] = countData.i_lcol;
//        A = countData.bv[countData.i_lrow][1];

//        for (i = 0; i < beginData.num_l; i++)
//        {
//            B = countData.sv[i][countData.i_lcol];
//            tmp_bv[i] = countData.bv[countData.i_lrow][1];

//            if (i != countData.i_lrow)                                                       //вычисление плана по методу прямоугольника
//                tmp_bv[i] = countData.bv[i][1] - A * B / countData.alm;
//            else
//                tmp_bv[i] /= countData.alm;
//        }

//        for (i = 0; i < beginData.num_l; i++)
//            countData.bv[i][1] = tmp_bv[i];

//        tmp_istr = countData.istr;                                                      //для новой индексной строки - нужно ввести ввременную строку
//        B = countData.istr[countData.i_lcol];

//        for (i = 0; i < beginData.num_v * 2; i++)
//        {
//            A = countData.sv[countData.i_lrow][i];
//            tmp_istr[i] = countData.istr[i] - A * B / countData.alm;                           //вычисление новой индексной строки
//        }

//        countData.istr = tmp_istr;                                                             //новая индексная строка

//        tmp_sv = (double**)calloc(beginData.num_l,sizeof(double));
//        for (i = 0; i < beginData.num_l; i++)
//            tmp_sv[i] = (double*)calloc(beginData.num_v*2,sizeof(double));

//        for (i = 0; i < beginData.num_l; i++)                                                  //пересчёт симплекс таблицы
//            for (j = 0; j < beginData.num_v * 2; j++)
//            {
//                tmp_sv[i][j] = countData.sv[i][j];

//                A = countData.sv[countData.i_lrow][j];
//                B = countData.sv[i][countData.i_lcol];

//                if (i == countData.i_lrow)
//                    tmp_sv[i][j] /= countData.alm;
//                else
//                    tmp_sv[i][j] = countData.sv[i][j] - A * B / countData.alm;
//            }

//        countData.sv = tmp_sv;
//        countData.i_lcol = 0;
//        tmp=countData.istr[0];
//        for (i = 0; i < beginData.num_v * 2-1; i++) {                   //максимальное по модулю отриц. число в индексной строке
//            if (countData.istr[i] < 0)
//            {
//                if (fabs(countData.istr[i + 1]) > fabs(tmp))
//                {
//                    tmp=countData.istr[i + 1];
//                    countData.i_lcol = i + 1;
//                }


//            }
//        }


//        for (i = 0; i < beginData.num_l; i++)
//            countData.th[i] = countData.bv[i][1] / countData.sv[i][countData.i_lcol];        //вычисление массива th

//        countData.i_lrow = 0;

//        for (i = 0; i < beginData.num_l -1; i++)                                             //вычисление разрешающей строки
//            if (countData.th[i] > countData.th[i + 1])
//                countData.i_lrow = i + 1;

//        countData.alm = countData.sv[countData.i_lrow][countData.i_lcol];                    //вычисление разешающего элемента
//        it_num++;                                                                            //итерация
//    }

//    if (!undefiendFunction(countData,beginData))                                             //проверка плана
//        printf("\nResolve is no, because function isn't limitation.\n");
//    else {
//        printf("\nF(x)=%lf\n",countData.func);                                               //вывод результата
//        printf("\nBasis:");
//        for (i = 0; i < beginData.num_l; i++) {
//            printf("\nx%.0lf=%.2lf",countData.bv[i][0] + 1,countData.bv[i][1]);
//        }
//    }
//    printf("\n\n");


//    free(tmp_bv);                                                                            //очищение памяти после работы с ней
//    free(tmp_istr);
//    free(countData.istr);
//    free(countData.th);
//    for(i=0;i<beginData.num_l;i++)
//    {
//        free(countData.sv[i]);
//        free(countData.bv[i]);
//    }
//    free(countData.sv);
//    free(countData.bv);
    return 0;
}



struct functionData inputFunction(){
    int i = 0, j = 0, valid = 0;
    struct functionData userData;
    do{
        printf("Enter limitations number: \n");
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
        printf("Enter variables number: \n");
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
    return 0;
}

