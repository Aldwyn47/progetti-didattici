#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

/*ELENCO VARIABILI STATICHE*/
static struct Scavatrice HarvesterA = {NULL, 0, 0, operativa};              //Scavatrice Arvais.
static struct Scavatrice HarvesterH = {NULL, 0, 0, operativa};              //Scavatrice Hartornen.
static int Turn_Counter = 0;                                                //Contatore dei turni.
static int Encounter_Prob = 0;                                              //Contatore della probabilità di incontro.
static int Victory_flag = 0;                                                //Variabile che identifica l'esito della partita.
static struct Caverna *FirstArvais = NULL;                                  //Puntatore a inizio lista per il cunicolo Arvais.
static struct Caverna *LastArvais = NULL;                                   //Puntatore a fine lista per il cunicolo Arvais.
static struct Caverna *FirstHartornen = NULL;                               //Puntatore a inizio lista per il cunicolo Hartornen.
static struct Caverna *LastHartornen = NULL;                                //Puntatore a fine lista per il cunicolo Hartornen.
static int Map_state = 0;                                                   //Indicatore binario sull'esistenza o meno di una mappa in memoria.

/*ELENCO FUNZIONI STATICHE: accanto a ognuna è riportata anche la linea di codice dove risiede la sua definizione */
static void Ins_Caverna(struct Caverna **First, struct Caverna **Last);     //219
static void Canc_Caverna(struct Caverna **First, struct Caverna **Last);    //265
static void Stampa_Cunicoli(struct Caverna *First, struct Caverna *Last);   //295
static int Chiudi_Cunicoli();                                               //342
static int Avanza(struct Scavatrice *Harv);                                 //373
static int Abbatti (struct Scavatrice *Harv);                               //442
static int Aggira(struct Scavatrice *Harv);                                 //510
static int Esci (struct Scavatrice *Harv);                                  //545
static void Cave_Initializer(struct Caverna *Nborn);                        //563 Questa funzione inizializza alcuni campi di una caverna.
static void Exit_Initializer (struct Caverna *New);                         //585 Questa funzione inizializza alcuni campi di una caverna.
static void Print_Fields(struct Caverna *Cave);                             //616 Questa funzione stampa le caratteristiche di una caverna.
static void Print_Harv(struct Scavatrice Harv);                             //622 Questa funzione stampa lo status di una scavatrice alla fine di un turno.
static void Turn_Cycle(struct Scavatrice *Harv);                            //640 Questa funzione gestisce i processi che hanno luogo durante un turno regolare.
static void End_Cycle();                                                    //671 Questa funzione illustra la situazione alla fine di un turno regolare.
static void Scontro_Finale();                                               //713 Questa funzione dà inizio allo scontro finale tra le due scavatrici.
static void Battle_Turn (struct Scavatrice *Harv, struct Scavatrice *Enemy);//771 Questa funzione gestisce i processi che hanno luogo durante un turno dello scontro.
static void Battle_Report();                                                //817 Questa funzione illustra la situazione alla fine di un turno dello scontro.
static int Scelta_Casata();                                                 //857 Questa funzione viene usata per determinare su quale casata devono agire alcuni dei comandi di Crea_Cunicoli.
static struct Caverna *NextStep(struct Caverna *Cave);                      //871 Questa funzione viene usata per cercare un collegamento successivo all'interno di una caverna qualsiasi.
static void Cave_Evaluation (struct Scavatrice *Harv);                      //882 Questa funzione viene usata per far subire a una scavatrice gli effetti della caverna in cui si trova. 
static struct Caverna *Dept_Charge (struct Caverna *Node);                  //933 Questa funzione viene usata per smantellare la mappa.


void Crea_Cunicoli(){
    printf("\033[1;32m");
    printf("\n+----------------------------------------------------------------------------+\n");
    printf("|                           BENVENUTO NEL MAP EDITOR!                        |\n");
    printf("+----------------------------------------------------------------------------+\n");
    printf("\033[0;32m");
    printf("\nQui è possibile creare una nuova mappa per giocare a \"Sabbie!\". Si ricorda che prima di iniziare una partita è necessario creare almeno 10 caverne per gli Arvais e 10 caverne per gli Hartornen.");
    while (getchar() != '\n');
    if (Map_state != 0){ //Se la variabile globale preposta allo scopo registra la presenza di un'altra mappa in memoria, essa viene deallocata prima di procedere alla creazione di una nuova
        printf ("\nE' stata rilevata la presenza di un'altra mappa in memoria. Purtroppo può essere salvata solo una mappa per volta, pertanto quella precedente verrà distrutta per consentire la creazione di una mappa nuova.");
        while (getchar() != '\n');
        FirstArvais = Dept_Charge(FirstArvais); //Al termine dell'operazione ai puntatori di inizio lista verrà restituito il valore NULL dalla funzione Depth_Charge
        FirstHartornen = Dept_Charge(FirstHartornen);
        LastArvais = NULL; //I puntatori di fine lista vengono invece riportati a NULL manualmente
        LastHartornen = NULL;
        Map_state = 0; //Siccome la mappa è stata cancellata, l'indicatore binario che ne registra la presenza o meno viene settato a 0
        printf ("\nLa vecchia mappa è stata smantellata con successo. Puoi ora procedere alla creazione di una nuova mappa.");
        while (getchar() != '\n');
    }
    int scelta = 0; //Variabile usata per registrare l'input di scelta dell'utente
    do  {
        printf ("\nInserisci un comando per decidere cosa fare:\n-1 per creare una nuova caverna.\n-2 per cancellare l'ultima caverna creata.\n-3 per visualizzare le caverne esistenti in uno dei cunicoli.\n-4 per uscire dal Map Editor.\n");
        scanf ("%d",&scelta);
        while (getchar() != '\n');
        switch (scelta)
        {
            case 1:
                if (Scelta_Casata()) //Ognuno dei comandi del menù eccetto quello di uscita dal Map Editor può essere applicato alla squadra Arvais o alla squadra Hartornen. La scelta è determinata usando la funzione Scelta_Casata
                    Ins_Caverna(&FirstHartornen,&LastHartornen);
                else
                    Ins_Caverna(&FirstArvais,&LastArvais);
                break;
            case 2:
                if (Scelta_Casata())
                    Canc_Caverna(&FirstHartornen,&LastHartornen);
                else
                    Canc_Caverna(&FirstArvais,&LastArvais);
                break;
            case 3:
                if (Scelta_Casata())
                    Stampa_Cunicoli(FirstHartornen,LastHartornen);
                else
                    Stampa_Cunicoli(FirstArvais,LastArvais);
                break;
            case 4:
                if (Chiudi_Cunicoli())
                    return; //La chiamata di Chiudi_Cunicoli può avere esito positivo o negativo. Il return di uscita dal Map Editor avrà luogo solo se Chiudi_Cunicoli dà l'OK
                break;
            default:
                printf("\nIl valore inserito non corrisponde a un comando valido, si prega di inserire un valore compreso tra 1 e 4.\n");
                break;
        }
    } while (1); //Il loop del menù del Map Editor è stato impostato per essere infinito: si può uscire da Crea_Cunicoli solo se si chiama la funzione Chiudi_Cunicoli e questa dà l'OK ad uscire.
}


