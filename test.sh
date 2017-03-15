#!/bin/bash

#vypise pocet cisel a procesoru
numCount=$1
numProc=$((numCount+1))
echo "Počet čísel:"$numCount
echo "Počet procesorů: "$numProc

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o es es.cpp

#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$numCount of=numbers

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $numProc es

#uklid
rm -f enum-sort numbers
