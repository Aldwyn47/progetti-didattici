#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>

#define error(msg) {perror(msg);exit(1);}

int cmpfunc(const void *a, const void *b); //Funzione di comparison usata da quicksort

const unsigned int blockSize = PIPE_BUF/sizeof(unsigned int); //Per minimizzare l'overhead dovuto alla comunicazione conviene far sì che ogni trasmissione al memorizzatore non invii un singolo intero ma quanti più interi gli sia possibile. Pertanto non verrà inviato un intero per volta ma un intero buffer/array contenente il massimo numero di interi scrivibili con una singola write.

int main (int argc, char* argv[]){
    if (argc!=4)
        error("Errore nell'avvio del programma Memorizzatore: numero di parametri non corretto.\n");
    unsigned int N = (unsigned int)(atoi(argv[1]));
    if (N==0 || N>UINT_MAX-100)
        error("Si è verificato un errore nel Memorizzatore: il numero N passato in input non costituisce un valore corretto.\n");
    printf("Il Memorizzatore è operativo, lavorerà ricevendo blocchi grandi %u interi.\n", blockSize);
    setpriority(PRIO_PROCESS, getpid(), -20); //Viene impostata la priorità massima al processo in modo da sveltire i calcoli per N molto grandi
    unsigned int stimaMassima = N/2 + 2; //Stiamo qui supponendo che il numero massimo di numeri primi che riceveremo non potrà mai superare la quantità N/2+2 (giacché tutti i numeri pari non sono primi). Ovviamente è una stima molto larga, per tenerci sicuri.
    unsigned long long arraySize = (( stimaMassima / (blockSize-2) ) + 1)*blockSize; //La dimensione dell'array di appoggio verrà calcolata proporzionalmente a quanto voluminosa è la quantità di numeri primi che ci aspettiamo di ricevere (giacché un volume molto grande richiederà più trasmissioni). In ogni caso, la dimensione minima dell'array di appoggio non deve mai scendere sotto quella di un singolo blocco di trasmissione. 
    unsigned int *appoggio = (unsigned int*)malloc(sizeof(unsigned int)*arraySize); //Creazione dell'array di appoggio. Va creato con la memoria dinamica perché quelli statici hanno un limite alla grandezza troppo basso che rischia facilmente di causare segmentation fault
    if (appoggio==NULL)
        error("Il Memorizzatore ha incontrato un problema imprevisto durante la creazione dell'array di appoggio.\n");
    unsigned long long indiceArray = 0; //Indice che tiene traccia della prossima posizione libera nell'array
    int fd; //File descriptor usato per leggere dalla Pipe e scrivere sul file di destinazione finale
    char flag = 1; //Flag usato per l'attesa
    if ((fd = open(argv[2], O_RDONLY))==-1)
        error("Si è verificato un errore nel Memorizzatore mentre tentava di aprire la pipe in lettura.\n");
    while(flag){ //Il memorizzatore si mette in ascolto usando un ciclo su un flag, ma la prima operazione che esegue all'interno del ciclo è una read() sulla pipe che lo mette in attesa passiva evitando sprechi
        if (read(fd, (char*)appoggio+(indiceArray*sizeof(unsigned int)), blockSize*sizeof(unsigned int))!=blockSize*sizeof(unsigned int)) //Usando casting di tipo a char* e aritmetica dei puntatori possiamo fare in modo che la read alleghi i dati di ogni blocco appena letto in modo che risultino contigui a quelli del blocco precedente
            error("Si è verificato un errore nel Memorizzatore mentre tentava di leggere dalla pipe.\n");
        indiceArray = indiceArray + blockSize; //Ogni volta che viene ricevuta una trasmissione l'indice dell'array di appoggio viene fatto scorrere in avanti di un intero blocco
        if (appoggio[indiceArray-1] == 4){ //Se il blocco appena ricevuto è l'ultimo della trasmissione viene aggiornato il flag che determina l'uscita dal ciclo e l'indice dell'array viene fatto arretrare fino alla cella che conteneva effettivamente l'ultimo valore valido
            flag = 0;
            indiceArray = indiceArray - blockSize + appoggio[indiceArray-2];
            printf("Il memorizzatore ha ricevuto con successo l'ultima trasmissione.\n");
        }
        else{
            indiceArray = indiceArray - 2; //Se il blocco non è l'ultimo della trasmissione vengono troncati gli ultimi due valori (che sono codici di trasmissione e non valori trasmessi).
            printf("Il memorizzatore ha ricevuto con successo una trasmissione.\nUltimo intero ricevuto: %u\n", appoggio[indiceArray-1]);
        }
    }
    close(fd); //Terminata la ricezione la pipe viene chiusa.
    printf("Il memorizzatore procederà ora a ordinare gli interi ricevuti.\n");
    qsort(appoggio, indiceArray, sizeof(unsigned int), cmpfunc); //L'array dei valori ottenuti viene ordinato con Quicksort
    printf("Il memorizzatore ha terminato l'ordinamento. Si occuperà ora di scrivere gli interi sul file di destinazione.\n");
    if ((fd = open(argv[3], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO))==-1)
        error("Si è verificato un errore nel Memorizzatore mentre tentava di aprire il file in cui scrivere i risultati del calcolo.\n");
    int fdtemp = dup(STDOUT_FILENO); //Copia temporanea dello stream di output
    if((dup2(fd, STDOUT_FILENO))!=STDOUT_FILENO) //Per la scrittura finale il memorizzatore redirige il suo stream di output direttamente sul file finale
        error("Si è verificato un errore nella redirezione finale dello stream del Memorizzatore.\n");
    for (int z = 0; z < indiceArray; z++)
        printf("%u\n",appoggio[z]); //Grazie alla redirezione dello stream la formattazione da numeri a caratteri può essere affidata direttamente alla funzione printf
    close(fd); //Terminata la scrittura su file, il file descriptor viene chiuso.
    if((dup2(fdtemp, STDOUT_FILENO))!=STDOUT_FILENO) //Il memorizzatore ripristina stdout per scrivere il suo messaggio finale
        error("Si è verificato un errore nella redirezione finale dello stream del Memorizzatore.\n");
    free(appoggio);
    printf("Il memorizzatore ha concluso il suo lavoro.\n");
    return 0;
}

int cmpfunc(const void *a, const void *b){
  if(*(unsigned int *)a  <  *(unsigned int *)b) return -1;
  if(*(unsigned int *)a  == *(unsigned int *)b) return 0;
  if(*(unsigned int *)a  >  *(unsigned int *)b) return 1; 
}