void Gioca(){
    if (Map_state == 0){ //Se si prova a iniziare una partita senza prima aver creato una mappa questo If lo scopre controllando il valore della variabile binaria associata alla mappa e riporta immediatamente indietro il giocatore.
        printf("\nPrima di giocare è necessario creare una mappa usando il map editor!\nVerrai ora riportato al menù principale.");
        while (getchar() != '\n');
        return;
    }
    HarvesterA.posizione = FirstArvais; //I campi delle scavatrici e le variabili che registrano percentuali di probabilità ed eventuale vincitore vengono reinizializzati ad ogni inizio di partita
    HarvesterA.SerbatoioE = 4;
    HarvesterA.SerbatoioM = 0;
    HarvesterA.status = operativa;
    HarvesterH.posizione = FirstHartornen;
    HarvesterH.SerbatoioE = 4;
    HarvesterH.SerbatoioM = 0;
    HarvesterH.status = operativa;
    Turn_Counter = 5;
    Encounter_Prob = 3;
    Victory_flag = VNowin;
    printf("\033[1;33m");
    printf("\n+----------------------------------------------------------------------------+\n");
    printf("|                              INIZIA LA SFIDA!!!                            |\n");
    printf("+----------------------------------------------------------------------------+");
    while (getchar() != '\n');
    printf("\033[0;31m\n"); //All'interno di ogni turno il colore dell'output viene cambiato per rendere più chiaro quale squadra stia per fare la sua mossa.
    printf("+---------------------------------------+\n");
    printf("|             TURNO HARTORNEN           |\n");
    printf("+---------------------------------------+");
    while (getchar() != '\n');
    printf("\nLa scavatrice Hartornen si addentra per prima nel suo cunicolo, esplorandone la caverna di entrata!");
    while (getchar() != '\n');
    Cave_Evaluation (&HarvesterH); //Durante il primo turno il giocatore non ha alcuna scelta: può solo assistere mentre la sua scavatrice raggiunge la caverna di entrata e ne subisce gli effetti.
    printf("\033[0;34m");
    printf("\n");
    printf("+---------------------------------------+\n");
    printf("|               TURNO ARVAIS            |\n");
    printf("+---------------------------------------+");
    while (getchar() != '\n');
    printf("\nLa scavatrice Arvais si muove a sua volta, raggiungendo anch'essa l'entrata del suo cunicolo!");
    while (getchar() != '\n');
    Cave_Evaluation (&HarvesterA);
    End_Cycle(); //Alla fine del primo turno viene stampato un resoconto della situazione
    while (Victory_flag==VNowin) { //I turni successivi avranno luogo solo fintanto che non è stato ancora determinato un vincitore
        if ((rand()%100)+1>Encounter_Prob){ //Lo svolgimento normale di un turno ha luogo solo se le scavatrici non si incontrano. Ciò è determinato tramite il confronto tra un numero casuale tra 1 e 100 e l'indicatore di probabilità di incontro, che cambia alla fine di ogni turno
            printf("\033[0;31m\n");
            printf("+---------------------------------------+\n");
            printf("|             TURNO HARTORNEN           |\n");
            printf("+---------------------------------------+");
            while (getchar() != '\n');
            if (HarvesterH.status!=libera) //Ogni scavatrice gioca il suo turno solo se non ha usato la funzione Esci per uscire dal cunicolo. In caso contrario, la scavatrice salta il suo turno e viene stampato un messaggio che ricorda al giocatore che la sua scavatrice è fuori.
                Turn_Cycle(&HarvesterH);   //I processi che compongono il turno di un giocatore sono stati incapsulati nella funzione Turn_Cycle, che viene chiamata ogni volta che un giocatore deve scegliere la sua mossa
            else{
                printf ("\nLa scavatrice Hartornen ha concluso la partita: essa è fuori dal cunicolo con un bottino di %d unità di melassa.\n", HarvesterH.SerbatoioM);
                while (getchar() != '\n');
            }
            printf("\033[0;34m\n"); //Ogni turno presenta un'alternanza in cui prima giocano gli Hartornen e poi gli Arvais. Quanto accade al primo giocatore viene replicato per intero per il secondo.
            printf("+---------------------------------------+\n");
            printf("|               TURNO ARVAIS            |\n");
            printf("+---------------------------------------+");
            while (getchar() != '\n');
            if (HarvesterA.status!=libera) {
                Turn_Cycle(&HarvesterA);
            }
            else{
                printf ("\nLa scavatrice Arvais ha concluso la partita: essa è fuori dal cunicolo con un bottino di %d unità di melassa.\n", HarvesterA.SerbatoioM);
                while (getchar() != '\n');
            }
        End_Cycle();
        }
        else //Se le scavatrici si incontrano, lo scorrimento dei turni regolari viene interrotto e viene lanciata la funzione Scontro_Finale per gestire la battaglia
            Scontro_Finale();
    } //Il ciclo principale di gioco ha termine quando i turni regolari o la battaglia finale riescono a determinare un vincitore
    printf("\n");
    printf("\033[1;33m");
    if (Victory_flag == VPareggio){ //A seconda dello stato in cui è stata impostata la variabile che registra il vincitore, viene stampato un messaggio di conclusione diverso.
            printf("+----------------------------------------------------------------------------+\n");
            printf("|                   LO SCONTRO E' FINITO CON UN PAREGGIO!                    |\n");
            printf("+----------------------------------------------------------------------------+\n");
    }
    else if (Victory_flag == VHartornen){
            printf("+----------------------------------------------------------------------------+\n");
            printf("|                LO SCONTRO E' FINITO! VINCONO GLI HARTORNEN!                |\n");
            printf("+----------------------------------------------------------------------------+\n");
    }
    else if (Victory_flag == VArvais){
            printf("+----------------------------------------------------------------------------+\n");
            printf("|                  LO SCONTRO E' FINITO! VINCONO GLI ARVAIS!                 |\n");
            printf("+----------------------------------------------------------------------------+\n");
    }
    printf("\nVerrai ora riportato al menù principale.");
    while (getchar() != '\n');
    printf("\033[0m");
    return;
}


void Termina_Gioco(){
    printf("\033[0;36m");
    if (Map_state != 0){ //Se la variabile binaria di pertinenza registra la presenza di una mappa in memoria, viene avviata una procedura di smantellamento prima di uscire dal gioco
        printf ("\nE' stata rilevata la presenza di una mappa salvata in memoria.\nSi procederà al suo smantellamento prima di uscire dal gioco.");
        while (getchar() != '\n');
        FirstArvais = Dept_Charge(FirstArvais);
        FirstHartornen = Dept_Charge(FirstHartornen);
        LastArvais = NULL;
        LastHartornen = NULL;
        printf("\nLa mappa è stata smantellata con successo.");
        while (getchar() != '\n');
    }
    printf("\nGrazie per aver giocato a \"Sabbie!\". Spero di rivederti presto!\n\n");
    printf("\033[1;36m");
    printf("+----------------------------------------------------------------------------+\n");    
    printf("|            _____                         ____                   _          |\n");
    printf("|           / ____|                       / __ \\                 | |         |\n");
    printf("|          | |  __  __ _ _ __ ___   ___  | |  | |_   _____ _ __  | |         |\n");
    printf("|          | | |_ |/ _` | '_ ` _ \\ / _ \\ | |  | \\ \\ / / _ \\ '__| | |         |\n");
    printf("|          | |__| | (_| | | | | | |  __/ | |__| |\\ V /  __/ |    |_|         |\n");
    printf("|           \\_____|\\__,_|_| |_| |_|\\___|  \\____/  \\_/ \\___|_|    (_)         |\n");
    printf("+----------------------------------------------------------------------------+");
    while (getchar() != '\n');
    return;
}


static void Ins_Caverna(struct Caverna **First, struct Caverna **Last){ //La funzione viene chiamata dal map editor per inserire una nuova caverna all'interno di uno dei cunicoli. Il cunicolo viene scelto sulla base dei riferimenti che vengono passati come argomenti
    struct Caverna * New = (struct Caverna*) malloc (sizeof(struct Caverna)); //Viene allocata la memoria per il nuovo nodo della lista e assegnata a un riferimento
    Cave_Initializer (New); //La funzione Cave_Initializer si occupa di inizializzare alcuni dei campi della nuova caverna
    int token = 3;
    do { //Questo loop assicura che vengano inseriti solo valori validi per l'inizializzazione del campo "stato" della caverna, unico da impostare manualmente in fase di creazione della mappa
        printf ("E' stata richiesta la creazione di una nuova caverna, si prega di specificarne il tipo:\n-Inserisci 0 per normale.\n-Inserisci 1 per speciale.\n-Inserisci 2 per accidentata.\n");
        scanf ("%d", &token);
        while (getchar() != '\n');
        printf("\n");
        if ((token >= 0) && (token <= 2))
            (*New).stato = token;
        else
            printf ("E' stato inserito un valore non valido, si prega di riprovare.\n\n");
    } while ((token > 2)||(token < 0));
    if (*Last == NULL){ //Se non erano presenti caverne nel cunicolo, la nuova diventa automaticamente la prima e i puntatori di inizio e fine lista vengono agganciati alla nuova caverna.
        printf ("Il cunicolo associato a questa casata era privo di caverne: la caverna appena creata è diventata automaticamente l'entrata del cunicolo.\n");
        *First = New;
        *Last = New;
    }
    else { //Se erano presenti caverne nel cunicolo, è il giocatore a dover scegliere dove creare il collegamento con le precedenti. Solo il puntatore di fine lista verrà agganciato alla nuova caverna.
        token = 3;
        do {//Questo loop assicura che vengano inseriti solo valori validi
            printf("Il cunicolo aveva già delle caverne, specificare in che direzione deve essere piazzata la prossima.\n-Inserisci 0 per sinistra.\n-Inserisci 1 per avanti.\n-Inserisci 2 per destra.\n");
            scanf ("%d", &token);
            while (getchar() != '\n');
            if (token == 0){
                (*Last)->NextSinistra = New;
                *Last = New;
            }
            else if (token == 1) {
                (*Last)->NextAvanti = New;
                *Last = New;
            }
            else if (token == 2) {
                (*Last)->NextDestra = New;
                *Last = New;
            }
            else 
                printf ("\nIl valore inserito non è valido, si prega di riprovare.\n\n"); 
        } while ((token > 2)||(token < 0));
        
    }
    return;
}


