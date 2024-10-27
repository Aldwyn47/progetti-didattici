#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

int main (){
    time_t t;
    srand ((unsigned)time(&t));
    printf("\033[1;33m"); //Printf come questi sono utilizzati in tutto il programma per cambiare il colore dell'output.
    printf("\nPrima di iniziare, si consiglia una visualizzazione a schermo intero per prevenire bug grafici.");
    while (getchar() != '\n'); //Questo loop viene usato per bloccare il testo in attesa che il giocatore prema invio, in modo da scandire l'output secondo ritmi precisi.
    printf("\nBenvenuto in...");
    while (getchar() != '\n');
    printf("\n+--------------------------------------------------------------------------------------+\n");
    printf("|                            _____       _     _     _      _                          |\n");
    printf("|            ▅▇□□█▇▆▅▄▃▂▁   / ____|     | |   | |   (_)    | |  ▁▂▃▄▅▆▇█□□▇▅           |\n");
    printf("|       ▅▆▇██═▃_▁════ ▔▔▔  | (___   __ _| |__ | |__  _  ___| |  ▔▔▔ ════▁_▃═██▇▆▅      |\n");
    printf("| ▂▄▅▆▇████████████▅        \\___ \\ / _  |  _ \\|  _ \\| |/ _ \\ |      ▅████████████▇▆▅▄▂ |\n");
    printf("| ██████████████████        ____) | (_| | |_) | |_) | |  __/_|      ██████████████████ |\n");                                      
    printf("|  □▲□▲□▲□▲□▲□▲□▲□▲        |_____/ \\____|____/|____/|_|\\___(_)       ▲□▲□▲□▲□▲□▲□▲□▲□  |\n");                                      
    printf("+--------------------------------------------------------------------------------------+");
    while (getchar() != '\n');
    printf("\nLa \033[1;34mArvais\033[1;33m e la \033[1;31mHartornen\033[1;33m sono due famiglie che si contendono da anni sul pianeta Sabbie l'estrazione della Melassa, una preziosissima sostanza fondamentale per la struttura della società galattica.\nL'estrazione è ostacolata da numerosi pericoli: improvvise frane, scontri con la compagnia rivale, e i pericolosissimi bachi delle sabbie.\nEssi possono raggiungere lunghezze di centinaia di metri ed ingoiare le scavatrici in un sol boccone!");
    while (getchar() != '\n');
    int scelta = 3; //Questa variabile viene usata per registrare l'input di scelta del giocatore
    printf("\n+----------------------------------------------------------------------------+\n");
    printf("|                        BENVENUTO NEL MENU' PRINCIPALE                      |\n");
    printf("+----------------------------------------------------------------------------+\n");
    printf("\n");
    do  {
        printf("\033[1;33m");
        if ((scelta==1)||(scelta==2)){ //Se il giocatore è stato riportato qui dal Map Editor o da una partita appena conclusa viene stampato un messaggio di "Bentornato"
                printf("\n+----------------------------------------------------------------------------+\n");
                printf("|                        BENTORNATO NEL MENU' PRINCIPALE                     |\n");
                printf("+----------------------------------------------------------------------------+\n\n");
        }
        printf ("INSERISCI ORA UN COMANDO PER DECIDERE COSA FARE:\n\n-Inserisci 1 per aprire il map editor e creare la tua mappa\n-Inserisci 2 per iniziare una nuova partita\n-Inserisci 3 per uscire dal gioco\n\n");
        scanf ("%d",&scelta);
        while (getchar() != '\n');
        switch (scelta)
        {
            case 1:
                Crea_Cunicoli();
                break;
            case 2:
                Gioca();
                break;
            case 3:
                Termina_Gioco();
                break;
            default:
                printf("\nIl valore inserito non è valido, si prega di inserire un valore intero compreso tra 1 e 3.\n\n");
                break;
        }
    } while (scelta !=3); //Il ciclo del menù principale ha termine solo se l'input di scelta corrisponde alla funzione Termina_Gioco
    printf("\033[0m");
    return 0;
}