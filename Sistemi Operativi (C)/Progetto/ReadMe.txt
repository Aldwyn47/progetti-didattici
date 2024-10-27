A cura di Andrea Imparato (323840)

Per lanciare il programma è sufficiente lanciare lo script NumeriPrimi.sh includendo tre parametri:
-Un intero N
-Il nome del file in cui salvare i numeri primi
-Un parametro a scelta tra NORM e FAST

Ad esempio: "./NumeriPrimi.sh 1000000 fileNumeri.txt NORM"

L'ultimo parametro serve a scegliere uno tra due possibili eseguibili con cui avviare il programma "Calcolatori".
La variante FAST svolge i calcoli nel modo più rapido possibile, ma a scapito di alcune modifiche come l'esclusione di diverse stampe a video.
La variante NORM è maggiormente conforme a quanto richiesto dalla traccia (per cui i calcolatori stampano a video ma lavorano più lentamente).

Entrambe le versioni sono state testate con diversi input. I risultati dei test sono disponibili nel file "TestTempi.txt"