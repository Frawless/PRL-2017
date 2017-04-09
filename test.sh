#!/bin/bash

if test "$#" -ne 1; then
    echo "Chybý počet parametrů! Zadejte prosím počet čísel pro seřazení."
	exit
fi

#pocet cisel bud zadam nebo 10 :)
if [ $# -lt 1 ];then 
    numCount=10;
else
    numCount=$1;
fi;

numProc=$((numCount+1))

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -std=c++11 -o es es.cpp

#vyrobeni souboru s random cisly
dd if=/dev/urandom bs=1 count=$numCount of=numbers &>/dev/null

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $numProc es

#uklid
rm -f enum-sort numbers
