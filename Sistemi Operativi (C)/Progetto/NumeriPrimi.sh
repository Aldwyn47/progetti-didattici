#!/bin/bash

if test $# -eq 3
then
	mkfifo miaPipe
    if test $3 = FAST
    then
        make calcFast -f mioMakefile.txt
        make memo -f mioMakefile.txt
        ./calcFast $1 miaPipe & ./memo $1 miaPipe $2
    elif test $3 = NORM
    then
        make calc -f mioMakefile.txt
        make memo -f mioMakefile.txt
        ./calc $1 miaPipe & ./memo $1 miaPipe $2
    else
        echo "errore: la modalità di lancio del programma non corrisponde a un valore valido. Esso deve essere FAST o NORM."
    fi
	rm miaPipe
else
	echo "errore: numero parametri non corretto. Si prega di inserire un intero N, il path del file di destinazione in cui inserire i numeri primi trovati e un valore a scelta tra FAST e NORM per la modalità di lancio."
fi
exit 0