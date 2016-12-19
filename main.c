#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "my_conio.h"
#include <malloc.h>

struct data{
    int num_v;                 //количество переменных целевой функции
    int num_l;                 //количество ограничений в системе
    double **system;           //система ограничений
    double *function;          //целевая функция
    double *fm;                //свободный члены при ограничениях
};

struct count_data{
    double func;               //значение целевой функции
    double **bv;               //значения базисных переменных задачи
    double **sv;               //коэффициенты при переменных задачи
    double *istr;              //индексная строка
    double *th;                //последний столбец симплексной таблицы
    double alm;                //разрешающий элемент, находящийся на пересечении ведущего столбца и ведущей строки
    int i_lrow;                //индекс ведущей строки текущего плана.
    int i_lcol;                //индекс ведущего столбца текущего плана
};

void error(int err_number)
{
    printf("Error.\n");
    switch(err_number){
    case 1:
        printf("You have entered incorrect value.\n");
        break;
    case 2:
        printf("You can't enter less than two limitations.\n");
        break;
    case 3:
        printf("You can't enter more than ten limitations.\n");
        break;
    case 4:
        printf("You can't enter less than two variable.\n");
        break;
    case 5:
        printf("You can't enter more than ten variable.\n");
        break;
    }
}

void printData(struct data printingData){
    int i,j;
    printf("\n\n\n");

    printf("F(x)=");
    for(i=0;i<printingData.num_v;i++)
    {
        if(printingData.function[i]<0||i==0)
            printf("%.2lfx%d",printingData.function[i],i+1);
        else
            printf("+%.2lfx%d",printingData.function[i],i+1);
    }
    printf(" -> max;\n");

    for(i=0;i<printingData.num_l;i++)
    {
        for(j=0;j<printingData.num_v;j++)
        {
            if(printingData.system[i][j]<0||j==0)
                printf("%.2lfx%d",printingData.system[i][j],j+1);
            else
                printf("+%.2lfx%d",printingData.system[i][j],j+1);
        }
        printf(" <= ");
        printf("%.2lf\n",printingData.fm[i]);
    }

}

struct data scanData(){
    int i=0, j=0;
    int valid=0;
    struct data userData;
    do{
        printf("Enter number of the limitations: ");    //Ввод количества ограничений
        scanf("%d",&userData.num_l);
        if(getchar()!='\n')
        {
            fflush(stdin);
            error(1);
            valid=0;
        }else if(userData.num_l<2){
            error(2);
            valid=0;
        }else if(userData.num_l>10){
            error(3);
            valid=0;
        }else{
            valid=1;
        }
    }while(!valid);

    do{
        printf("Enter number of the variable: ");        //Ввод количетсва переменных
        scanf("%d",&userData.num_v);
        if(getchar()!='\n')
        {
            fflush(stdin);
            error(1);
            valid=0;
        }else if(userData.num_v<2){
            error(4);
            valid=0;
        }else if(userData.num_v>10){
            error(5);
            valid=0;
        }else{
            valid=1;
        }
    }while(!valid);


    userData.function=(double*)calloc(userData.num_v,sizeof(double));
    userData.fm=(double*)calloc(userData.num_l,sizeof(double));
    userData.system=(double**)calloc(userData.num_l,sizeof(double*));
    for(i=0;i<userData.num_l;i++)
        userData.system[i]=(double*)calloc(userData.num_v,sizeof(double));


    printf("Enter coefficients of the function;\n");
    for(i=0;i<userData.num_v;i++){
        do{
            printf("Enter coefficient of x%d: ", i+1);
            scanf("%lf",&userData.function[i]);
            if(getchar()!='\n'||userData.function[i]<=-100000||userData.function[i]>=100000)
            {
                fflush(stdin);
                error(1);
                valid=0;
            }else{
                valid=1;
            }
        }while(!valid);

    }

    printf("Enter system of limitations;\n");
    printf("Please, make every system lool like 'smt <= C';\n");

