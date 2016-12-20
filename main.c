#include "my_conio.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
    float l=-1, c;
    int i,j, ii, jj, res, iw=0, a,b, k=0, p=0;
    char str1[4]="max";
    char str2[4]="min";
    char sign1[4]="=";
    char sign2[4]="<=";
    char sign3[4]=">=";
    char sign[4];
    char str[4];
    double Array[200][200];

  //1
    do{
    system("clear");
    printf("Enter number of variables (2..10)\n");
    res=scanf("%d",&a);
    fflush(stdin);
    if(res!=1 || a<2 || a>10){
        //kill user
        printf("Error! Enter variables  more than 2 less than 10\n");
        res=0;
        getch();
    }} while(res!=1);

  //2
    do{
    system("clear");
    printf("Enter number of lines-restrictions without x>=0 (2..10)\n");
    res=scanf("%d", &b);
    fflush(stdin);
    if(res!=1 || b<2 || b>10){
        printf("Error! Enter lines more than 2 less then 10\n");
        //kill user
        res=0;
       getch();
    }} while(res!=1);



    for(i=1;i<=b; i++){
    for(j=0;j<=a+1 ;j++){Array[i][j]=-39784632;}}

   do{
   system("clear");
   printf("F(x)-> max or min?\n");
   scanf("%s", str);
   if(strcmp (str, str1)==0){p=1;}
   if(strcmp (str, str2)==0){p=2;}
   }while(p!=1 && p!=2);


    for(j=1; j<=a; j++){
        do{
        system("clear");
        printf ("Enter the target rate at function x%d\n", j);
        res=scanf("%f", &l);
        fflush(stdin);
        if(res!=1){printf("Invalid input. Try again.\n");}
        }while(res!=1);
        Array[b+1][j]=-l;
    }


    for (ii=1; ii<=b+1; ii++){
       for (jj=1; jj<=a; jj++){

           system("clear");
       iw=0;

       for (i=1; i<=b; i++){
          for (j=1; j<a; j++){

//функція
              if(iw==0){
              for(iw=1; iw<a; iw++){
                  printf("%.4fX%d+", -Array[b+1][iw],iw);
                 }
              if(p==1){
                 printf("%.4fX%d->max\n\n", -Array[b+1][iw],iw);}
              if(p==2){
                 printf("%.4fX%d->min\n\n", -Array[b+1][iw], iw);}
              if(p!=1 && p!=2){printf("Error\n");}
              }

  //двомірний масив
              if(Array[i][j]==-39784632){
              printf("k%d%dx+", i, j);
               }
              else{
                  printf("%.4fx+", Array[i][j]);
                  }
             }

           if(Array[i][j]==-39784632){
         printf("k%d%dx",i,j);
      }
         else{
            printf("%.4fx",Array[i][j]);
         }

           if(Array[i][j+1]==-39784632){
         printf("=C%d\n", i);
               }
         else{
               if(Array[i][j+1]==0) {printf(" = %.4f\n",Array[i][0]);}
               if(Array[i][j+1]==1) {printf(" <= %.4f\n",Array[i][0]);}
               if(Array[i][j+1]==-1) {printf(" >= %.4f\n",Array[i][0]);}
           }
         }

//введення даних

       i=ii;
       j=jj;
       if(i!=b+1){
       do{
       printf("Enter k%d%d= ",i,j);
       res=scanf("%f", &l);
       fflush(stdin);
       if(res!=1){printf("Invalid input. Try again.\n");}
       }while(res!=1);
       Array[i][j]=l;}
       }

       if(i!=b+1){
           do{
           printf("Enter sign at %d line (=,<=,>=)\n",i);
           scanf("%s", sign);
           if(strcmp (sign, sign1)==0){ Array[i][j+1]= 0;}
           if(strcmp (sign, sign2)==0){ Array[i][j+1]= 1;}
           if(strcmp (sign, sign3)==0){ Array[i][j+1]= -1;}
           }while(Array[i][j+1]!=0 && Array[i][j+1]!=1 && Array[i][j+1]!=-1);


do{
           printf("Enter C%d=",i);
           res=scanf("%f", &l);
           fflush(stdin);
           if(res!=1){printf("Invalid input. Try again.\n");}}while(res!=1);
           Array[i][0]=l;}
       }

    for(i=1; i<=b; i++){
    if(Array[i][a+1]==-1){
        for(j=0; j<=a; j++){Array[i][j]=-Array[i][j];}
    }}

    printf( "Our simplex-table:\n" );
    for (i=1; i<=b+1; i++){
       for (j=0; j<=a+1; j++){
                   printf( "[%d][%d]=%.4f ", i, j, Array[i][j] );
               }
               printf( "\n" );
             }
             getchar();

do{
    //визначення опорного плану
    l=0;
    for(j=1; j<=a; j++){
        if((p==1 && Array[b+1][j]<0) || (p==2 && Array[b+1][j]>0)){
            l = Array[b+1][j];
            k = j;
        }
    }
//    printf("l=%.4f\nk=%d\n", l, k);

    for(j = 1; j <= a; ++j){
        if((p == 1 && Array[b+1][j] < 0) || (p == 2 && Array[b+1][j] > 0)){
            if(fabs(Array[b+1][j]) > fabs(l)){
                l = Array[b+1][j];
                k = j;
            }
        }
    }
    if(l == 0){
        printf("Solution F(x1..x%d))=%.4f\n", a, Array[b+1][0]);
        return 0;
    }
//    printf("k=%d\n", k);


    for(i=1; i<=b; i++){
    if (Array[i][k]>=0){
    c=Array[i][0]/Array[i][k];
    ii=i;}}

    for(i=1; i<=b; i++){
     if(Array[i][k]>=0){
    if((Array[i][0]/Array[i][k])<(c)){c=Array[i][0]/Array[i][k]; ii=i;}
     }}
    // The lesser divisor between elements
    printf("%.4f\n", c);
    if (ii==b+1){printf("All members in support program less than 0\n"); return 0;}
    printf("%.4f - x[%d][%d] allowing member\n",Array[ii][k], ii, k);



    //генерація нової симплекс-таблиці
    for(i=1; i<=b+1; i++){
     for(j=0; j<=a; j++){
      if(i!=ii && j!=k){
          Array[i][j]=(Array[i][j]*Array[ii][k]-Array[ii][j]*Array[i][k])/Array[ii][k];
      }
     }
    }
//    printf("Array - %.4f\n", Array[ii][k]);

     for(j=0; j<=a; j++){
         if(i!=ii && j!=k){
        Array[ii][j]=Array[ii][j]/Array[ii][k];}
     }
     for(i=1; i<=b+1; i++){
         if(i!=ii && j!=k){
        Array[i][k]=-Array[i][k]/Array[ii][k];}
     }
     Array[ii][k]=1/Array[ii][k];



     printf( "Our new simplex-table:\n" );
     for (i=1; i<=b+1; i++){
        for (j=0; j<=a+1; j++){
                    printf( "[%d][%d]=%.4f ", i, j, Array[i][j] );
                }
                printf( "\n" );
              }
              getchar();

}while(1);
    return 0;
}