static void Canc_Caverna(struct Caverna **First, struct Caverna **Last){ //La funzione elimina l'ultima caverna inserita in uno dei due cunicoli. La scelta del cunicolo è determinata sulla base di quale riferimenti vengono passati come argomenti
    if ((*First) == NULL) //La cancellazione avrà luogo solo se nel cunicolo ci sono effettivamente caverne, condizione rappresentata dal fatto che il puntatore di inizio lista punta effettivamente a qualcosa
        printf("Nel cunicolo non ci sono caverne da eliminare.\n");
    else{
        if (*First == *Last){ //Se nel cunicolo è presente una sola caverna basta deallocarla e settare i puntatori di inizio e fine lista a NULL. Lo si scopre controllando se il puntatore di inizio e fine lista puntano alla stessa caverna
            free(*First);
            *First = NULL; //Dopo la cancellazione il cunicolo è vuoto, quindi entrambi i puntatori vengono riportati a NULL
            *Last = NULL;
        }
        else { //Se nel cunicolo sono presenti più caverne, bisogna scorrere la lista
            struct Caverna * scan = *First; //Variabile usata per scorrere la lista
            struct Caverna * newLast = NULL; //Variabile usata per ricordare quale caverna dovrà diventare l'ultima dopo la cancellazione
            while (scan != *Last) { //Lo scorrimento andrà avanti finché non si arriva all'ultima caverna
                newLast = scan; //Prima di far fare un passo successivo a scan, viene salvata la sua posizione in newLast. Quando scan arriverà all'ultimo nodo, newLast avrà ancora l'informazione del penultimo.
                scan = NextStep(scan); //Se il ciclo ha avuto luogo significa che scan non era arrivata alla fine, quindi le si fa fare un passo avanti
            }
            free(scan); //L'ultima caverna viene deallocata
            newLast->NextSinistra = NULL; //I collegamenti della penultima vengono settati su NULL, dato che adesso è l'ultima
            newLast->NextAvanti = NULL;
            newLast->NextDestra = NULL;
            *Last = newLast; //La penultima diventa ultima
        }
    printf("La caverna più in profondità del cunicolo è stata eliminata con successo.\n"); //Dopo l'eliminazione viene stampato un messaggio per confermare il successo della procedura
    if (*First==NULL)
        printf("Nel cunicolo ora non ci sono più caverne.\n");  //Il messaggio viene integrato con una seconda parte se il cunicolo ora è vuoto
    }
    return;
}


static void Stampa_Cunicoli(struct Caverna *First, struct Caverna *Last){ //La funzione viene usata per stampare in ordine tutte le caverne che compongono un cunicolo. Il cunicolo da stampare viene determinato sulla base dei riferimenti passati come argomenti
    printf("+----------------------------------------------------------------------------+\n"); //Viene stampato anche un elemento grafico che fa capire chiaramente l'inizio della lista
    printf("|                                ELENCO CAVERNE:                             |\n");
    printf("+----------------------------------------------------------------------------+\n");
    if (First == NULL) //Se il cunicolo non ha caverne viene stampato un messaggio per riferirlo
        printf ("\nIl cunicolo al momento non contiene alcuna caverna.\n");
    else{
        printf ("\nIl cunicolo al momento contiene le seguenti caverne:\n"); //Se il cunicolo ha caverne viene stampato per prima cosa un messaggio di apertura della lista
        struct Caverna * scan = First; //Riferimento che viene usato per scorrere la lista
        int counter = 1; //Il contatore verrà usato per associare a ogni caverna un numero che ne esprima la posizione nel cunicolo 
        if (scan==Last){ //Se la caverna è una sola, i messaggi sono leggermente diversi
            printf ("\nCAVERNA N.1, prima e ultima del cunicolo.\n");
            Print_Fields(scan); //Print_Fields viene usata per automatizzare quelle parti del processo di stampa che rimangono uguali da caverna a caverna
            printf ("Questa è l'unica caverna del cunicolo e non ha ulteriori collegamenti.\n");
        }
        else { //Se c'è più di una caverna, ne vengono stampate le caratteristiche in ordine
            while (scan != Last) {
                printf ("\nCAVERNA N. %d\n", counter);
                Print_Fields(scan);
                counter++; //Il contatore sale, per tenere traccia dell'ordine delle caverne
                if (scan->NextSinistra != NULL){ //Il riferimento che scorre la lista avanza o meno in una direzione sulla base del se esiste effettivamente un collegamento o meno. Se esiste, viene stampato e il riferimento lo segue subito dopo.
                    printf ("Il suo collegamento con la successiva è a sinistra, all'indirizzo %p.\n", scan->NextSinistra);
                    scan = scan->NextSinistra;
                }
                else if (scan->NextAvanti != NULL){
                    printf ("Il suo collegamento con la successiva è avanti, all'indirizzo %p.\n", scan->NextAvanti);
                    scan = scan->NextAvanti;
                }
                else if (scan->NextDestra != NULL){
                    printf ("Il suo collegamento con la successiva è a destra, all'indirizzo %p.\n", scan->NextDestra);
                    scan = scan->NextDestra;
                }
            };
            printf ("\nCAVERNA N. %d\n", counter); //Il ciclo termina non appena il riferimento di scorrimento raggiunge il riferimento di fine lista. Questo significa che l'ultima caverna non viene stampata nel ciclo e ha bisogno della sua stampa a parte
            Print_Fields(scan);
            printf ("La caverna è l'ultima del cunicolo e non ha ulteriori collegamenti.\n"); 
            if (counter >=10) //Se il numero di caverne è almeno 10, viene stampato anche un messaggio che ricorda al giocatore che il cunicolo ha raggiunto il numero minimo di caverne necessarie a giocare
                printf ("\nQuesto cunicolo ha raggiunto la quantità minima di caverne necessaria a iniziare una partita.\n");
        }
    }
    printf("\n+----------------------------------------------------------------------------+\n"); //Viene stampato anche un elemento grafico che fa capire chiaramente la fine della lista
    printf("|                                                                            |\n");
    printf("+----------------------------------------------------------------------------+\n");
    return;
}


static int Chiudi_Cunicoli (){ //La funzione viene usata per chiedere l'uscita dal Map Editor. La sua chiamata può restituire uno o zero a seconda del numero di caverne contate, che può essere sufficiente o meno per l'inizio di una partita
    int counterArv = 1, counterHart = 1; //Ogni cunicolo ha il suo contatore
    struct Caverna * scan = FirstArvais; //Riferimento usato per scorrere le liste
    while (scan!=LastArvais){ 
        counterArv++; //A ogni passo successivo dello scorrimento di un cunicolo, il relativo contatore aumenta
        scan = NextStep(scan);
    }
    scan = FirstHartornen;
    while (scan!=LastHartornen){
        counterHart++;
        scan = NextStep(scan);
    }
    if (FirstHartornen == NULL) //Ogni contatore viene inizializzato ad uno perché se il loop di scorrimento ha luogo significa che c'è almeno una caverna. Se però il loop viene saltato perché il cunicolo è vuoto, essi vanno impostati a zero.
        counterHart = 0;
    if (FirstArvais == NULL)
        counterArv = 0;
    if ((counterArv>=10)&&(counterHart>=10)){ //Se il numero di caverne risulta adeguato per l'inizio di una partita viene stampato un messaggio che avvisa il giocatore e restituito il valore 1. Altrimenti viene stampato un messaggio diverso e restituito 0.
        printf ("\nCongratulazioni, il numero di caverne presente nei cunicoli è sufficiente: è possibile giocare.\nVerrai ora riportato al menù principale.");
        while (getchar() != '\n');
        Map_state = 1; //Se il numero di cunicoli è sufficiente prima di restituire un valore che autorizza l'uscita dal Map Editor viene anche impostato a 1 l'indicatore binario che registra la presenza di una mappa in memoria
        printf("\033[0m");
        return 1;
    }
    else {
        printf ("\nSono state contate %d caverne per gli Arvais e %d caverne per gli Hartornen.\nIl numero di caverne create non risulta sufficiente per l'inizio di una partita.\nSi prega di creare almeno 10 caverne per gli Arvais e almeno 10 per gli Hartornen prima di uscire dal Map Editor.", counterArv, counterHart);
        while (getchar() != '\n');
        return 0;
    }
}


