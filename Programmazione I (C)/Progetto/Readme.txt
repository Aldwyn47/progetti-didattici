Nome: Andrea
Cognome: Imparato
Matricola: 323840

Rispetto a quanto prescritto dalla traccia originale non è stato inserito alcun tetto massimo per la scorta di energia e melassa delle scavatrici.
Questo perché nella mia implementazione della battaglia finale esse possono spararsi a vicenda provocando anche forti perdite di melassa e/o energia (motivo per cui avere serbatoi più ampi consente alla battaglia di durare di più).
La funzione "Abbatti" non ha inoltre un controllo a monte che ne impedisca l'invocazione qualora l'energia della scavatrice fosse 0.
Inserirlo avrebbe comportato la possibilità di provocare uno stallo durante la partita qualora una scavatrice fosse arrivata alla caverna più profonda con esattamente 0 energia.
In quel caso infatti nessuna mossa sarebbe stata possibile: Esci e Aggira possono essere usate solo in circostanze speciali, Avanza solo se c'è una via aperta e Abbatti solo se è rimasta appunto dell'energia nel serbatoio.
Nella mia implementazione una scavatrice in quella situazione (per quanto comunque spacciata) non provoca un ciclo infinito: è costretta per esclusione ad Abbattere una parete qualsiasi e nel farlo si "autodistrugge" perché spinge il motore oltre il limite.
Conseguentemente ho anche scelto di spostare la soglia di eliminazione da -1 a 0: ogni scavatrice è eliminata se la sua energia scende a 0.

All'inizio di un'eventuale battaglia finale entrambe le scavatrici ottengono un'unità di energia extra (per simboleggiare l'attivazione dei sistemi di armamento).
Le due scavatrici dovranno poi decidere a turno come impiegare la loro energia: possono tentare la fuga dalla battaglia o cercare di colpire la scavatrice nemica.
Ciascuna delle due mosse consumerà un'unità di energia dal serbatoio, sia in caso di successo che di fallimento. La probabilità di colpire con successo la scavatrice nemica è del 60%, quella di fuggire con successo del 35%.
Ogni colpo messo a segno ha una probabilità dell'80% di far perdere melassa alla scavatrice nemica (1 o 2 unità, determinato a caso con eque probabilità) e del 20% di farle perdere energia (1 unità, sempre).
In caso di fuga con successo di una delle due scavatrici la battaglia termina alla fine del turno e vince il giocatore con più melassa.

C'è inoltre una probabilità molto bassa (5%) che all'inizio di un turno la battaglia abbia immediatamente termine a causa di un imprevisto.
Questo è costituito dalla comparsa di un essere potentissimo (sorteggiato casualmente tra due possibilità equiprobabili) il cui arrivo viene accompagnato dal messaggio "Accade l'inimmaginabile!".
Ciascuno di questi esseri potentissimi distruggerà immediatamente entrambi i giocatori causando un pareggio. Essi NON SONO ASSOLUTAMENTE una parodia goliardica di altri professori del corso di laurea.

Accanto a ogni dichiarazione di funzione (sia in Gamelib.c che in Gamelib.h) è stato inserito un commento che comincia sempre con un numero: esso indica la linea di codice di Gamelib.c a cui comincia la definizione.
Per poter testare rapidamente tutte le funzionalità del programma si consiglia di alterare gli indici di probabilità per aumentare la probabilità di innesco di determinati eventi.
La probabilità di trovare l'uscita viene inizializzata alla linea 112 di Gamelib.c e incrementata alla linea 681.
La probabilità di incontrare la scavatrice nemica viene inizializzata alla linea 113 di Gamelib.c e incrementata alla linea 682.
Il lancio casuale che determina la comparsa o meno dell'essere potentissimo durante una battaglia finale avviene alla linea 736 di Gamelib.c.