    for(i=0;i<userData.num_l;i++)
    {
        printf("%d limitation;\n",i+1);
        for(j=0;j<userData.num_v;j++)
        {
            do{
                printf("x%d: ", j+1);
                scanf("%lf",&userData.system[i][j]);
                if(getchar()!='\n'||userData.system[i][j]<=-100000||userData.system[i][j]>=100000)
                {
                    fflush(stdin);
                    error(1);
                    valid=0;
                }else{
                    valid=1;
                }
            }while(!valid);
        }

        do{
            printf("Free member: ");
            scanf("%lf",&userData.fm[i]);
            if(getchar()!='\n'||userData.fm[i]<-100000||userData.fm[i]>100000)
            {
                fflush(stdin);
                error(1);
                valid=0;
            }else{
                valid=1;
            }
        }while(!valid);
    }
    return userData;
}

void freeData(struct data breakData){
    int i;
    free(breakData.function);
    free(breakData.fm);
    for(i=0;i<breakData.num_l;i++)
        free(breakData.system[i]);
    free(breakData.system);
}

int unlimited(struct count_data checkingCountData, struct data checkingData){
    int i;
    for (i = 0; i <checkingData.num_l; i++)
        if (checkingCountData.th[i] < 0)
            return 1;
    return 0;
}

int optimal(struct count_data checkingCountData, struct data checkingData){
    int i;
    for (i = 0; i < checkingData.num_v * 2; i++)
        if (checkingCountData.istr[i] < 0)
            return 0;
    return 1;
}

void generatePlane(struct count_data countData, struct data beginData)
{
    int i, j, it_num = 0;
    double A, B;
    double tmp;
    double *tmp_bv, **tmp_sv;

    while (!(optimal(countData,beginData) && unlimited(countData,beginData)))
    {
        A = countData.bv[countData.i_lrow][1];                                              //вычисление А и Б прямоугольника
        B = countData.istr[countData.i_lcol];

        countData.func -= A * B / countData.alm;                                            //вычисление функции

        tmp_bv=(double*)calloc(beginData.num_l,sizeof(double));                             //для формирования нового плана введём временный массив

        countData.bv [countData.i_lrow][0] = countData.i_lcol;
        A = countData.bv[countData.i_lrow][1];

        for (i = 0; i < beginData.num_l; i++)
        {
            B = countData.sv[i][countData.i_lcol];
            tmp_bv[i] = countData.bv[countData.i_lrow][1];

            if (i != countData.i_lrow)                                                       //вычисление плана по методу прямоугольника
                tmp_bv[i] = countData.bv[i][1] - A * B / countData.alm;
            else
                tmp_bv[i] /= countData.alm;
        }

        for (i = 0; i < beginData.num_l; i++)
            countData.bv[i][1] = tmp_bv[i];

        B = countData.istr[countData.i_lcol];

        for (i = 0; i < beginData.num_v * 2; i++)
        {
            A = countData.sv[countData.i_lrow][i];
            countData.istr[i] -= A * B / countData.alm;                           //вычисление новой индексной строки
        }

        tmp_sv = (double**)calloc(beginData.num_l,sizeof(double));
        for (i = 0; i < beginData.num_l; i++)
            tmp_sv[i] = (double*)calloc(beginData.num_v*2,sizeof(double));

        for (i = 0; i < beginData.num_l; i++)                                                  //пересчёт симплекс таблицы
            for (j = 0; j < beginData.num_v * 2; j++)
            {
                tmp_sv[i][j] = countData.sv[i][j];

                A = countData.sv[countData.i_lrow][j];
                B = countData.sv[i][countData.i_lcol];

                if (i == countData.i_lrow)
                    tmp_sv[i][j] /= countData.alm;
                else
                    tmp_sv[i][j] = countData.sv[i][j] - A * B / countData.alm;
            }

        countData.sv = tmp_sv;
        countData.i_lcol = 0;
        tmp=countData.istr[0];
        for (i = 0; i < beginData.num_v * 2-1; i++)                     //максимальное по модулю отриц. число в индексной строке
            if (countData.istr[i] < 0)
                if (fabs(countData.istr[i + 1]) > fabs(tmp))
                {
                    tmp=countData.istr[i + 1];
                    countData.i_lcol = i + 1;
                }

        for (i = 0; i < beginData.num_l; i++)
            countData.th[i] = countData.bv[i][1] / countData.sv[i][countData.i_lcol];        //вычисление массива th

        countData.i_lrow = 0;

        for (i = 0; i < beginData.num_l -1; i++)                                             //вычисление разрешающей строки
            if (countData.th[i] > countData.th[i + 1])
                countData.i_lrow = i + 1;

        countData.alm = countData.sv[countData.i_lrow][countData.i_lcol];                    //вычисление разешающего элемента
        it_num++;                                                                            //итерация
    }

    if (unlimited(countData, beginData))                                             //проверка плана
        printf("\nNo solution, function unlimited\n");
    else {
        printf("\nF(x)=%lf\n",countData.func);                                               //вывод результата
        printf("\nBasis(If there are other variables, skip them):");
        for (i = 0; i < beginData.num_l; i++) {
            printf("\nx%.0lf=%.2lf",countData.bv[i][0] + 1,countData.bv[i][1]);
        }
    }
    printf("\n\n");


    free(tmp_bv);                                                                            //очищение памяти после работы с ней
    free(countData.istr);
    free(countData.th);
    for(i=0;i<beginData.num_l;i++)
    {
        free(countData.sv[i]);
        free(countData.bv[i]);
    }
    free(countData.sv);
    free(countData.bv);
}

