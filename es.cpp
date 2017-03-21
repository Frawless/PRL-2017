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
#include <vector>
 
#define BUFSIZE 8
#define TAG 0
#define SIZE 1

#define TAG_BUS 1
#define TAG_LIN 2
#define TAG_SOR 3
#define TAG_DON 4
#define EXEC_PROC 0

using namespace std;
 
typedef struct {
	int procCount;
	int procId;
	int nextProc;
	int prevProc;
} T_SORT;


long long int numCnt = 0;

MPI_Status stat;


/*
 * Funkce pro řídící procesor
 */
void executiveSort(T_SORT* sort){
	ifstream inputFile;
	inputFile.open("numbers", ios::binary);
	sort->nextProc = 1;
	sort->prevProc = -1;
	int number;
	vector<int>::iterator i;
	vector<int> numbers;
	vector<int> sortedNumbers;
	int numberX;
	int indexX;
	
	cerr<<"Procesor: "<<sort->nextProc<<endl;
	
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
		cout<<*i<<" ";
	cout<<endl;
	
	numCnt = numbers.size();
	
	// Distribuce počtu čísel na sběrnici
//	MPI_Bcast(&numCnt,1,MPI_INT,0,MPI_COMM_WORLD);
	
	// Rozesílání čísel
	for(int i = 0; i < (sort->procCount-1); i++)
	{
		numberX = numbers[i];
		indexX = i;
		cerr<<"numberX: "<<numberX<<"| indexX: "<<indexX<<endl;
		// Sběrnice
		MPI_Send(&numberX, BUFSIZE, MPI_INTEGER, i+1, TAG_BUS, MPI_COMM_WORLD);
		MPI_Send(&indexX, BUFSIZE, MPI_INTEGER, i+1, TAG_BUS, MPI_COMM_WORLD);
		
		// Lineární spojení
		MPI_Send(&numberX, BUFSIZE, MPI_INTEGER, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);
		MPI_Send(&indexX, BUFSIZE, MPI_INTEGER, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);
	}
	
	
	// Přijmutí seřazených čísel
	for(int i = 0; i < (sort->procCount-1); i++)
	{
		cerr<<"Procesor "<<EXEC_PROC<<" přijímá seřazená čísla..."<<endl;
		MPI_Recv(&numberX, BUFSIZE, MPI_INTEGER, MPI_ANY_SOURCE, TAG_DON, MPI_COMM_WORLD, &stat);
		MPI_Recv(&indexX, BUFSIZE, MPI_INTEGER, MPI_ANY_SOURCE, TAG_DON, MPI_COMM_WORLD, &stat);
		cerr<<"přijmuto"<<endl;
		sortedNumbers[indexX] = numberX;
	}
	
	for(i = sortedNumbers.begin(); i != sortedNumbers.end(); ++i)
		cout<<*i<<"\n";
	cout<<endl;
	
	
}

void sortNumbers(T_SORT* sort){
	int X;
	int indexX;
	int Y;
	int indexY;
	int C  = 1;
//	cerr<<"C:"<<C<<endl;
	int Z;
	int indexZ = sort->procId;
	sort->nextProc = sort->procId+1;
	sort->prevProc = sort->procId-1;
	
	cerr<<"Procesor: "<<sort->nextProc<<endl;
	
	// Nahrání hodnoty X a jeho indexu ze sběrnice
	MPI_Recv(&X, BUFSIZE, MPI_INTEGER, EXEC_PROC, TAG_BUS, MPI_COMM_WORLD, &stat);
	MPI_Recv(&indexX, BUFSIZE, MPI_INTEGER, EXEC_PROC, TAG_BUS, MPI_COMM_WORLD, &stat);
	
	cerr<<"X: "<<X<<endl;
	
	// Přijmutí seřazených čísel
	for(int i = 0; i < (sort->procCount-1); i++)
	{
		// Lineární spojení - získání hodnot
		MPI_Recv(&Y, BUFSIZE, MPI_INTEGER, sort->prevProc, TAG_LIN, MPI_COMM_WORLD, &stat);
		MPI_Recv(&indexY, BUFSIZE, MPI_INTEGER, sort->prevProc, TAG_LIN, MPI_COMM_WORLD, &stat);
		
		cerr<<"Y: "<<Y<<endl;
		
		// Každý procesor se neprázdným X a Y porovná X a Y
		// pokud X > Y -> C++
//		if (X > Y)
//			C++;
		//C = X > Y ? C+1 : C;
		
		cerr<<"C:"<<C<<" P: "<<sort->procId<<endl;
		
		// Lineární spojení - posun Y vpravo
		if(sort->nextProc < sort->procCount)
		{
			MPI_Send(&Y, BUFSIZE, MPI_INTEGER, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);
			MPI_Send(&indexY, BUFSIZE, MPI_INTEGER, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);			
		}

	}
	
	cerr<<"test"<<endl;
	
	// Poslání hodnoty X procesoru s ID == C (Sběrnice)
	MPI_Send(&X, BUFSIZE, MPI_INTEGER, C, TAG_SOR, MPI_COMM_WORLD);
	MPI_Send(&indexX, BUFSIZE, MPI_INTEGER, C, TAG_SOR, MPI_COMM_WORLD);
	// Tady ještě OK
	
	
	// Uložení posladné hodnoty X do registru Z (sorted)	 !!!TADY TO NEPROJDE!!!
	MPI_Recv(&Z, BUFSIZE, MPI_INTEGER, MPI_ANY_SOURCE, TAG_SOR, MPI_COMM_WORLD, &stat);
	MPI_Recv(&indexZ, BUFSIZE, MPI_INTEGER, MPI_ANY_SOURCE, TAG_SOR, MPI_COMM_WORLD, &stat);
	
	cerr<<"Funguje"<<endl;
	
	// Odeslání čísla a jeho správného indexu řídícímu procesoru
	MPI_Send(&Z, BUFSIZE, MPI_INTEGER, EXEC_PROC, TAG_DON, MPI_COMM_WORLD);
	MPI_Send(&indexZ, BUFSIZE, MPI_INTEGER, EXEC_PROC, TAG_DON, MPI_COMM_WORLD);
	
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
       MPI_Send(buff, BUFSIZE, MPI_INTEGER, i, TAG, MPI_COMM_WORLD);
     }
     for(i=1;i<numprocs;i++)
     {
       MPI_Recv(buff, BUFSIZE, MPI_INTEGER, i, TAG, MPI_COMM_WORLD, &stat);
       printf("%d: %s\n", myid, buff);
     }
   }
   else
   {
     MPI_Recv(buff, BUFSIZE, MPI_INTEGER, 0, TAG, MPI_COMM_WORLD, &stat);
     sprintf(idstr, "Processor %d ", myid);
     strcat(buff, idstr);
     strcat(buff, "OK\n");
     MPI_Send(buff, BUFSIZE, MPI_INTEGER, 0, TAG, MPI_COMM_WORLD);
   }
   return 0;*/
 }

