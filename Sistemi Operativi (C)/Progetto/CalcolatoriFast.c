#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define error(msg) {perror(msg);exit(1);}

const unsigned int blockSize = PIPE_BUF/sizeof(unsigned int); //Per minimizzare l'overhead dovuto alla comunicazione conviene far sì che ogni trasmissione al memorizzatore non invii un singolo intero ma quanti più interi gli sia possibile. Pertanto non verrà inviato un intero per volta ma un intero buffer/array contenente il massimo numero di interi scrivibili con una singola write.

unsigned int indice; //Indice di scorrimento usato per navigare il buffer in uso
unsigned int **buffer; //Puntatore ad array con cui si accede al buffer in uso
unsigned int **radio; //Un secondo puntatore ad array che viene usato come riferimento di appoggio a cui agganciare un buffer pieno in modo che un apposito thread "trasmettitore" possa poi trasmetterlo.
unsigned int N = 0; //Il totale N di interi su cui il programma deve lavorare, il main lo aggiornerà con il parametro passato dall'esterno. La sincronizzazione qui non è necessaria perché i thread accedono in sola lettura.
unsigned int activeSubThreads = 0; //Variabile che tiene traccia dei thread attivi, i thread la useranno per inizializzare il loro identificativo
int fd; //Variabile di appoggio per il file descriptor della pipe.
char flag = 1; //Flag usato dal thread trasmettitore per sapere quando il suo lavoro finisce

pthread_mutex_t sezCritT = PTHREAD_MUTEX_INITIALIZER; //Mutex usato dai thread per accedere alla variabile tramite cui inizializzeranno il loro identificativo
pthread_mutex_t sezCritB = PTHREAD_MUTEX_INITIALIZER; //Mutex usato per sincronizzare l'accesso al buffer di appoggio
sem_t bufferPieno; //Semaforo usato dai thread per segnalare al trasmettitore che c'è un blocco pieno da inviare
sem_t trasmettitorePronto; //Semaforo usato dal trasmettitore per segnalare ai thread che è pronto a ricevere un nuovo blocco da inviare

int primalTest (unsigned int n); //Dichiarazione della funzione per il test di primalità
void calcolatore(void* arg); //Dichiarazione della funzione per i thread calcolatori
void trasmettitore(void* arg); //Dichiarazione della funzione per il thread incaricato di trasmettere

int main(int argc, char* argv[]){
    if (argc!=3)
        error("Si è verificato un errore nel main di Calcolatori: il numero di parametri inseriti non è corretto.\n");
    N = (unsigned int)(atoi(argv[1]));
    if (N==0 || N>UINT_MAX-100) //Ho scelto arbitrariamente il valore UINT_MAX-100 come valore massimo tollerabile dal programma (il -100 è per stare più larghi)
        error("Si è verificato un errore nel main di Calcolatori: il numero N passato in input non costituisce un valore corretto.\n");
    setpriority(PRIO_PROCESS, getpid(), -20); //Viene impostata la priorità massima al processo in modo da sveltire i calcoli per N molto grandi (verrà ereditata anche dai suoi thread)
    unsigned int *Buff1 = (unsigned int*)malloc(sizeof(unsigned int)*blockSize); //Primo buffer usato per contenere i numeri primi trovati dai calcolatori, allocato dinamicamente
    unsigned int *Buff2 = (unsigned int*)malloc(sizeof(unsigned int)*blockSize); //Secondo buffer, identico al primo
    buffer = &Buff1; //La variabile globale a cui i thread accederanno in modo sincronizzato si aggancia al primo dei due buffer
    radio = &Buff2; //Il puntatore globale "radio" si aggancia invece all'altro buffer
    (*buffer)[0] = 1; //I primi tre interi sono primi, quindi vengono scritti sul buffer fin da subito
    (*buffer)[1] = 2;
    (*buffer)[2] = 3;
    indice = 3;
    sem_init(&bufferPieno, 0, 0); //Inizialmente il buffer è vuoto quindi questo semaforo viene inizializzato a 0 e l'altro a 1
    sem_init(&trasmettitorePronto, 0, 1);
    if ((fd = open(argv[2], O_WRONLY))==-1)
        error("Si è verificato un errore nel main di Calcolatori mentre tentava di aprire la pipe in scrittura.\n")
    pthread_t arrThread[5]; //Array usato dal main per invocare 4 thread calcolatori più un singolo thread trasmettitore
    printf("Il programma Calcolatori inizierà ora il suo lavoro.\n");
    if (pthread_create(&arrThread[0], NULL, (void*)trasmettitore, NULL)!=0) //Per primo viene avviato il trasmettitore.
            error("Si è verificato un errore nel main di Calcolatori mentre tentava di creare un thread.\n");
    for (int z = 1; z<5;z++) //Vengono poi invocati i calcolatori. Il mio programma ne prevede quattro invece che tre perché nei test che ho condotto sono risultati più efficienti.
        if (pthread_create(&arrThread[z], NULL, (void*)calcolatore, NULL)!=0)
            error("Si è verificato un errore nel main di Calcolatori mentre tentava di creare un thread.\n");
    for (int z = 1; z<5; z++) //Il main si mette in attesa aspettando che i calcolatori finiscano il loro lavoro.
        pthread_join(arrThread[z], NULL);
    (*buffer)[blockSize-1] = 4; //Terminato il lavoro dei Calcolatori il main predispone l'invio di un ultimo blocco per trasmettere gli ultimi dati rimasti e anche per segnalare al ricevente che questi dati saranno gli ultimi. Il codice usato per segnalare è "4" perché è un valore non-primo.
    (*buffer)[blockSize-2] = indice; //L'ultima trasmissione contiene anche un indice salvato nella penultima posizione dell'array. Questo indice rappresenta la posizione in cui finiscono i dati validi (giacché l'ultimo blocco sarà pieno solo in parte)
    sem_wait(&trasmettitorePronto);
    unsigned int **temp = buffer; //Similmente a come avrebbe fatto un qualsiasi calcolatore, il main procede a inviare al trasmettitore l'ultimo blocco da trasmettere
    buffer = radio;
    radio = temp;
    flag = 0; //Subito prima di svegliare il trasmettitore, si setta a 0 un flag che ne determinerà lo spegnimento subito dopo la trasmissione finale.
    sem_post(&bufferPieno);
    pthread_join(arrThread[0], NULL); //Il main attenderà che anche il trasmettitore abbia terminato prima di procedere con le ultime operazioni di chiusura
    pthread_mutex_destroy(&sezCritB); //Operazioni di ripulitura finali
    pthread_mutex_destroy(&sezCritT);
    sem_destroy(&bufferPieno);
    sem_destroy(&trasmettitorePronto);
    free(*buffer);
    free(*radio);
    close(fd); 
    printf("Il programma Calcolatori ha concluso il suo lavoro.\n");
    return 0;
}


