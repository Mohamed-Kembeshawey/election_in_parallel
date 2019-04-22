#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpich/mpi.h>

int candidats = 3;
int voters = 3;


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



int main(int argc , char* argv[]){

   int vote[] = {1 , 2 , 3};
   writeRandomVotes(vote);
   writeRandomVotes(vote);
   writeRandomVotes(vote);

    return 0;
}