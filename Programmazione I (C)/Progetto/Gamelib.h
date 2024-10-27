#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum Tipo_imprevisto {nessun_imprevisto, crollo, baco};
enum Quantita_melassa {nessuna, poca, molta=3};
enum Tipo_caverna {normale, speciale, accidentata, uscita};
enum Freedom_state {operativa, bloccata, libera};               //Enum non prescritto dal pdf guida che viene usato per decidere se la scavatrice deve giocare o meno il turno o se alcune opzioni le devono essere precluse durante il turno stesso.
enum Victorious {VNowin, VArvais, VHartornen, VPareggio};       //Enum non prescritto dal pdf guida che viene usato per determinare gli stati della variabile che registra la presenza o meno di un vincitore

struct Caverna {
    struct Caverna *NextAvanti;
	struct Caverna *NextDestra;
	struct Caverna *NextSinistra;
	enum Quantita_melassa melassa;
	enum Tipo_imprevisto imprevisto;
	enum Tipo_caverna stato;
};

struct Scavatrice {
	struct Caverna *posizione;
	int SerbatoioE;
	int SerbatoioM;
    enum Freedom_state status;                                  //Enum non prescritto dal pdf guida che viene usato per decidere se la scavatrice deve giocare o meno il turno o se alcune opzioni le devono essere precluse durante il turno stesso.
};

void Crea_Cunicoli();                                           //42 di gamelib.c
void Gioca();                                                   //98 di gamelib.c
void Termina_Gioco();                                           //192 di gamelib.c