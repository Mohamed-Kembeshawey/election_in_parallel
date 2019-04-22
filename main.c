#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpich/mpi.h>

int candidats = 0;
int voters = 0;

int* myrandom(int num)
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


void writeRandomVotes(int* vote){
    FILE* myfile;
    myfile = fopen("myfile.txt","a");
    int c = 0;
    for (; c < candidats ; c++)
    {
        fprintf (myfile, "%d ",vote[c]);
    }
    fprintf(myfile , "\n");

    fclose(myfile);
}

void inializeTheVotes(){
    printf("enter number of candidats : ");
    scanf("%d" , &candidats);
    printf("Enter the number of voters : ");
    scanf("%d" , &voters);

    int num = candidats , v;

    int *arr= malloc(sizeof(int)*num);
    for(v = 1; v <= voters ;v++)
    {
        arr=myrandom(num);
        writeRandomVotes(arr);
    }
}

int main(int argc , char* argv[]){

inializeTheVotes();
    
    return 0;
}