static int Avanza(struct Scavatrice *Harv){ //La funzione costituisce una delle opzioni possibili all'interno di un turno regolare. Il riferimento passato come argomento dipende da quale giocatore stava giocando il suo turno. La funzione restituisce 0 se l'azione ha avuto luogo correttamente, 1 se qualcosa rendeva impossibile la sua esecuzione.
    if (Harv->status == bloccata){ //Se l'opzione viene scelta dopo che la scavatrice era rimasta intrappolata in un crollo, essa non viene eseguita e viene restituito il valore 1
        printf("La scavatrice è intrappolata nelle macerie e non può avanzare!\nDeve obbligatoriamente cercare di aggirarle!\nScegli un'altra mossa tra le opzioni disponibili.\n");
        return 1;
    }
    if (NextStep(Harv->posizione) == Harv->posizione){ //Se l'opzione viene scelta in una caverna dove sono assenti collegamenti che permettono di avanzare l'azione non viene svolta e viene restituito il valore 1
        printf ("La scavatrice ha raggiunto la massima profondità e non può più avanzare!\nDovrà aprirsi un varco abbattendo le pareti di roccia o cercare di uscire dal cunicolo!\nScegli un'altra mossa tra le opzioni disponibili.\n");
        return 1;
    }
    if (((rand()%100)+1)>25) { //Ogni volta che si prova ad avanzare c'è una probabilità del 25% che si verifichi un crollo. L'avanzamento ha luogo solo se viene estratto casualmente un numero tra 1 e 100 che sia maggiore di 25.
        if ((Harv->posizione)->NextAvanti != NULL) //Sulla base di quali collegamenti della caverna sono aperti vengono stampati da uno a tre messaggi per specificare il possibile avanzamento in tale direzione
            printf ("E' possibile avanzare verso avanti.\n");
        if ((Harv->posizione)->NextSinistra != NULL)
            printf ("E' possibile avanzare verso sinistra.\n");
        if ((Harv->posizione)->NextDestra != NULL)
            printf ("E' possibile avanzare verso destra.\n");
        int direzione = 3; //Variabile usata per registrare il successivo input di scelta del giocatore
        do {
            printf ("\nSpecificare in che direzione si intende avanzare:\n-Inserire 0 per sinistra.\n-Inserire 1 per avanti.\n-Inserire 2 per destra.\n");
            scanf ("%d", &direzione);
            while (getchar() != '\n');
            printf ("\n");
            switch (direzione)
            {
                case 0:
                    if ((Harv->posizione)->NextSinistra == NULL){ //Ogni valore potrebbe essere o meno un valore valido sulla base delle caratteristiche della caverna in cui viene invocata la funzione. Se il valore non è valido, la variabile di scelta viene reimpostata a un valore che causa la ripetizione del loop e il giocatore viene avvertito che la scelta non è valida.
                        printf ("Al valore specificato non corrisponde una direzione valida per l'avanzamento. Si prega di riprovare.\n");
                        direzione = 3;
                    }
                    else 
                        Harv->posizione = (Harv->posizione)->NextSinistra; //Se la direzione scelta è valida, la posizione della scavatrice viene aggiornata
                    break;
                case 1:
                    if ((Harv->posizione)->NextAvanti == NULL){
                        printf ("Al valore specificato non corrisponde una direzione valida per l'avanzamento. Si prega di riprovare.\n");
                        direzione = 3;
                    }
                    else 
                        Harv->posizione = (Harv->posizione)->NextAvanti;
                    break;
                case 2:
                    if ((Harv->posizione)->NextDestra == NULL){
                        printf ("Al valore specificato non corrisponde una direzione valida per l'avanzamento. Si prega di riprovare.\n");
                        direzione = 3;
                    }
                    else 
                        Harv->posizione = (Harv->posizione)->NextDestra;
                    break;
                default:
                    printf ("Al valore specificato non corrisponde una direzione valida per l'avanzamento. Si prega di riprovare.\n");
                    break;
            }
        } while ((direzione < 0) || (direzione > 2)); //Questo loop assicura che vengano inseriti solo valori validi
        printf("La scavatrice ha seguito il percorso indicato, addentrandosi nella nuova caverna!"); //Dopo che è stata selezionata una direzione valida per l'avanzamento viene stampato un messaggio che esprime il successo dell'azione
        while (getchar() != '\n');
        Cave_Evaluation(Harv); //La scavatrice subisce immediatamente gli effetti della nuova caverna in cui mette piede
        return 0; //Dopo che l'avanzamento ha avuto successo, la funzione restituisce il valore 0
    }
    else { //Se è stato sorteggiato un valore compreso tra 1 e 25 la scavatrice rimane bloccata a causa di un crollo e l'avanzamento fallisce. Viene stampato un messaggio per avvertire il giocatore e viene restituito il valore 1.
        printf("Un crollo improvviso ha distrutto la galleria che collegava la caverna attuale con le successive!");
        while (getchar() != '\n');
        printf("La scavatrice dovrà farsi strada aggirando le macerie!\nScegli un'altra mossa tra le opzioni disponibili.");
        while (getchar() != '\n');
        Harv->status = bloccata; //Per rappresentare la condizione di blocco viene cambiato il campo status della scavatrice
        return 1;
    }
}


static int Abbatti (struct Scavatrice *Harv){ //La funzione costituisce una delle opzioni possibili all'interno di un turno regolare. Il riferimento passato come argomento dipende da quale giocatore stava giocando il suo turno. La funzione restituisce 0 se l'azione ha avuto luogo correttamente, 1 se qualcosa rendeva impossibile la sua esecuzione.
    if (Harv->status == bloccata){ //Se l'opzione viene scelta dopo che la scavatrice era rimasta intrappolata in un crollo, essa non viene eseguita e viene restituito il valore 1
        printf("La scavatrice è intrappolata nelle macerie e non può avanzare!\nDeve obbligatoriamente cercare di aggirarle.\nScegli un'altra mossa tra le opzioni disponibili.\n");
        return 1;
    }
    if ((((Harv->posizione)->NextSinistra != NULL) && ((Harv->posizione)->NextAvanti != NULL)) && ((Harv->posizione)->NextDestra != NULL)) {
        printf ("Non è possibile abbattere nessuna parete perché la via è aperta in tutte le direzioni.\nScegli un'altra mossa tra le azioni disponibili.\n");
        return 1; //Se l'opzione viene scelta in una caverna dove sono assenti pareti da abbattere perché tutte le direzioni hanno già dei collegamenti l'azione non viene svolta e viene restituito il valore 1
    }
    (Harv->SerbatoioE)--; //Se entrambe le condizioni che possono prevenire il buon esito della mossa non si verificano si procede con il risolverla e per prima cosa viene pagato il costo in energia della stessa
    if ((Harv->posizione)->NextAvanti == NULL) //Sulla base di quante direzioni sono valide per l'abbattimento vengono stampati da uno a tre messaggi per riferirlo al giocatore
        printf ("E' possibile abbattere la parete frontale della caverna.\n");
    if ((Harv->posizione)->NextSinistra == NULL)
        printf ("E' possibile abbattere la parete sinistra della caverna.\n");
    if ((Harv->posizione)->NextDestra == NULL)
        printf ("E' possibile abbattere la parete destra della caverna.\n");
    int direzione = 3; //Questa variabile è usata per registrare il successivo input di scelta del giocatore
    do {
        printf ("\nSpecificare in che direzione si intende scavare:\n-Inserisci 0 per sinistra.\n-Inserisci 1 per avanti.\n-Inserisci 2 per destra.\n");
        scanf ("%d", &direzione);
        while (getchar() != '\n');
        printf ("\n");
        switch (direzione)
        {
            case 0:
                if ((Harv->posizione)->NextSinistra != NULL){ //Ogni valore potrebbe essere o meno un valore valido sulla base delle caratteristiche della caverna in cui viene invocata la funzione. Se il valore non è valido, la variabile di scelta viene reimpostata a un valore che causa la ripetizione del loop e il giocatore viene avvertito che la scelta non è valida.
                    printf ("Al valore specificato non corrisponde una direzione valida per lo sfondamento. Si prega di riprovare.\n");
                    direzione = 3;
                }
                else { //Viceversa se la direzione scelta era valida viene creato un nuovo nodo della lista e viene agganciato al collegamento associato alla direzione scelta
                    struct Caverna * New = (struct Caverna*) malloc (sizeof(struct Caverna));
                    Exit_Initializer (New); //Per inizializzare le caverne create con l'abbattimento viene usata una funzione a parte dedicata solo a questo
                    (Harv->posizione)->NextSinistra = New;
                }
                break;
            case 1:
                if ((Harv->posizione)->NextAvanti != NULL){
                    printf ("Al valore specificato non corrisponde una direzione valida per lo sfondamento. Si prega di riprovare.\n");
                    direzione = 3;
                }
                else {
                    struct Caverna * New = (struct Caverna*) malloc (sizeof(struct Caverna));
                    Exit_Initializer (New);
                    (Harv->posizione)->NextAvanti = New;
                }
                break;
            case 2:
                if ((Harv->posizione)->NextDestra != NULL){
                    printf ("Al valore specificato non corrisponde una direzione valida per lo sfondamento. Si prega di riprovare.\n");
                    direzione = 3;
                }
                else {
                    struct Caverna * New = (struct Caverna*) malloc (sizeof(struct Caverna));
                    Exit_Initializer (New);
                    (Harv->posizione)->NextDestra = New;
                }
                break;
            default:
                printf ("Al valore specificato non corrisponde una direzione valida per lo sfondamento. Si prega di riprovare.\n");
                break;
        }
    } while ((direzione < 0) || (direzione > 2)); //Questo loop assicura che vengano inseriti solo valori validi
    printf("La scavatrice ha abbattuto la parete con successo!"); //Dopo che l'abbattimento ha avuto luogo viene stampato un messaggio che esprime il successo dell'azione e restituito il valore 0
    while (getchar() != '\n');
    return 0;
}


