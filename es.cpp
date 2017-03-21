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
#include <string> 

#define BUFSIZE 1
#define TAG 0
#define SIZE 1

#define TAG_BUS 1
#define TAG_LIN 2
#define TAG_SOR 3
#define TAG_DON_VAL 4
#define TAG_DON_IND 5
#define EXEC_PROC 0

using namespace std;
 
typedef struct {
	int procCount;
	int procId;
	int nextProc;
	int prevProc;
	int regC;
	int regX;
	int regY;
	int regZ;
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
	int sortedNumbers[sort->procCount-1];
	int numberX;
	int indexX;
	
//	cerr<<"Procesor: "<<sort->nextProc<<endl;
	
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
	for(int i = 0; i < numCnt; i++)
	{
		numberX = numbers[i];
		indexX = i;
//		cerr<<"numberX: "<<numberX<<"| indexX: "<<indexX<<" to processor: "<<i+1<<endl;
		// Sběrnice
		MPI_Send(&numberX, BUFSIZE, MPI_INT, i+1, TAG_BUS, MPI_COMM_WORLD);
//		MPI_Send(&indexX, BUFSIZE, MPI_INT, i+1, TAG_BUS, MPI_COMM_WORLD);
		
		// Lineární spojení
		MPI_Send(&numberX, BUFSIZE, MPI_INT, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);
//		MPI_Send(&indexX, BUFSIZE, MPI_INT, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);		
	}
	
	int recv[2];
	
	// Přijmutí seřazených čísel
	for(int i = 0; i < numCnt; i++)
	{
		MPI_Recv(&recv, 2, MPI_INT, MPI_ANY_SOURCE, TAG_DON_VAL, MPI_COMM_WORLD, &stat);
//		MPI_Recv(&sortedIndex, BUFSIZE, MPI_INT, MPI_ANY_SOURCE, TAG_DON_IND, MPI_COMM_WORLD, &stat);
		
//		cerr<<"Sorted: Num:"<<sortedNumber[0]<<" index:"<<sortedNumber[1]<<endl;
		sortedNumbers[recv[1]-1] = recv[0];
	}
//	cerr<<"Přijmuto"<<endl;
	
	// Výsledný výpis
	for(int z = 0; z < (sort->procCount-1); z++)
		cout<<sortedNumbers[z]<<endl;	
}

void sortNumbers(T_SORT* sort){
	int indexZ;
	
	// Nahrání hodnoty X a jeho indexu ze sběrnice
	MPI_Recv(&sort->regX, BUFSIZE, MPI_INT, EXEC_PROC, TAG_BUS, MPI_COMM_WORLD, &stat);
//	MPI_Recv(&indexX, BUFSIZE, MPI_INT, EXEC_PROC, TAG_BUS, MPI_COMM_WORLD, &stat);
	
	// Přijmutí seřazených čísel
	for(int i = 0; i < (sort->procCount-1); i++)
	{
		// Lineární spojení - získání hodnot
		MPI_Recv(&sort->regY, BUFSIZE, MPI_INT, sort->prevProc, TAG_LIN, MPI_COMM_WORLD, &stat);
//		MPI_Recv(&indexY, BUFSIZE, MPI_INT, sort->prevProc, TAG_LIN, MPI_COMM_WORLD, &stat);			
		
		// Každý procesor se neprázdným X a Y porovná X a Y
		// pokud X > Y -> C++
		if (sort->regX > sort->regY)
			sort->regC++;
		
		// Lineární spojení - posun Y vpravo
		if(sort->nextProc < sort->procCount)
		{
			MPI_Send(&sort->regY, BUFSIZE, MPI_INT, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);
//			MPI_Send(&indexY, BUFSIZE, MPI_INT, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);			
		}
	}
	
//	cerr<<"Procesor: "<<sort->procId<<" Hodnota X: "<<sort->regX<<" Správný index: "<<sort->regC<<endl;
	
	// Poslání hodnoty X procesoru s ID == C (Sběrnice)
	MPI_Send(&sort->regX, BUFSIZE, MPI_INT, sort->regC, TAG_SOR, MPI_COMM_WORLD);
//	MPI_Send(&(sort->regC), BUFSIZE, MPI_INT, sort->regC, TAG_SOR, MPI_COMM_WORLD);
	
	// Uložení posladné hodnoty X do registru Z (sorted)	
	MPI_Recv(&sort->regZ, BUFSIZE, MPI_INT, MPI_ANY_SOURCE, TAG_SOR, MPI_COMM_WORLD, &stat);
//	MPI_Recv(&indexZ, BUFSIZE, MPI_INT, MPI_ANY_SOURCE, TAG_SOR, MPI_COMM_WORLD, &stat);
	
//	cerr<<"Procesor: "<<sort->procId<<" Hodnota Z: "<<sort->regZ<<endl;
	
	int send[2];
	send[0] = sort->regZ;
	send[1] = sort->procId;
	
	// Odeslání čísla a jeho správného indexu řídícímu procesoru
	MPI_Send(&send, 2, MPI_INT, EXEC_PROC, TAG_DON_VAL, MPI_COMM_WORLD);
	
//	cerr<<"Ukončuji procesor: "<<sort->procId<<endl;
//	
//	indexZ = sort->procId;
//	cerr<<"Procesor: "<<sort->procId<<" Zindex před odesláním: "<<indexZ<<endl;
////	
//	MPI_Send(&indexZ, BUFSIZE, MPI_INTEGER, EXEC_PROC, TAG_DON_IND, MPI_COMM_WORLD);
//	cerr<<"Procesor: "<<sort->procId<<" Odesilá hodnotu a správný index procesoru 0"<<endl;
}

// Měření - MPI_wtime

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
	double time1, time2;
	bool analyzis = false;
 
	MPI_Init(&argc,&argv); /* inicializace MPI */
	MPI_Comm_size(MPI_COMM_WORLD,&sort->procCount); /* zjistĂ­me, kolik procesĹŻ bÄ›ĹľĂ­ */
	MPI_Comm_rank(MPI_COMM_WORLD,&sort->procId); /* zjistĂ­me id svĂ©ho procesu */
   
	sort->regC = 1;
	sort->nextProc = sort->procId+1;
	sort->prevProc = sort->procId-1;

	// Začátek spuštění procesoru
	time1 = MPI_Wtime();
	
	if(sort->procId == 0)
		executiveSort(sort);
	else
		sortNumbers(sort);
	
	// Konec spuštění procesoru
	time2 = MPI_Wtime();
	
	// Výpis času
	if(analyzis)
	{
		printf("MPI_Wtime() procesoru %d: %1.6f\n",sort->procId, time2-time1);
		fflush(stdout);		
	}
	
	free(sort);
	MPI_Finalize(); 
	return EXIT_SUCCESS;
 
 }