void calcolatore(void* arg){
    unsigned int threadNumber;
    pthread_mutex_lock(&sezCritT);
    threadNumber = ++activeSubThreads; //Il thread procede a inizializzare il suo identificativo
    pthread_mutex_unlock(&sezCritT);
    printf("Il Calcolatore numero %u è attivo.\n", threadNumber);
    unsigned int nextInt; //Variabile che tiene traccia del prossimo intero da analizzare
    switch(threadNumber) //Il primo intero su cui lavora ogni thread viene opportunamente inizializzato per poter suddividere il carico di lavoro nel modo migliore possibile
    {
        case 1:
			nextInt = 5;
			break;
        case 2:
			nextInt = 7;
			break;
        case 3:
			nextInt = 11;
			break;
        case 4:
			nextInt = 13;
			break;
        default:
			break;
    }
    do{
        if (nextInt<=N){//Se il valore di nextInt è ancora un valore valido il calcolatore procede con il lavoro
            if (primalTest(nextInt)==1){ //Se il prossimo numero su cui lavora il thread è un numero primo, viene messo nel buffer di appoggio
                pthread_mutex_lock(&sezCritB); //Solo un thread alla volta può scrivere sul buffer
                (*buffer)[indice] = nextInt;
                indice++;
                if (indice == blockSize - 2){ //Se al termine della scrittura il blocco risulta pieno si avvia il post processing che precede la trasmissione
                    sem_wait(&trasmettitorePronto); //Il post processing può avere inizio solo se il trasmettitore è libero
                    (*buffer)[blockSize-1] = 0; //L'ultima cella viene posta a 0 per segnalare al ricevente che questo non sarà l'ultimo blocco. La penultima cella non può mai essere usata da nessuno perché giocherà un ruolo ben preciso durante l'ultima trasmissione.
                    indice = 0; //L'indice del blocco viene riportato a zero per permetterne la sovrascrittura con un nuovo blocco
                    unsigned int **temp = buffer; //Il buffer pieno viene scambiato con quello vuoto, così i calcolatori possono continuare a scrivere.
                    buffer = radio;
                    radio = temp;
                    sem_post(&bufferPieno); //Terminato il post processing, il thread sveglia il trasmettitore
                }
                pthread_mutex_unlock(&sezCritB);
            }
        }
        else
            break; //Appena si incontra il primo valore non valido, il lavoro del calcolatore termina.
        nextInt = nextInt + 12; //Ogni thread ottiene l'intero successivo su cui lavorare avanzando di 12. In questo modo i thread non hanno bisogno di effettuare accessi sincronizzati per ottenere il prossimo intero, velocizzando i tempi
    }while(1);
    printf("Il Calcolatore numero %u ha terminato il suo lavoro.\n", threadNumber);
    pthread_exit(NULL); //Terminato il lavoro del calcolatore, il thread viene chiuso con pthread_exit
}

void trasmettitore(void* arg){ //La trasmissione è affidata a un thread a parte
    do{ //Il ciclo con cui lavora il trasmettitore è impostato su un flag, ma la prima operazione che esegue il trasmettitore al suo interno è una wait su semaforo che eviterà sprechi di prestazione
        sem_wait(&bufferPieno);
        if (write(fd, (char*)(*radio), blockSize*sizeof(unsigned int))!=blockSize*sizeof(unsigned int)) //Grazie al casting di tipo a (char*) è possibile trasmettere direttamente il buffer pieno
            error("Si è verificato un errore mentre il trasmettitore provava a inviare un blocco.\n");
        sem_post(&trasmettitorePronto);
    }while(flag); //Quando questo flag passa da 1 a 0, il trasmettitore viene spento
    pthread_exit(NULL); //Terminato il lavoro, il thread viene chiuso con pthread_exit
}


int primalTest (unsigned int n){ //Test semplificato del 6k+-1, è escluso in partenza il caso per cui N può essere multiplo di 2 o 3 per via della particolare strategia con cui i tre thread scelgono l'intero successivo con cui lavorare 
    unsigned int i = 5;
    while (i*i<=n){
        if ((n%i==0)||(n%(i+2)==0))
            return 0;
            i = i + 6;
        }
    return 1;
    }