static int Aggira(struct Scavatrice *Harv){ //La funzione costituisce una delle opzioni possibili all'interno di un turno regolare. Il riferimento passato come argomento dipende da quale giocatore stava giocando il suo turno. La funzione restituisce 0 se l'azione ha avuto luogo correttamente, 1 se qualcosa rendeva impossibile la sua esecuzione.
    if (Harv->status != bloccata){ //L'azione può avere luogo solo se lo status della scavatrice è impostato su "bloccata" a seguito di un crollo. In tutti gli altri casi viene stampato un messaggio per avvertire il giocatore e restituito il valore 1
        printf("La strada della scavatrice è libera, non c'è nessun ostacolo da aggirare!\nScegli un'altra mossa tra le opzioni disponibili.\n");
        return 1;
    }
    else {
        struct Caverna * New = (struct Caverna*) malloc (sizeof(struct Caverna)); //Se l'azione ha luogo, per prima cosa viene allocata la memoria per una nuova caverna e le viene assegnato un riferimento
        Cave_Initializer(New); //Alcuni campi vengono inizializzati usando la stessa funzione del map editor, essendo le percentuali di probabilità le stesse.
        New->NextAvanti = NextStep(Harv->posizione); //Il collegamento in avanti della caverna di bypass viene subito agganciato a una caverna scelta tra le successive a quella in cui si è verificato il crollo. L'ordine è determinato dalla funzione NextStep ed è Sinistra > Avanti > Destra.
        if ((New->NextAvanti!=(Harv->posizione)->NextAvanti)&&((Harv->posizione)->NextAvanti!=NULL))
            (Harv->posizione)->NextAvanti = Dept_Charge((Harv->posizione)->NextAvanti); //La linea di codice 525 collegherà la caverna crollata a quella di bypass usando il collegamento in avanti della caverna crollata. Questo costituisce un potenziale problema perché se l'istruzione della linea 518 aveva assegnato alla caverna di bypass un collegamento con una caverna diversa da quella che si trovava avanti alla caverna crollata, il riferimento alla caverna avanti verrà perso quando verrà eseguita la linea di codice 525, causando un memory leak. In quel caso viene quindi preventivamente deallocato ciò a cui puntava il collegamento in avanti della caverna crollata.
        if (New->NextAvanti == (Harv->posizione)->NextDestra) //Per prevenire successivi errori in fase di deallocazione della memoria, in qualsiasi momento ogni caverna deve avere uno e un solo collegamento con la caverna precedente. Se il collegamento destro o quello sinistro della caverna crollata puntano alla stessa caverna a cui punta la caverna di bypass, il collegamento deve essere chiuso. Non è necessario eseguire l'operazione per il collegamento in avanti perché quello viene assegnato alla caverna di bypass alla linea 525.
            (Harv->posizione)->NextDestra = NULL;
        if (New->NextAvanti == (Harv->posizione)->NextSinistra)
            (Harv->posizione)->NextSinistra = NULL;
        (Harv->posizione)->NextAvanti = New; //Una volta completate le procedure di sicurezza che riconfigurano il cunicolo per prevenire errori, il collegamento in avanti della caverna crollata viene agganciato alla caverna di bypass. La configurazione finale sarà CavernaCrollata -> Avanti -> CavernaBypass -> Avanti -> Una delle caverne immediatamente successive alla caverna crollata.
        int token = ((rand()%100)+1); //A differenza delle caverne create nel Map Editor il campo "stato" viene inizializzato casualmente e non impostato manualmente dal giocatore. Ciò viene fatto usando la variabile token
        if (token <= 20)
            (*New).stato = accidentata;
        else if (token <= 60)
            (*New).stato = normale;
        else
            (*New).stato = speciale;
        printf("La scavatrice ha aggirato con successo le macerie, creando una nuova caverna che le permetterà di proseguire nel cunicolo.\nAlcune porzioni del cunicolo originale potrebbero essere crollate in seguito agli scavi.\n\n");//Una volta completata la caverna di bypass viene stampato un messaggio per avvertire il giocatore della sua creazione come della possibilità che alcune delle caverne originali non esistano più.
        Print_Fields(New); //Questa istruzione può essere nascosta usando un commento se si vuole aumentare la suspence. Altrimenti è utile per visualizzare immediatamente le caratteristiche della nuova caverna di bypass.
        Harv->status = operativa; //Lo status della scavatrice viene aggiornato ad "operativa" per simboleggiare il superamento del crollo e permetterle di tornare a svolgere regolarmente i turni successivi.
        printf("\nLa scavatrice si addentrerà ora nella caverna che ha appena creato!");
        while (getchar() != '\n');
        Harv->posizione = (Harv->posizione)->NextAvanti; //La scavatrice si addentra immediatamente nella caverna di bypass: viene stampato un messaggio, la posizione viene aggiornata e gli effetti della caverna hanno luogo tramite la funzione Cave_Evaluation
        Cave_Evaluation(Harv);
        return 0;
    }
}


static int Esci (struct Scavatrice *Harv){ //La funzione costituisce una delle opzioni possibili all'interno di un turno regolare. Il riferimento passato come argomento dipende da quale giocatore stava giocando il suo turno. La funzione restituisce 0 se l'azione ha avuto luogo correttamente, 1 se qualcosa rendeva impossibile la sua esecuzione.
    if (Harv->status == bloccata){ //Se l'opzione viene scelta dopo che la scavatrice era rimasta intrappolata in un crollo, essa non viene eseguita e viene restituito il valore 1
        printf("La scavatrice è intrappolata nelle macerie e non può avanzare!\nDeve obbligatoriamente cercare di aggirarle.\nScegli un'altra mossa tra le opzioni disponibili.\n");
        return 1;
    }
    if ((Harv->posizione)->stato != uscita){//Se l'opzione viene selezionata da una caverna il cui stato non è "uscita", essa non viene eseguita e viene restituito il valore 1
        printf ("Dalla posizione attuale è impossibile scavare una via di fuga.\nScegli un'altra mossa tra le azioni disponibili.\n");
        return 1;
    }
    else {
        printf ("Congratulazioni! La tua scavatrice ce l'ha fatta sana e salva ed è fuori dal cunicolo!\n");
        Harv->status = libera;//Se il tentativo di uscita dal cunicolo ha successo oltre a venir stampato un messaggio che riferisce il successo, lo status della scavatrice viene impostato su "libera" ed essa non giocherà più i turni successivi
        while (getchar() != '\n');
        return 0;
    }
}


static void Cave_Initializer (struct Caverna *Nborn){ //La funzione viene usata per inizializzare automaticamente alcuni campi di una nuova caverna. Viene usata dal Map Editor e dalla funzione Aggira, mentre la funzione Abbatti ne utilizza una simile ma con percentuali di probabilità diverse.
    (*Nborn).NextAvanti = NULL; //I collegamenti della nuova caverna vengono inizializzati a NULL
    (*Nborn).NextDestra = NULL;
    (*Nborn).NextSinistra = NULL;
    int token = ((rand()%100)+1); //La melassa viene inizializzata casualmente con probabilità del 50-30-20
    if (token <= 50)
        (*Nborn).melassa = nessuna;
    else if (token <=80)
        (*Nborn).melassa = poca;
    else
        (*Nborn).melassa = molta;
    token = ((rand()%100)+1); //L'imprevisto viene inizializzato casualmente con probabilità del 50-35-15
    if (token <= 50)
        (*Nborn).imprevisto = nessun_imprevisto;
    else if (token <=85)
        (*Nborn).imprevisto = crollo;
    else
        (*Nborn).imprevisto = baco;
    return;
}


static void Exit_Initializer (struct Caverna *New){ //La funzione viene usata per inizializzare automaticamente alcuni campi di una nuova caverna. Viene usata esclusivamente dalla funzione Abbatti e oltre ad avere percentuali di probabilità diversa per i campi melassa e imprevisto prevede anche la possibilità che la caverna assuma stato uguale a "uscita"
    New->NextAvanti = NULL; //I collegamenti della nuova caverna vengono inizializzati a NULL
    New->NextSinistra = NULL;
    New->NextDestra = NULL;
    int token = ((rand()%100)+1); //La melassa viene inizializzata casualmente con probabilità del 40-40-20
    if (token <= 40)
        (*New).melassa = nessuna;
    else if (token <=80)
        (*New).melassa = poca;
    else
        (*New).melassa = molta;
    token = ((rand()%100)+1); //L'imprevisto viene inizializzato casualmente con probabilità del 40-40-20
    if (token <= 40)
        (*New).imprevisto = nessun_imprevisto;
    else if (token <= 80)
        (*New).imprevisto = crollo;
    else
        (*New).imprevisto = baco;
    token = ((rand()%100)+1); //Lo stato viene inizializzato casualmente. La probabilità di "accidentata" è sempre del 20%, quella di "uscita" dipende invece dai turni trascorsi e sale del 5% ogni turno. "Normale" e "speciale" si dividono la probabilità restante.
    if (token <= 20)
        (*New).stato = accidentata;
    else if (token <= 20 + Turn_Counter)
        (*New).stato = uscita;
    else if (token <= 20 + Turn_Counter + (int)((100-(20+Turn_Counter))/2))
        (*New).stato = normale;
    else
        (*New).stato = speciale;
    return;
}


