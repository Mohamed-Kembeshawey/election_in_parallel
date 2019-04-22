#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int * random(int num)
{
    int i,j;
    int *arr= malloc(sizeof(int)*num);
    for (i = 0; i < num; i++)
    {
        arr[i] = rand() % num + 1;
        for(j=0;j<i;j++)
        {
            if(arr[j]==arr[i])
                --i;
        }
    }
    return arr;
}
int main()
{
    int i,j,num =5;
    int *arr= malloc(sizeof(int)*num);
    for(i=0;i<10;i++)
    {
        arr=random(num);
        for(int j=0;j<num;j++)
            printf("%d ",arr[j]);
        printf("\n");
    }
    return 0;
}