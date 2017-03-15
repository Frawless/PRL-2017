/************************************************
*		Projekt: Projekt do předmětu PRL    	*
*		Enumeration sort						*
*		Autor: Bc. Jakub Stejskal <xstejs24>	*
*		Nazev souboru: enum-sort.cpp			*
*		Datum: 15. 3. 2017						*		
*		Verze: 1.0								*
*************************************************/

/*
  "Hello World" s využitím MPI
*/
#include <mpi.h>
#include <stdio.h>
#include <string.h>
 
#define BUFSIZE 128
#define TAG 0
#define SIZE 1

using namespace std;
 
typedef struct {
	int processorCount;
} T_SORT;



 int main(int argc, char *argv[])
 {
   char idstr[32];
   char buff[BUFSIZE];
   int numprocs;
   int myid;
   int i;
   MPI_Status stat; 
 
   MPI_Init(&argc,&argv); /* inicializace MPI */
   MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /* zjistĂ­me, kolik procesĹŻ bÄ›ĹľĂ­ */
   MPI_Comm_rank(MPI_COMM_WORLD,&myid); /* zjistĂ­me id svĂ©ho procesu */
 
   /* ProtoĹľe vĹˇechny programy majĂ­ stejnĂ˝ kĂłd (Same Program, Multiple Data â€“ SPMD) rozdÄ›lĂ­me ÄŤinnost
      programĹŻ podle jejich ranku. Program s rankem 0 rozeĹˇle postupnÄ› vĹˇem zprĂˇvu a pĹ™ijme od vĹˇech
       odpovÄ›ÄŹ 
   */
   if(myid == 0)
   {
     printf("%d: We have %d processors\n", myid, numprocs);
     for(i=1;i<numprocs;i++)
     {
       sprintf(buff, "Hello %d! ", i);
       MPI_Send(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
     }
     for(i=1;i<numprocs;i++)
     {
       MPI_Recv(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD, &stat);
       printf("%d: %s\n", myid, buff);
     }
   }
   else
   {
     /* obdrĹľĂ­me zprĂˇvu od procesu s rankem 0: */
     MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
     sprintf(idstr, "Processor %d ", myid);
     strcat(buff, idstr);
     strcat(buff, "OK\n");
     /* OdpovĂ­me na zprĂˇvu: */
     MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
   }
 
   MPI_Finalize(); 
   return 0;
 }