static void Print_Fields(struct Caverna *Cave){ //La funzione viene usata da Stampa_Cunicoli e opzionalmente da Aggira per illustrare le caratteristiche di una caverna creata da poco
    printf("La caverna ha le seguenti caratteristiche:\nQuantità melassa: %d\nTipo imprevisto: %d\nTipo caverna: %d\n",(*Cave).melassa,(*Cave).imprevisto,(*Cave).stato);
    return;
}


static void Print_Harv(struct Scavatrice Harv){ //La funzione viene usata per stampare informazioni relative alle scavatrici alla fine di ogni turno regolare o di combattimento
    if (Harv.status == libera)
        printf ("\nLa scavatrice è fuori dal cunicolo!");
    else
        printf ("\nLa scavatrice è nella caverna %p", Harv.posizione);
    printf ("\nENERGIA: ");
    for (int i=0;i<Harv.SerbatoioE;i++) //Questi loop permettono di visualizzare il totale di energia e melassa accumulata sottoforma di health bar
        printf("▇ ");
    printf ("\nMELASSA: ");
    printf("\033[1;33m");
    for (int i=0;i<Harv.SerbatoioM;i++)
        printf("▇ ");
    printf("\033[0m");
    printf("\n");
    return;
}


static void Turn_Cycle(struct Scavatrice *Harv){ //La funzione viene chiamata da Gioca per gestire la normale struttura di un turno
    int token = 1; //Questa variabile viene usata per tenere traccia del se il turno sia stato "speso" o meno. E' possibile infatti che il giocatore scelga una mossa che non può fare per i motivi più vari, nel qual caso gli viene riproposta la scelta ma il token non viene consumato.
    do {
        int scelta = 0; //Questa variabile viene usata per registrare il successivo input di scelta del giocatore
        printf ("\nHai quattro opzioni disponibili:\n-Inserisci 1 per provare ad avanzare ancora più in profondità.\n-Inserisci 2 per provare ad abbattere una parete nella caverna in cui sei ora.\n-Inserisci 3 per aprirti una via alternativa se sei stato bloccato dalle macerie di un crollo.\n-Inserisci 4 per cercare di aprirti una via di fuga che ti porti fuori dal cunicolo.\n");
        scanf ("%d",&scelta);
        while (getchar() != '\n');
        printf("\n");
        switch (scelta)
        {
            case 1:
                token = Avanza(Harv); //Per ogni mossa scelta dal giocatore, alla variabile token verrà assegnato il valore di ritorno delle funzioni che gestiscono i processi associati a ogni mossa: se la mossa ha avuto luogo token verrà "consumato" e diventerà zero, altrimenti rimarrà uno.
                break;
            case 2:
                token = Abbatti(Harv);
                break;
            case 3:
                token = Aggira(Harv);
                break;
            case 4:
                token = Esci(Harv);
                break;
            default:
                printf("Il valore inserito non corrisponde a un comando valido, si prega di inserire un valore compreso tra 1 e 4.\n");
                break;
        }
    } while (token); //Questo loop è configurato per ripetersi finché una delle funzioni mossa non consuma il token, facendolo passare da uno a zero
    return;
}


static void End_Cycle() { //La funzione stampa un rapporto di fine turno durante i turni regolari
    printf("\033[0m"); //Per questa funzione viene usato un colore neutrale, che non corrisponde a nessuna delle due squadre
    printf("\n+---------------------------------------+\n");
    printf("|          FINE DEL TURNO N. %d          |\n", Turn_Counter/5);
    printf("+---------------------------------------+");
    while (getchar() != '\n');
    printf ("\nSTATO DELLA SCAVATRICE \033[1;31mHARTORNEN\033[0m:"); //Per ogni scavatrice vengono stampati i campi usando Print_Harv
    Print_Harv(HarvesterH);
    printf ("\nSTATO DELLA SCAVATRICE \033[1;34mARVAIS\033[0m:");
    Print_Harv(HarvesterA);
    Turn_Counter += 5; //La fine di un turno determina anche l'aumento di un contatore che viene usato sia per tenere traccia dei turni passati sia della probabilità di trovare l'uscita
    Encounter_Prob += 3; //La fine di un turno determina anche l'aumento della probabilità di incontro tra le due scavatrici
    if ((HarvesterA.SerbatoioE==0) && (HarvesterH.SerbatoioE==0)){ //A fine turno viene anche svolta una valutazione per eleggere eventuali vincitori sulla base dell'esaurimento o meno dell'energia delle scavatrici
        printf("\nEntrambe le scavatrici sono state sconfitte: la partita finisce in parità!"); //Se viene eletto un vincitore viene anche stampato un messaggio
        Victory_flag = VPareggio;
    }
    else if (HarvesterA.SerbatoioE==0){
        printf("\nLa scavatrice Arvais è stata sconfitta!");
        Victory_flag = VHartornen;
    }
    else if (HarvesterH.SerbatoioE==0){
        printf("\nLa scavatrice Hartornen è stata sconfitta!");
        Victory_flag = VArvais;
    }
    if ((HarvesterA.status==libera)||(HarvesterH.status==libera)) //Se una qualsiasi delle scavatrici esce dal suo cunicolo, diventa impossibile l'incontro
        Encounter_Prob = 0;
    if ((Victory_flag==VNowin)&&((HarvesterA.status==libera)&&(HarvesterH.status==libera))){ //Se entrambe le scavatrici escono dal cunicolo, il vincitore viene determinato contando la quantità di Melassa raccolta
        printf("\nEntrambe le scavatrici sono fuggite con successo dai rispettivi cunicoli!\nLa scavatrice Arvais porta a casa un bottino di %d unità di melassa.\nLa scavatrice Hartornen porta a casa un bottino di %d unità di melassa.", HarvesterA.SerbatoioM, HarvesterH.SerbatoioM);
        if (HarvesterA.SerbatoioM>HarvesterH.SerbatoioM)
            Victory_flag = VArvais;
        else if (HarvesterA.SerbatoioM<HarvesterH.SerbatoioM)
            Victory_flag = VHartornen;
        else
            Victory_flag = VPareggio;
    }
    if (Victory_flag==VNowin) //Se non viene eletto un vincitore viene invece stampato un messaggio relativo alle probabilità del turno successivo
        printf("\nInizierà ora il turno N. %d.\nLa probabilità di trovare l'uscita sarà del %d%%.\nLe due scavatrici avranno una probabilità del %d%% di incontrarsi.", Turn_Counter/5, Turn_Counter, Encounter_Prob); 
    while (getchar() != '\n');
    return;
}


