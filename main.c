#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpich/mpi.h>

int candidats = 0;
int voters = 0;
int startByteOffset = 0;


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
            arr[i]=(char)(num/10)+48;//you can print in file here and did not use array
        else
        {
            arr[i]=(char)rem+48;//and here
            num-=rem;
        }
    }
    //OR you can print the whole array here
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


void writeRandomVotes(int* vote , int numofCandidates){
    FILE* myfile;
    myfile = fopen("myfile.txt","a");
    int c = 0;
    for (; c < numofCandidates ; c++)
    {
        //fprintf (myfile, "%d ",vote[c]);
        convert(myfile , vote[c]);
    }
    fprintf(myfile , "\n");

    fclose(myfile);
}

void inializeTheVotes(){
    
    int numOfCandidates , numOfVoters , v;
    printf("enter number of candidats : ");
    scanf("%d" , &numOfCandidates);
    printf("Enter the number of voters : ");
    scanf("%d" , &numOfVoters);

    
    FILE* myfile;
    myfile = fopen("myfile.txt","a");
    fprintf (myfile, "%d\n",numOfCandidates);
    fprintf (myfile, "%d\n",numOfVoters);
    fclose(myfile);


    int *arr= malloc(sizeof(int)*numOfCandidates);
    for(v = 1; v <= numOfVoters ;v++)
    {
        arr=myrandom(numOfCandidates);
        writeRandomVotes(arr , numOfCandidates);
    }
    
}

int getmyByteoffset(int rank , int portion){
    return rank * portion * candidats * (getDigits(candidats)+1) +  startByteOffset;
}

void readFile(){
    FILE* myfile;
    myfile = fopen("myfile.txt","r");

    fseek(myfile , 0 , SEEK_SET);
    fscanf(myfile , "%d" , &candidats);
    
    fseek(myfile , 1 , SEEK_CUR);
    fscanf(myfile , "%d" , &voters);
    
    fseek(myfile , 1 , SEEK_CUR);
    
    startByteOffset = ftell(myfile);

    fclose(myfile);
}

int main(int argc , char* argv[]){    

    int p , rank , portion , rem , *myvoters;
    MPI_Init(&argc , &argv);

    MPI_Comm_rank(MPI_COMM_WORLD , &rank);
    MPI_Comm_size(MPI_COMM_WORLD , &p);

    if(rank == 0 ){
        inializeTheVotes();
        readFile();

        portion  = voters / p;
        rem = voters % p;
    }
    FILE* myfile;
    myfile = fopen("myfile.txt","r");
    MPI_Bcast(&candidats , 1  , MPI_INT , 0 , MPI_COMM_WORLD );
    MPI_Bcast(&voters , 1  , MPI_INT , 0 , MPI_COMM_WORLD );
    MPI_Bcast(&startByteOffset , 1  , MPI_INT , 0 , MPI_COMM_WORLD );
    MPI_Bcast(&portion , 1  , MPI_INT , 0 , MPI_COMM_WORLD );

    int start = getmyByteoffset(rank , portion);
    //fseek(myfile , start , SEEK_SET);
    //int x;
    //fscanf(myfile , "%d" , &x);
    printf("for process#%d --> the start is %d \n" ,rank , start);

    
    MPI_Finalize();
fclose(myfile);
    return 0;
}