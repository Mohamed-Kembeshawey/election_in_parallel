#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpich/mpi.h>

int candidats = 10;
int voters = 0;


int getDigits(int num)
{
    int size=1;
    while (num>9)
    {
        num/=10;
        size++;
    }
    return size;
}


void convert(FILE* myfile , int num)
{
    int size,copy,i,rem;
    size=getDigits(candidats);
    char* arr=malloc(sizeof(char)*size);
    copy=getDigits(num);
    for(i=0;i<size-copy;i++)
        arr[i]='0'; //you can print 0 in file and do not use array
    for(i=size-1;i>=size-copy;i--)
    {
        rem=num/10;
        rem=num-(rem*10);
        if(rem==0)
            arr[i]='0';
        else
        {
            arr[i]=(char)rem+48;
            num-=rem;
        }
        num/=10;
    }
    for(i = 0; i<size ; i++){
        fprintf(myfile , "%c" , arr[i]);
    }
    fprintf(myfile , " ");
}



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
        //fprintf (myfile, "%d ",vote[c]);
        convert(myfile , vote[c]);
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

    
    FILE* myfile;
    myfile = fopen("overhead.txt","a");
    fprintf (myfile, "%d ",candidats);
    fprintf (myfile, "%d ",voters);
    fclose(myfile);


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