static void Scontro_Finale(){
    printf("\033[1;35m"); //Per lo scontro finale viene usato un colore diverso. Dopo una breve introduzione vengono anche stampate le istruzioni con le varie mosse possibili e le relative percentuali di probabilità
    printf("\n...");
    while (getchar() != '\n');
    printf("\nLe vibrazioni provocate dagli scavi fanno crollare improvvisamente un pilastro portante comune a entrambi i cunicoli!\nLe due scavatrici, che credevano di star operando a grande distanza l'una dall'altra si ritrovano di colpo faccia a faccia nella stessa caverna!");
    while (getchar() != '\n');
    printf("\n+----------------------------------------------------------------------------+\n");
    printf("|                              __      _______ _                             |\n");
    printf("|            ▅▇□□█▇▆▅▄▃▂▁      \\ \\    / / ____| |     ▁▂▃▄▅▆▇█□□▇▅           |\n");
    printf("|       ▅▆▇██═▃_▁════ ▔▔▔       \\ \\  / / (___ | |     ▔▔▔ ════▁_▃═██▇▆▅      |\n");
    printf("| ▂▄▅▆▇████████████▅             \\ \\/ / \\___ \\| |         ▅████████████▇▆▅▄▂ |\n");
    printf("| ██████████████████              \\  /  ____) |_|         ██████████████████ |\n");                                         
    printf("|  □▲□▲□▲□▲□▲□▲□▲□▲                \\/  |_____/(_)          ▲□▲□▲□▲□▲□▲□▲□▲□  |\n");                                      
    printf("+----------------------------------------------------------------------------+");
    while (getchar() != '\n');
    printf("\033[0;35m");
    printf("\nI sistemi di armamento di entrambe le scavatrici sono stati attivati! Entrambe ottengono un'unità di energia extra!\n\nLe due scavatrici dovranno a turno decidere come impiegare la loro energia.\nPossono tentare la fuga dalla battaglia o cercare di colpire la scavatrice nemica.\nCiascuna delle due mosse consumerà un'unità di energia dal serbatoio.\nOgni colpo messo a segno ha una probabilità dell'80%% di far perdere melassa alla scavatrice nemica e del 20%% di farle perdere energia.\nLa probabilità di colpire con successo la scavatrice nemica è del 60%%.\nIn caso di fuga con successo di una delle due scavatrici, la battaglia termina alla fine del turno e vince il giocatore con più melassa.\nLa probabilità di successo della fuga è del 35%%.");
    HarvesterA.posizione = HarvesterH.posizione; //Le due scavatrici si trovano nella stessa caverna, quindi la posizione di una viene aggiornata per essere uguale a quella dell'altra
    (HarvesterH.SerbatoioE)++; //Entrambe le scavatrici ottengono un aumento di un'unità di energia prima della battaglia
    (HarvesterA.SerbatoioE)++;
    while (getchar() != '\n');
    Battle_Report(); //Prima dell'inizio del primo turno di combattimento viene stampato un resoconto della situazione
    do {
        if ((rand()%100)+1>5){ //Esiste la possibilità, per quanto solo del 5%, che si verifichi un evento imprevisto durante la battaglia. I turni di combattimento hanno luogo solo se questo evento speciale non si verifica
            printf("\033[0;31m"); //Come nei turni regolari, i turni di battaglia sono scanditi cambiando il colore a seconda del giocatore che sceglie la mossa
            printf("+---------------------------------------+\n");
            printf("|             TURNO HARTORNEN           |\n");
            printf("+---------------------------------------+");
            while (getchar() != '\n');
            Battle_Turn(&HarvesterH,&HarvesterA); //La funzione Battle_Turn è usata per far scegliere al giocatore la sua mossa
            printf("\033[0;34m");
            printf("\n+---------------------------------------+\n");
            printf("|               TURNO ARVAIS            |\n");
            printf("+---------------------------------------+");
            while (getchar() != '\n');
            Battle_Turn(&HarvesterA,&HarvesterH);
            Battle_Report();
        }
        else{
            printf("\033[1;36m"); 
            printf("+---------------------------------------+\n");
            printf("|         ACCADE L'INIMMAGINABILE!      |\n");
            printf("+---------------------------------------+\n");
            printf("\033[0;36m"); 
            printf("\nUN ESSERE ESTREMAMENTE POTENTE FA LA SUA IMPROVVISA COMPARSA NELLA CAVERNA!");
            while (getchar() != '\n');
            if (rand()%2) //L'Easter Egg prevede la comparsa di un personaggio determinato casualmente tra due possibilità. In entrambi i casi si tratta di parodie strampalate di un professore di corso.
                printf("\nSi tratta di TUFIR 'A CARP, ex mentat della casata Ortodoss ora al servizio delle gilde commerciali.\nLe gilde hanno deciso che la nuova rete sequenziale dovrà essere costruita passando proprio attraverso la caverna in cui ora si trovano i giocatori.\nEssi provano a protestare, ma Tufir A' Carp fa partire dai palmi delle sue mani due input di zeri e uni che investono in pieno le due scavatrici, risequenziando i loro computer di bordo.\nEsse vengono trasformate in due automi a stati finiti il cui livello di complessità è tale da far invidia a un vero cervello positronico.\nLe due scavatrici acquisiscono quindi consapevolezza di sé e soprattutto del pericolo incombente.\nEsse scaricano i rispettivi giocatori a terra, esclamano \"Addio e grazie per tutta la melassa!\" e si danno alla fuga.\nI giocatori non possono far altro che assistere allibiti mentre i circuiti della nuova rete sequenziale li investono in pieno.\nEssi vengono ridotti ai mintermini essenziali tramite il metodo tabellare e diventano parte integrante di essa.\nEntrambi i giocatori perdono!");
            else
                printf("\nSi tratta del malvagio BARTH 'O LEE, il temutissimo computer umano capace di svolgere anche i calcoli più impensabili nel giro di pochi secondi grazie alla sua superiore intelligenza.\nSi è guadagnato in tutta la galassia il soprannome di Barth Fener a causa della sua glaciale spietatezza.\nIl suo sguardo disintegratore si posa immediatamente sulle scavatrici, colpevoli di aver fatto rumore con i loro scavi.\nEntrambe vengono istantaneamente atomizzate insieme ai rispettivi giocatori, mentre Barth Fener afferma ironicamente di aver svolto nient'altro che una comune fattorizzazione, per di più estremamente banale.\nEntrambi i giocatori perdono!");
            while (getchar() != '\n');
            Victory_flag=VPareggio; //A prescindere da chi sia il personaggio che fa la sua comparsa l'esito è lo stesso, ovvero la sconfitta di entrambi i giocatori
        }
    } while (Victory_flag==VNowin); //Il loop fa durare la battaglia finché non viene determinato un vincitore
    return;
}


static void Battle_Turn (struct Scavatrice *Harv, struct Scavatrice *Enemy){ //La funzione che gestisce il turno di battaglia prende come argomenti due riferimenti: il primo è del giocatore che gioca il turno, il secondo è la scavatrice nemica usata come bersaglio di una delle due azioni
    int scelta=2; //Questa variabile registra il successivo input dell'utente
    do {
        printf ("\nPuoi scegliere di provare a scappare o azionare i sistemi d'arma per cercare di colpire la scavatrice avversaria.\n-Inserisci 0 per tentare la fuga.\n-Inserisci 1 per aprire il fuoco.\n");
        scanf("%d",&scelta);
        while (getchar() != '\n');
        switch (scelta)
        {
            case 0:
                if (((rand()%100)+1)<=35) { //Se il giocatore sceglie di provare a scappare ha una probabilità del 35% di riuscirci
                    printf ("\nCongratulazioni!\nLa tua scavatrice ha scoperto una via di fuga e sarà presto in salvo!");
                    Harv->status = libera; //Se la fuga di una scavatrice ha successo oltre a venir stampato un messaggio che lo riferisce essa acquisisce lo status di "libera"
                }
                else //Se la fuga fallisce viene stampato un messaggio per riferirlo
                    printf ("\nNel tentare la fuga la tua scavatrice si è imbattuta in un terreno particolarmente accidentato!\nIl tuo tentativo di fuga è fallito!");
                break;
            case 1:
                if (((rand()%100)+1)<=60){ //Se il giocatore sceglie di provare a colpire la scavatrice nemica con le armi c'è una probabilità del 60% che il colpo vada a segno
                    printf("\nIl cannone laser della tua scavatrice brilla nell'oscurità della caverna!\nLa scavatrice nemica non fa in tempo a spostarsi per evitare il colpo!\nHai colpito il bersaglio!");
                    while (getchar() != '\n');
                    if (((rand()%100)+1)<=80){ //Se il colpo va a segno l'effetto viene determinato casualmente: nell'80% dei casi provoca una perdita di melassa, nel 20% dei casi provoca una perdita di energia
                        int perdita = (rand()%2)+1; //la perdita di melassa è un valore casuale tra 1 e 2
                        printf("La scavatrice nemica viene danneggiata, perdendo %d unità di melassa!", perdita);
                        Enemy->SerbatoioM -= perdita; //la perdita viene detratta dal serbatoio della scavatrice bersaglio dopo che un messaggio riferisce l'accaduto
                        if ((Enemy->SerbatoioM)<0)
                            Enemy->SerbatoioM = 0; //La quantità di melassa deve essere un valore positivo quindi se il serbatoio è vuoto esso non viene fatto scendere sotto zero.
                    }
                    else {
                        printf("La scavatrice nemica viene danneggiata, perdendo 1 unità di energia!");
                        (Enemy->SerbatoioE)--; //Se viene colpito il serbatoio dell'energia viene detratta un'unità dopo la stampa di un messaggio
                    }
                }
                else //Se il colpo va a vuoto viene stampato un messaggio che lo riferisce
                    printf ("\nIl cannone laser della tua scavatrice brilla nell'oscurità della caverna!\nMa un'agile manovra operata all'ultimo momento dal pilota della scavatrice riesce a fargli evitare il colpo!\nHai mancato il bersaglio!");
                break;
            default:
                printf("\nIl valore inserito non è valido, riprovare.\n");
                break;
        }
    } while ((scelta!=0)&&(scelta!=1)); //Questo loop assicura che siano inseriti solamente valori validi
    (Harv->SerbatoioE)--;//Qualsiasi sia la mossa scelta dal giocatore, essa ha come costo un'unità di energia. Esso viene pagato alla fine del turno, quando l'azione si è risolta.
    while (getchar() != '\n');
    return;
}                             


