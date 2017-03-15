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
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>
 
#define BUFSIZE 128
#define TAG 0
#define SIZE 1

using namespace std;
 
typedef struct {
	int procCount;
	int procId;
} T_SORT;


long long int numCnt = 0;


/*
 * Funkce pro řídící procesor
 */
void executiveSort(T_SORT* sort){
	ifstream inputFile;
	inputFile.open("numbers", ios::binary);
	int number;
	list<int>::iterator i;
	list<int> numbers;
	
	while(1){
		number = inputFile.get();
		// Zjistíme, že jsme na konci souboru, poslední číslo je -1 (EOF)
		if(inputFile.eof())
			break;
		numbers.push_back(number);
		//cerr<<"Number "<<x<<": "<<number<<endl;
	}
	
	inputFile.close();
	
	// Vypsání načtených čísel
	for(i = numbers.begin(); i != numbers.end(); ++i)
		cerr<<*i<<" ";
	cerr<<endl;
	
	numCnt = numbers.size();
	
	// Distribuce čísel na sběrnici
	MPI_Bcast(&numCnt,1,MPI_INT,0,MPI_COMM_WORLD);
	
	
	
	
}

void sortNumbers(T_SORT* sort){
	int x;
	int y;
	int c;
	int z;
	//cerr<<"Funguje"<<endl;
	
}


 int main(int argc, char *argv[])
 {
	T_SORT* sort = new T_SORT;
	 
	/*executiveSort();	
	return EXIT_SUCCESS;
	char idstr[32];
	char buff[BUFSIZE];
	int numprocs;
	int myid;
	int i;*/
	MPI_Status stat; 
 
	MPI_Init(&argc,&argv); /* inicializace MPI */
	MPI_Comm_size(MPI_COMM_WORLD,&sort->procCount); /* zjistĂ­me, kolik procesĹŻ bÄ›ĹľĂ­ */
	MPI_Comm_rank(MPI_COMM_WORLD,&sort->procId); /* zjistĂ­me id svĂ©ho procesu */
   
	if(sort->procId == 0)
		executiveSort(sort);
	else
		sortNumbers(sort);
	
	
	MPI_Finalize(); 
	return EXIT_SUCCESS;
 
   /* ProtoĹľe vĹˇechny programy majĂ­ stejnĂ˝ kĂłd (Same Program, Multiple Data â€“ SPMD) rozdÄ›lĂ­me ÄŤinnost
      programĹŻ podle jejich ranku. Program s rankem 0 rozeĹˇle postupnÄ› vĹˇem zprĂˇvu a pĹ™ijme od vĹˇech
       odpovÄ›ÄŹ 
   */
   /*if(myid == 0)
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
     MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
     sprintf(idstr, "Processor %d ", myid);
     strcat(buff, idstr);
     strcat(buff, "OK\n");
     MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
   }
   return 0;*/
 }

