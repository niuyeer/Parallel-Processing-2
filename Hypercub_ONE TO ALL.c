#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <math.h>
#include "time.h"//clock() get time
#define SIZE 10000000
int array[SIZE];
int d=4;

int power(int n,int m)
{
    int i,sum=1;
   for(i=1;i<=m;i++)
   { sum*=n;}
    return sum;
    
}

//struct timeval start1,end1,start2,end2,start3,end3;

int main(int argc, char ** argv){
  // gettimeofday(&start1,NULL);
    int k;
    int myNode,allNodes;
    int long sum=0,partialSum=0,accum=0,rootSum=0;
    int start,end;
    clock_t t1,t2,t3,t4,t5;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &myNode);
    MPI_Comm_size( MPI_COMM_WORLD, &allNodes);
   
   // MPI_Barrier(MPI_COMM_WORLD);
   //t1=MPI_Wtime();
    t1=clock();

    if (myNode==0)
    {
        
        //create array
        int n;
        srand((unsigned)time(NULL));
        for (n=0;n<SIZE;n++)
        {
            array[n]=rand()%10;
            //printf("number %d , %d process\n", array[n],myNode);
        }
        
        
    }
    t2=clock();
     //gettimeofday(&start2,NULL);
    //one to all broadcast
    int msg_des,msg_sour;
    k=power(2,d)-1;
   // printf("k=%d\n",k);
    int i;
    for (i=d-1; i>=0; i--) {
        k=k^power(2,i);
       // printf("k=%d\n",k);
        if ((myNode&k)==0) {
            if ((myNode&power(2,i))==0) {
                msg_des=myNode^power(2,i);
                 printf("from %d to %d process\n", myNode,msg_des);
                 MPI_Send( &array, SIZE, MPI_INT, msg_des, 0, MPI_COMM_WORLD );
            }
            else
            { msg_sour=myNode^power(2,i);
              //  printf("sour %d , %d process\n", msg_sour,myNode);
             MPI_Recv( &array, SIZE, MPI_INT, msg_sour, 0, MPI_COMM_WORLD,&status );
                
            }
        }
    }
   // printf("number %d , %d process\n", array[1],myNode);
  //   gettimeofday(&end2,NULL);
    /*divde*/
   // t2=MPI_Wtime();;
   t3=clock();
    partialSum=0;
    start = SIZE*myNode/allNodes;
    end = SIZE*(myNode+1)/allNodes-1;
    if (myNode==allNodes-1)
    {
        end=SIZE-1;
    }
    
    for(i=start;i<=end;i++)
    {
        partialSum+=array[i];
    }
    printf("partial sum %d calculated by %d process\n", partialSum,myNode);
  //  t3=MPI_Wtime();;
   t4=clock();
    k=0;
    for (i=0;i<=d-1;i++)
    {
        if ((myNode&k)==0)
        {
            if ((myNode&power(2,i))!=0)
            {
                msg_des=myNode^power(2,i);
                MPI_Send( &partialSum, 1, MPI_LONG, msg_des, 0, MPI_COMM_WORLD );
                
            }
            else{
                msg_sour=myNode^power(2,i);
                MPI_Recv( &partialSum, 1, MPI_LONG, msg_sour, 0, MPI_COMM_WORLD,&status );
                partialSum+=partialSum;
            }
            k=k^power(2,i);
        }
    }
   
    t5=clock();
    
   // MPI_Reduce(&partialSum, &accum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
   // t4=clock();
   // t4=MPI_Wtime();
    printf("total time=%d us from node%d\n",t5-t1,myNode);
    printf("calcualtion time=%d us from node%d\n",t2-t1+t4-t3,myNode);
    printf("communication time=%d us from node%d\n",t5-t4+t3-t2,myNode);
    
    
    
    if(myNode==0){
        printf("pross=%d sum=%d\n",myNode,partialSum);
        
       // gettimeofday(&end1,NULL);
      //  double timeUse = end1.tv_sec - start1.tv_sec + (end1.tv_usec - start1.tv_usec)/1000000.0;
      //  printf("the total time is %d\n",timeUse);
    }
    
  //  printf("the calculation time is %d from node %d\n",t3-t2),myNode;
  //  printf("the communication time is %d from node %d\n",t2-t1+t4-t3,myNode);
 //  printf("the total time is %d from node %d\n",t4-t1,myNode);
   // printf("The compution time = %d the communication time = %d from node%d\n",(t3-t2),(t2-t1+t4-t3),myNode);
    MPI_Finalize();
    return 0;
}