static void Battle_Report(){ //La funzione stampa il resoconto di fine turno durante la battaglia finale. Essa si occupa anche di controllare alcune condizioni a cui è associata l'eventuale elezione di un vincitore
    printf("\033[0;35m");
    printf("\n+---------------------------------------+\n");
    printf("|           SITUAZIONE ATTUALE:         |\n");
    printf("+---------------------------------------+");
    while (getchar() != '\n');
    printf ("\nSCAVATRICE \033[1;31mHARTORNEN\033[0m:");
    Print_Harv(HarvesterH); //Di ogni scavatrice vengono stampate le informazioni usando Print_Harv
    printf("\033[0;35m");
    printf ("\nSCAVATRICE \033[1;34mARVAIS\033[0m:");
    Print_Harv(HarvesterA);
    printf("\033[0;35m");
    if ((HarvesterA.SerbatoioE==0) && (HarvesterH.SerbatoioE==0)){ //Se una o più scavatrici esauriscono l'energia la battaglia termina e viene calcolato il vincitore
        printf("\nEntrambe le scavatrici hanno esaurito l'energia: la battaglia finisce in parità!");
        Victory_flag = VPareggio;
    }
    else if (HarvesterA.SerbatoioE==0){
        printf("\nLa scavatrice Arvais ha esaurito l'energia ed è stata sconfitta!");
        Victory_flag = VHartornen;
    }
    else if (HarvesterH.SerbatoioE==0){
        printf("\nLa scavatrice Hartornen ha esaurito l'energia ed è stata sconfitta!");
        Victory_flag = VArvais;
    }
    if (Victory_flag==VNowin){ //Se una qualsiasi delle scavatrici riesce a scappare la battaglia termina e il vincitore viene determinato confrontando la melassa raccolta. Questo ha luogo solo se il controllo dell'energia non aveva già determinato un vincitore
        if ((HarvesterA.status==libera)||(HarvesterH.status==libera)){
            printf("\nLa fuga ha posto fine alla battaglia!\nLa scavatrice Arvais porta a casa un bottino di %d unità di melassa.\nLa scavatrice Hartornen porta a casa un bottino di %d unità di melassa.", HarvesterA.SerbatoioM, HarvesterH.SerbatoioM);
            if (HarvesterA.SerbatoioM>HarvesterH.SerbatoioM)
                Victory_flag = VArvais;
            else if (HarvesterA.SerbatoioM<HarvesterH.SerbatoioM)
                Victory_flag = VHartornen;
            else
                Victory_flag = VPareggio;
        }
    }
    while (getchar() != '\n');
    return;
}


static int Scelta_Casata(){ //La funzione viene usata in Crea_Cunicoli per determinare su quale cunicolo deve agire uno dei tre comandi principali. Essa si limita a registrare un valore inserito in input che può essere 0 o 1 e a restituirlo
    int scelta = 2; //Questa variabile viene usata per registrare il successivo input del giocatore
    do {
        printf ("\nPer quale casata?\n-Inserire 0 per Arvais.\n-Inserire 1 per Hartornen.\n");
        scanf("%d",&scelta);
        while (getchar() != '\n');
        printf("\n");
        if ((scelta!=0)&&(scelta!=1))
            printf("Il valore inserito non è valido, riprovare.\n");
    } while ((scelta!=0)&&(scelta!=1)); //Questo ciclo assicura che vengano inseriti solo valori validi
    return scelta;
}


static struct Caverna *NextStep (struct Caverna *Cave){ //La funzione viene usata in varie parti del programma per scoprire se una determinata caverna passata come argomento ha almeno un collegamento attivo. La ricerca procede secondo l'ordine Sinistra->Avanti->Destra e viene interrotta appena viene trovato un collegamento, nel qual caso la funzione restituisce il riferimento a quella caverna. Se non viene trovato nessun collegamento, viene restituito come riferimento la stessa caverna iniziale.
    if (Cave->NextSinistra != NULL)
        Cave = Cave->NextSinistra;
    else if (Cave->NextAvanti != NULL)
        Cave = Cave->NextAvanti;
    else if (Cave->NextDestra != NULL)
        Cave = Cave->NextDestra;
    return Cave;
}


static void Cave_Evaluation (struct Scavatrice *Harv){ //La funzione si occupa di far subire alla scavatrice passata come argomento gli effetti della caverna in cui si trova. Viene chiamata non appena una scavatrice entra in una nuova caverna.
    if ((Harv->posizione)->stato != normale){ //Il primo messaggio che viene stampato dipende dal campo "stato" della caverna a cui punta il campo "posizione" della scavatrice.
            if ((Harv->posizione)->stato == speciale) {
                printf ("\nNella nuova caverna le radiazioni irradiate dalla melassa presente nel resto del cunicolo sono particolarmente forti!\nIl serbatoio di energia subisce una ricarica automatica di un'unità derivante dall'esposizione a queste radiazioni.");
                (Harv->SerbatoioE)++;
            }
            else if ((Harv->posizione)->stato == accidentata) {
                printf ("\nLa nuova caverna presenta un terreno accidentato. Per farsi strada la scavatrice è costretta a usare il massimo della potenza, consumando un'unità di energia!");
                (Harv->SerbatoioE)--;
            }
            else if ((Harv->posizione)->stato == uscita){
                printf("\nOggi è il tuo giorno fortunato!\nI sensori di navigazione della scavatrice hanno scoperto che da questa caverna sarebbe possibile tracciare con facilità una scorciatoia.\nFare ciò permetterebbe alla scavatrice di emergere istantaneamente dal cunicolo!");
            }
        }
    else
        printf("\nLe caratteristiche geologiche della nuova caverna sono conformi ai parametri di normalità.");
    while (getchar() != '\n');
    if ((Harv->posizione)->melassa != nessuna){ //Il secondo messaggio stampato dipende dalla quantità di melassa trovata. Se ne viene trovata, viene specificata la quantità e chiesto al giocatore dove immagazzinarla tramite il messaggio stesso
        printf("\nNella nuova caverna è stata trovata della melassa! Nello specifico, %d unità.\nScegli ora se immagazzinarle nel serbatoio dell'energia o in quello della melassa.\n", (Harv->posizione)->melassa);
        int scelta = 2; //Questa variabile registra il successivo input di scelta del giocatore
        do {
            printf ("-Inserisci 0 per caricare il serbatoio dell'energia.\n-Inserisci 1 per caricare il serbatoio della melassa.\n");
            scanf ("%d", &scelta);
            if (scelta==0)
                Harv->SerbatoioE += (Harv->posizione)->melassa; //Sulla base della scelta del giocatore viene caricato uno dei due serbatoi
            else if (scelta==1)
                Harv->SerbatoioM += (Harv->posizione)->melassa;
            else
                printf ("\nE' stato inserito un valore non valido, si prega di riprovare.\n");
        } while ((scelta!=0)&&(scelta!=1)); //Questo ciclo assicura che vengano inseriti solo valori validi
    }
    else
        printf ("\nNella nuova caverna non è stata trovata melassa, che sfortuna!.");
    while (getchar() != '\n');
    if ((Harv->posizione)->imprevisto != nessun_imprevisto){ //L'ultimo messaggio riferisce informazioni relative al verificarsi o meno di un eventuale imprevisto
        if ((Harv->posizione)->imprevisto == crollo) {
            printf ("\n\aLe vibrazioni legate al movimento della scavatrice provocano la caduta di alcuni massi dal soffitto della caverna.\nUno di questi colpisce il serbatoio dell'energia causando la perdita di un'unità di energia.");
            (Harv->SerbatoioE)--; //Se l'imprevisto della caverna è un crollo il serbatoio dell'energia perde un'unità
        }
        else if ((Harv->posizione)->imprevisto == baco) {
            printf ("\n\a\aUn mostruoso baco delle sabbie emerge a sorpresa dal terreno!\nLa scavatrice viene divorata in un sol boccone!");
            Harv->SerbatoioE=0, Harv->SerbatoioM=0; //Se l'imprevisto della caverna è un baco i serbatoi vengono svuotati del tutto e scendono a 0
        }
    }
    else 
        printf ("\nPuoi tirare un sospiro di sollievo: nella nuova caverna non c'è nessun pericolo in agguato.");
    while (getchar() != '\n');
    return;
}


static struct Caverna *Dept_Charge (struct Caverna*Node){ //La funzione viene usata per deallocare tutto ciò che si trova davanti al nodo che viene passato come argomento. In determinate situazioni viene invocata da Termina_Gioco, da Crea_Cunicoli o da Aggira ma non può essere chiamata dai giocatori.
    printf("Depth Charge è entrata nel nodo %p.\n", Node); //La funzione è stata configurata per stampare un messaggio ogni volta che entra in un nodo. Ogni nodo in cui entra la funzione verrà prima o poi distrutto, ma essendo la funzione ricorsiva l'ordine preciso dipende dal numero di chiamate che hanno luogo.
    while (NextStep(Node)!=Node){ //Se tramite la funzione NextStep viene trovato almeno un collegamento attivo, Depth_Charge verrà invocata su ogni caverna associata a ogni collegamento attivo. Si esce da questo loop solo se le chiamate successive hanno già chiuso tutti i collegamenti attivi o se non ve ne erano affatto.
        if ((Node->NextAvanti)!=NULL)
            Node->NextAvanti = Dept_Charge(Node->NextAvanti);
        if ((Node->NextDestra)!=NULL)
            Node->NextDestra = Dept_Charge(Node->NextDestra);
        if ((Node->NextSinistra)!=NULL)
            Node->NextSinistra = Dept_Charge(Node->NextSinistra);
    }
    printf("Depth Charge distruggerà ora il nodo %p.\n", Node); //Se la ricerca di collegamenti successivi non produce risultati, Dept_Charge procede a distruggere il nodo in cui si trova: prima di farlo, stampa un messaggio.
    free(Node);
    return NULL; //Dopo la distruzione di un nodo Dept_Charge restituisce il valore NULL
}