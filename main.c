#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpich/mpi.h>

int candidats = 0;
int voters = 0;
int startByteOffset = 0;
int fMax = 0;
int sMax = 0;
int idxfMax = 0;
int idxsMax = 0;
int increamentd = 0;

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
            arr[i] = '0';//you can print in file here and did not use array
        else
        {
            arr[i]=(char)rem+48;//and here
            num-=rem;
        }
        num /=10;
    }
    //OR you can print the whole array here
    for(i = 0; i<size ; i++){
        fprintf(myfile , "%c" , arr[i]);
      //  printf("%c" , arr[i]);
    }
    //printf(" ");
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

    int  v;
    printf("enter number of candidats : ");
    scanf("%d" , &candidats);
    printf("Enter the number of voters : ");
    scanf("%d" , &voters);


    FILE* myfile;
    myfile = fopen("myfile.txt","a");
    fprintf (myfile, "%d\n",candidats);
    fprintf (myfile, "%d\n",voters);
    fclose(myfile);


    int* arr= malloc(sizeof(int) * candidats);
    for(v = 1; v <= voters ;v++)
    {
        arr=myrandom(candidats);
        writeRandomVotes(arr , candidats);
    }

}

int getmyByteoffset(int rank , int portion){
    return rank * portion * (candidats * (getDigits(candidats)+1) + 1) +  startByteOffset;
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

int getMax(int* arr){
    int max = 0 , i;
    int idx = -1;
    for(i = 0 ; i<candidats; i++){
        if(arr[i] > max){
            max = arr[i];
            idx = i;
        }
    }
    return idx;
}

int getSecondMax(int* arr , int index){
    int max = 0 , i;
    int idx = -1;
    for(i = 0 ; i<candidats; i++){
        if(arr[i] > max && i != index ){
            max = arr[i];
            idx = i;
        }
    }
    return idx;
}

int main(int argc , char* argv[]){
    int i;
    int *localCounter;
    int *secondLocalCounter;
    int *secondglobalCounter;
    int *globalCounter;
    FILE* myfile;
    int p , rank , portion , rem , *myvoters;
    MPI_Init(&argc , &argv);

    MPI_Comm_rank(MPI_COMM_WORLD , &rank);
    MPI_Comm_size(MPI_COMM_WORLD , &p);

    if(rank == 0 ){
        inializeTheVotes();
        readFile();
        portion  = voters / p;
        rem = voters % p;
        globalCounter = (int*)malloc(candidats*sizeof(int));
    }
    MPI_Bcast(&candidats , 1  , MPI_INT , 0 , MPI_COMM_WORLD );
    MPI_Bcast(&voters , 1  , MPI_INT , 0 , MPI_COMM_WORLD );
    MPI_Bcast(&startByteOffset , 1  , MPI_INT , 0 , MPI_COMM_WORLD );
    MPI_Bcast(&portion , 1  , MPI_INT , 0 , MPI_COMM_WORLD );
    localCounter=(int*)malloc(candidats*sizeof(int));
    for(i=0;i<candidats;i++)
        localCounter[i]=0;

    myfile = fopen("myfile.txt","r");
    int start = getmyByteoffset(rank , portion);
    fseek(myfile, start , SEEK_SET);
    int localVote;
    for(i=0;i<portion;i++)
    {
        fscanf(myfile,"%d",&localVote);
        //printf("for p#%d -- > num : %d\n" ,rank , localVote);
        fseek(myfile, ( 2 * getDigits(candidats)+candidats+1) , SEEK_CUR);
        localCounter[localVote-1]++;
    }
    

    for(i=0; i<candidats; i++)
        MPI_Reduce(&localCounter[i] , &globalCounter[i] , 1 , MPI_INT , MPI_SUM , 0 , MPI_COMM_WORLD );
    
    if(rank == 0){
        /*int start = getmyByteoffset(p, portion);    
        printf("strat %d\n",start);
        printf("boooom\n");
        fseek(myfile , start , SEEK_SET);
        for(i = portion * p; i<voters ; i++){
            fscanf(myfile,"%d",&localVote);
            printf("%d \n" , localVote);-------------------
strat 29
boooom
the winner is #2 in round 1
            fseek(myfile, ( 2 * getDigits(candidats)+candidats+1) , SEEK_CUR);
            printf("globalCounter= %d\n",globalCounter[localVote-1]);
            globalCounter[localVote-1]++;
            printf("globalCounter++= %d\n",globalCounter[localVote-1]);

        }
*/
        int idx = getMax(globalCounter);
        if(globalCounter[idx] > (voters / 2) ){
            printf("the winner is #%d in round 1\n" , idx+1);
            return 0;
        }
        // in round 2
        int idxSecondMax = getSecondMax(globalCounter , idx);
        fMax = globalCounter[idx];
        idxfMax = idx;
        sMax = globalCounter[idxSecondMax];
        idxsMax = idxSecondMax;
        //printf("idx%d  : %d\n" , idxfMax+1 , fMax );
        //printf("idx%d  : %d\n" , idxsMax+1 , sMax );

    }
    MPI_Bcast(&idxfMax , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast(&idxsMax , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
    secondLocalCounter = (int*) malloc( 2 * sizeof(int));
    for(i = 0; i<2; i++){
        secondLocalCounter[i] = 0;
    }

    fseek(myfile, start , SEEK_SET);
        
    for(i=0;i<portion;i++)
    {
        int c;
        for(c = 0 ; c<candidats; c++){
            fscanf(myfile,"%d",&localVote);
            
            if(localVote == idxfMax+1 && !increamentd){
                secondLocalCounter[0]++;
                increamentd = 1;
            }
            if(localVote == idxsMax+1 && !increamentd)
            {
                secondLocalCounter[1]++;
                increamentd = 1;
            }
            
            fseek(myfile, 1 , SEEK_CUR);
        }
        fseek(myfile, 1 , SEEK_CUR);
        increamentd = 0;
    }

    if(rank == 0){
        secondglobalCounter = (int*) malloc(2 * sizeof(int));
    }

    for(i = 0; i<2; i++){
        MPI_Reduce(&secondLocalCounter[i] , &secondglobalCounter[i] , 1 , MPI_INT , MPI_SUM , 0 , MPI_COMM_WORLD );
    }


    if(rank == 0){

        if(secondglobalCounter[0] > secondglobalCounter[1]){
            printf("the winner is #%d in round 2\n" , idxfMax+1);
        }else
        {
            printf("the winner is #%d in round 2\n" , idxsMax+1);
        }
        
    }
        
    
    fclose(myfile);
    MPI_Finalize();
    return 0;
}
