#################################################
#	Projekt: Projekt do předmětu PDS	#
#		 MitM attack			#
#	 Autoři: Bc. Jakub Stejskal <xstejs24>	#
# Nazev souboru: Makefile			#
#	  Datum: 13. 2. 2017		    	#		
#	  Verze: 1.0				#
#################################################

all:
	mpic++ -o enum-sort enum-sort.cpp #jazyk c++
    
clean:
	rm -f *.o *.out $(FILES)