void initCountData(struct data beginData){
    struct count_data mainData;
    int i, j;
    double tmp;

    mainData.func=0;
    mainData.sv=(double**)calloc(beginData.num_l,sizeof(double));
    for(i=0;i<beginData.num_l;i++)
        mainData.sv[i]=(double*)calloc(beginData.num_v*2,sizeof(double));

    for (i = 0; i < beginData.num_l; i++) {                           //Матрица коэффициентов
        for (j = 0; j < beginData.num_v; j++)                         //заполнение коэффициентов и добавление единичной марицы
            mainData.sv[i][j] = beginData.system[i][j];
        for (; j < beginData.num_v * 2; j++)
            if (i + beginData.num_v == j)
                mainData.sv[i][j] = 1;
            else
                mainData.sv[i][j] = 0;
    }

    mainData.istr=(double*)calloc(beginData.num_v*2,sizeof(double));  //выделение памяти под индексную строку

    mainData.bv=(double**)calloc(beginData.num_l,sizeof(double));     //выделение памяти и инициализация базиса
    for (i = 0; i < beginData.num_l; i++) {
        mainData.bv[i]=(double*)calloc(2,sizeof(double));
        mainData.bv[i][0] = i + beginData.num_v;
        mainData.bv[i][1] = beginData.fm[i];
    }

    for (i = 0; i < beginData.num_v * 2; i++)
        if (i < beginData.num_v)
            mainData.istr[i] = beginData.function[i] * (-1);          //заполнение индексной строки коэфф. функции, домнож на (-1)
        else
            mainData.istr[i] = 0;

    mainData.i_lcol = 0;                                              //столбец разрешающего элемента
    tmp=mainData.istr[0];

    //максимальное по модулю отриц. число в индексной строке
    for (i = 0; i < beginData.num_v * 2 - 1; i++) {
        if (mainData.istr[i] < 0)
            if (fabs(mainData.istr[i + 1]) > fabs(tmp))
            {
                tmp=mainData.istr[i + 1];
                mainData.i_lcol = i + 1;
            }
    }

    mainData.th = (double*)calloc(beginData.num_l, sizeof(double));       //выделение памяти под массив

    for (i = 0; i < beginData.num_l; i++)
            mainData.th[i] = mainData.bv[i][1] / mainData.sv[i][mainData.i_lcol];

    mainData.i_lrow = 0;                                                //строка разрешающего элемента

    tmp = 0;
    for (i = 0; i < beginData.num_l - 1; i++)
        if (tmp > mainData.th[i + 1])
            tmp = i + 1;
    mainData.i_lrow = tmp;

    mainData.alm = mainData.sv[mainData.i_lrow][mainData.i_lcol];       //вычисление разрешающего элемента

    generatePlane(mainData,beginData);                                  //вычисление оптимального плана
}

int main(void)
{
    struct data Data=scanData();
    printData(Data);
    initCountData(Data);
    freeData(Data);
    return 0;
}
