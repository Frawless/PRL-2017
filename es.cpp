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

using namespace std;

// velikost přednášené struktury
#define BUFF_SIZE 2
// Definované TAGy pro komunikaci
#define TAG_DON 0
#define TAG_BUS 1
#define TAG_LIN 2
#define TAG_SOR 3
#define EXEC_PROC 0
 
// Struktura pro vlastnosti procesoru
typedef struct {
	int procCount;
	int procId;
	int nextProc;
	int prevProc;
} T_SORT;

// Struktura pro jednotlivé registry, které se přenáší
typedef struct {
	int value;
	int index;
} T_REG;

// Proměnná pro časovou analýzu
bool analyzis = false;

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
	
	T_REG* regX = new T_REG;
	T_REG* regZ = new T_REG;
	
	// Analýza času
	double time1, time2;
	
	while(1){
		number = inputFile.get();
		// Zjistíme, že jsme na konci souboru, poslední číslo je -1 (EOF)
		if(inputFile.eof())
			break;
		numbers.push_back(number);
	}
	
	inputFile.close();
	
	int z = 1;
	numCnt = numbers.size();
	// Vypsání načtených čísel
	if(!analyzis){
		for(i = numbers.begin(); i != numbers.end(); ++i){
			if(z == numCnt)
				cout<<*i;
			else
				cout<<*i<<" ";
			z++;
		}
		cout<<endl;		
	}

	// Začátek měření
	time1 = MPI_Wtime();
	
	// Rozesílání čísel
	for(int i = 0; i < numCnt; i++)
	{
		regX->value = numbers[i];
		regX->index = i;
		// Sběrnice
		MPI_Send(regX, BUFF_SIZE, MPI_INT, i+1, TAG_BUS, MPI_COMM_WORLD);
		
		// Lineární spojení
		MPI_Send(regX, BUFF_SIZE, MPI_INT, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);
	}
	
	// Přijmutí seřazených čísel
	for(int i = 0; i < numCnt; i++)
	{
		MPI_Recv(regZ, 2, MPI_INT, MPI_ANY_SOURCE, TAG_DON, MPI_COMM_WORLD, &stat);
		sortedNumbers[regZ->index-1] = regZ->value;
	}
	
	// Konec spuštění procesoru
	time2 = MPI_Wtime();
	
	if(!analyzis){
		// Výsledný výpis
		for(int z = 0; z < numCnt; z++)
			cout<<sortedNumbers[z]<<endl;	
	}
	// Výpis času
	else{
		printf("MPI_Wtime() %d: %1.6f s\n",sort->procId, time2-time1);
		fflush(stdout);		
	}
	
	free(regX);
	free(regZ);
}

/* 
 * Funkce pro jednotlivé řadící procesory
 */
void sortNumbers(T_SORT* sort){
	// Jednotlivé registry
	T_REG* regX = new T_REG;
	T_REG* regY = new T_REG;
	T_REG* regZ = new T_REG;
	int regC = 1;
	
	numCnt = sort->procCount-1;
	
	// Nahrání hodnoty X a jeho indexu ze sběrnice
	MPI_Recv(regX, BUFF_SIZE, MPI_INT, EXEC_PROC, TAG_BUS, MPI_COMM_WORLD, &stat);
	
	// Přijmutí seřazených čísel
	for(int i = 0; i < numCnt; i++)
	{
		// Lineární spojení - získání hodnot
		MPI_Recv(regY, BUFF_SIZE, MPI_INT, sort->prevProc, TAG_LIN, MPI_COMM_WORLD, &stat);

		/* 
		 * Každý procesor se neprázdným X a Y porovná X a Y
		 * Nutno ošetřit stejná čísla na základě indexu
		 * viz algortihm 2 v http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.106.4976&rep=rep1&type=pdf 
		 */
		if (regX->value == regY->value && regX->index > regY->index)
			regC++;
		else if (regX->value > regY->value)
			regC++;
		
		// Lineární spojení - posun Y vpravo
		if(sort->nextProc < sort->procCount)
			MPI_Send(regY, BUFF_SIZE, MPI_INT, sort->nextProc, TAG_LIN, MPI_COMM_WORLD);
	}
	
	// Poslání hodnoty X procesoru s ID == C (Sběrnice)
	MPI_Send(regX, BUFF_SIZE, MPI_INT, regC, TAG_SOR, MPI_COMM_WORLD);
	
	// Uložení posladné hodnoty X do registru Z (sorted)	
	MPI_Recv(regZ, BUFF_SIZE, MPI_INT, MPI_ANY_SOURCE, TAG_SOR, MPI_COMM_WORLD, &stat);
	// Získání správného indexu do seřazené posloupnosti
	regZ->index = sort->procId;
	// Odeslání čísla a jeho správného indexu řídícímu procesoru
	MPI_Send(regZ, BUFF_SIZE, MPI_INT, EXEC_PROC, TAG_DON, MPI_COMM_WORLD);
	
	// Vyčištění paměti
	free(regX);
	free(regY);
	free(regZ);
}

// Měření - MPI_wtime

int main(int argc, char *argv[])
{
	// Struktura pro vlastnosti procesorů
	T_SORT* sort = new T_SORT;
 
	MPI_Init(&argc,&argv); /* inicializace MPI */
	MPI_Comm_size(MPI_COMM_WORLD,&sort->procCount); /* zjíštění počtu procesorů*/
	MPI_Comm_rank(MPI_COMM_WORLD,&sort->procId); /* zjištění ID procesoru */
   
	sort->nextProc = sort->procId+1;
	sort->prevProc = sort->procId-1;
	
	// Spuštění procesorů
	if(sort->procId == 0)
		executiveSort(sort);
	else
		sortNumbers(sort);	
	
	free(sort);
	MPI_Finalize(); 
	return EXIT_SUCCESS;
 
 }

