/*ALCUNE CONSIDERAZIONI PRELIMINARI SULLO SCHEMA RELAZIONALE PROPOSTO

Gli attributi "titoloCanzone" nelle relazioni 'autore' ed 'esecuzione' hanno lo stesso nome, lo stesso
dominio e rappresentano semanticamente la stessa cosa, ragion per cui sarebbe sensata l'aggiunta di un
vincolo di integrità referenziale che li leghi. Sfortunatamente, ognuna delle strade percorribili presenta 
dei problemi:

OPZIONE 1: autore.titoloCanzone è chiave esterna su esecuzione.titoloCanzone. In questo scenario esecuzione.titoloCanzone
deve essere sottoposto a vincolo di univocità affinché sia compatibile con il vincolo di integrità referenziale, il che
implica l'impossibilità di conservare nella base di dati sia la versione originale di una canzone (ad esempio "The Sound 
of Silence" cantata da "Paul Simon") che eventuali cover di altri musicisti (ad esempio "The Sound of Silence" cantata 
dal gruppo "Disturbed"). Questa scelta rende anche obsoleto l'attributo esecuzione.codiceReg, giacché essendo 
esecuzione.titoloCanzone sia univoco che non null potrebbe fare lui stesso da chiave primaria (la non nullità deriva 
dal fatto che considero senza senso l'esecuzione di una canzone senza titolo). In aggiunta a questo sarebbe anche 
impossibile inserire nella base di dati l'autore di una canzone di cui non risultino presenti anche delle esecuzioni.

OPZIONE 2: esecuzione.titoloCanzone è chiave esterna su autore.titoloCanzone. In questo scenario è autore.titoloCanzone a
dover essere sottoposto a vincolo di univocità affinché sia compatibile con il vincolo di integrità referenziale. Relativamente
alla chiave primaria valgono le stesse considerazioni già viste per l'opzione 1: supponendo che titoloCanzone non possa essere
null (giacché non esistono canzoni prive di titolo, almeno a quanto ne so io), autore.titoloCanzone sarebbe a quel punto sia
univoco che non null e quindi potrebbe diventare esso stesso chiave primaria. In questo caso inoltre sarebbe fortemente 
opportuno che lo facesse perché la coppia ( autore.nome , autore.titoloCanzone ) smetterebbe di essere chiave candidata 
in virtù dell'esistenza di una superchiave più piccola al suo interno (autore.titoloCanzone): percorrere questa strada 
implicherebbe quindi l'entrare in conflitto con quanto prescritto dalla traccia, secondo cui la chiave primaria deve invece 
essere la coppia (autore.nome, autore.titoloCanzone). Esistono inoltre ulteriori implicazioni scomode: l'univocità di 
autore.titoloCanzone implicherebbe l'impossibilità di registrare nella base di dati canzoni che hanno lo stesso titolo 
ma autori diversi (ad esempio "The Power of Love" di "Jennifer Rush" e "The Power of Love" di "Huey Lewis"), oppure autori 
molteplici che hanno collaborato alla scrittura di una stessa canzone. Nella relazione 'esecuzione' risulterebbe inoltre 
impossibile inserire tuple relative a canzoni il cui autore non sia stato già preventivamente registrato. Anche la formulazione
di politiche di reazione sensate sarebbe alquanto dolorosa: ad esempio in caso di cancellazione SET NULL non sarebbe 
applicabile per la non nullità di titoloCanzone, SET DEFAULT non sarebbe applicabile per l'assenza di un valore di default 
sensato adatto a qualsiasi canzone e CASCADE finirebbe addirittura con il cancellare tutte le esecuzioni di una canzone 
qualora venisse rimosso l'artista che ne è l'autore, incluse eventuali cover (il che potrebbe a sua volta innescare 
la rimozione di un artista come "cantante" qualora una di queste cover fosse la sua unica esecuzione).

OPZIONE 3: tra esecuzione.titoloCanzone e autore.titoloCanzone non vi è alcun vincolo di integrità referenziale. 
La base di dati ne esce indebolita in virtù dell'assenza a livello semantico di un legame tra due attributi che oltre a 
condividere lo stesso dominio rappresentano di fatto la stessa cosa. Perdurano inoltre problemi di ambiguità legati al come è
formulato lo schema relazionale originale in senso generale, con particolare riferimento al fatto che due canzoni diverse 
possono avere lo stesso titolo senza che una sia cover dell'altra. Se ad esempio inserissimo nella relazione 'autore' le tuple
("Jennifer Rush", "The Power of Love") e ("Huey Lewis" , "The Power of Love") non ci sarebbe modo di stabilire se sono 
due artisti che hanno scritto due canzoni diverse ma omonime oppure due autori che hanno collaborato insieme alla scrittura di
una stessa canzone. Questa ambiguità è pericolosa soprattutto per la query numero 2, che chiede di identificare i cantautori 
(ovvero le persone che hanno "scritto e cantato la stessa canzone"). La soluzione che ho proposto è un join di 'autore, 
'esecuzione' e 'cantante' che sfrutti nomeAutore = nomeCantante e cantante.codiceReg = esecuzione.codiceReg, seguito da 
una selezione basata sull'uguaglianza tra esecuzione.titoloCanzone e autore.titoloCanzone: sfortunatamente la mia soluzione 
produrrebbe un falso positivo qualora un artista avesse scritto una canzone con un certo titolo e allo stesso tempo cantato 
una canzone diversa il cui titolo è identico. Tra quelle esaminate questa rimane comunque l'opzione meno dolorosa.

OPZIONE BONUS: il problema potrebbe venir risolto alla radice intervenendo direttamente sullo schema relazionale proposto 
in partenza, ma ho preferito astenermi dal farlo perché la cosa avrebbe determinato modifiche alla traccia originale del 
progetto. Nello specifico, avrei proposto le seguenti modifiche:
canzone(ID_CANZONE, titoloCanzone);
esecuzione (CODICEREG, id_canzone, anno); -> id_canzone è chiave esterna su canzone(id_canzone)
autore(NOMEAUTORE, ID_CANZONE); -> id_canzone è chiave esterna su canzone(id_canzone)
Grazie a queste modifiche due canzoni diverse ma con lo stesso titolo non creano ambiguità perché sono associate a due 
id_canzone differenti. Se si suppone che per ogni canzone esista un solo autore la relazione che lega artisti e canzoni 
diventa 1-N e la relazione 'autore' può addirittura essere sostituita dall'aggiunta di un singolo attributo 'autoreOriginale'
alla relazione 'canzone' (rendendolo chiave esterna su artista(nome)). Ciò non sarebbe però attuabile qualora la base di dati
dovesse rappresentare anche le situazioni in cui più artisti hanno collaborato alla scrittura di una stessa canzone.

Alla luce di questa breve analisi preliminare procederò seguendo l'OPZIONE 3, giacché tra quelle che non necessitano 
di modificare lo schema relazionale originale è quella che produce gli effetti meno gravi.*/


/*ESERCIZIO 1*/


DROP SCHEMA IF EXISTS DBDischi CASCADE;
CREATE SCHEMA DBDischi;
SET search_path TO DBDischi;

CREATE DOMAIN NumeroSerieDisco AS Integer NOT NULL CHECK (VALUE >= 0); --Sto qui supponendo che i numeri di serie non possano essere negativi
CREATE DOMAIN CodiceRegistrazioneCanzone AS Integer NOT NULL CHECK (VALUE >= 0); --Sto qui supponendo che i numeri di serie non possano essere negativi
CREATE DOMAIN NomePersona AS Varchar NOT NULL;
CREATE DOMAIN Anno AS Date;
CREATE DOMAIN PrezzoEuro AS Integer CHECK (VALUE >= 0); --Il prezzo è rappresentato in centesimi, ad esempio 30 euro corrispondono a un valore di 3000
CREATE DOMAIN TitoloAlbum AS Varchar NOT NULL;
CREATE DOMAIN TitoloCanzone AS Varchar NOT NULL; 
CREATE DOMAIN Nazionalita AS Varchar;
CREATE DOMAIN Sesso AS Char(1) CHECK (VALUE = 'M' OR VALUE = 'F');

CREATE TABLE disco (
	nroSerie NumeroSerieDisco PRIMARY KEY, 
	titoloAlbum TitoloAlbum, 
	anno Anno, 
	prezzo PrezzoEuro);
CREATE TABLE esecuzione (
	codiceReg CodiceRegistrazioneCanzone PRIMARY KEY, 
	titoloCanzone TitoloCanzone, 
	anno Anno);
CREATE TABLE artista (
	nome NomePersona PRIMARY KEY, 
	nazionalita Nazionalita, 
	dataNascita Anno, 
	sesso Sesso);
CREATE TABLE contiene (
	nroSerieDisco NumeroSerieDisco REFERENCES disco (nroSerie) ON DELETE CASCADE ON UPDATE CASCADE, 
	codiceReg CodiceRegistrazioneCanzone REFERENCES esecuzione (codiceReg) ON DELETE CASCADE ON UPDATE CASCADE, 
	PRIMARY KEY (nroSerieDisco, codiceReg));
CREATE TABLE cantante (
	nomeCantante NomePersona REFERENCES artista (nome) ON DELETE CASCADE ON UPDATE CASCADE,
	codiceReg CodiceRegistrazioneCanzone REFERENCES esecuzione (codiceReg) ON DELETE CASCADE ON UPDATE CASCADE,
	PRIMARY KEY (nomeCantante, codiceReg));
CREATE TABLE autore (
	nomeAutore NomePersona REFERENCES artista (nome) ON DELETE CASCADE ON UPDATE CASCADE, 
	titoloCanzone TitoloCanzone, 
	PRIMARY KEY (nomeAutore, titoloCanzone));

INSERT INTO disco VALUES 
	('1', 'Let the bad times roll', '2021-04-16', '3090'), 
	('2', 'Rock Classics 1995', '1995-02-01', '3100'), 
	('3', 'Rock Classics 1997', '1997-12-04', '2100'), 
	('4', 'Qualcosa di nuovo', '2020-10-30', '1999'), 
	('5', 'Rock Classics 1999', '1999-10-11', '1299'), 
	('6', 'No one sings like you' , NULL , '3750'); 

INSERT INTO artista VALUES
	('Bob Dylan', 'USA', '1941-05-24', 'M'), 
	('Pat Metheny', 'USA', '1954-08-12', 'M'), 
	('Jon Bon Jovi', 'USA', '1962-03-02', 'M'), 
	('Bryan Adams', NULL, '1969-11-05', 'M'), 
	('Max Pezzali', 'Italia' , '1967-11-14', 'M'), 
	('Elton John', 'UK', '1947-03-25', 'M'), 
	('Bruce Springsteen', 'USA', '1949-09-23', 'M'), 
	('Tarja Turunen', 'Finlandia', '1977-08-17', 'F');

INSERT INTO esecuzione VALUES
	('1', 'Man of constant sorrow', '1962-03-19'), 
	('2', 'The last train home', '1987-07-01'), 
	('3', 'Let it be', '1999-10-11'), 
	('4', 'Lonely Nights', '1981-07-21'), 
	('5', 'Non smettere mai', '2020-10-30'), 
	('6', 'With a little help from my friends', '1992-10-27'), 
 	('7', 'Born to run', '1975-08-25'), 
	('8', 'Help!', '1995-02-01'), 
	('9', 'Third Wind', '1987-07-01'),
	('10', 'Rocket man', '1972-04-17'),
	('11', 'Distance', '1987-07-01'), 
	('12', 'Oasis', '2007-11-19'); 

INSERT INTO contiene VALUES 
	('5','1'), 
	('5','2'), 
	('5','3'), 
	('5','4'), 
	('4','5'),
	('3','6'), 
	('3','7'), 
	('2','8'), 
	('2','9'), 
	('2','10'), 
	('2','11');

INSERT INTO cantante VALUES
	('Bob Dylan', '1'), 
	('Pat Metheny', '2'), 
	('Jon Bon Jovi', '3'), 
	('Bryan Adams', '4'), 
	('Max Pezzali', '5'), 
	('Jon Bon Jovi', '6'),
	('Bruce Springsteen', '7'), 
	('Jon Bon Jovi', '8'), 
	('Pat Metheny', '9'), 
	('Elton John', '10'), 
	('Pat Metheny', '11'), 
	('Tarja Turunen', '12');

INSERT INTO autore VALUES
	('Bob Dylan', 'Man of constant sorrow'), 
	('Pat Metheny', 'The last train home'), 
	('Bryan Adams', 'Lonely Nights'), 
	('Max Pezzali', 'Non smettere mai'), 
	('Pat Metheny', 'Distance'), 
	('Pat Metheny', 'Third Wind'), 
	('Elton John', 'Rocket man'),
	('Bruce Springsteen', 'Born to run');


/*ESERCIZIO 2*/


/* 1) Trovare gli album pubblicati dopo il 2018 che costano più di 30 euro. */
SELECT * FROM disco WHERE (anno > '2018-12-31' AND prezzo > 3000);


/* 2) Trovare i cantautori (persone che hanno scritto e cantato la stessa canzone) di nazionalità non Italiana */
SELECT DISTINCT nome, nazionalita, dataNascita, sesso FROM artista
JOIN cantante ON artista.nome = cantante.nomeCantante 
JOIN autore ON artista.nome = autore.nomeAutore 
JOIN esecuzione USING (codiceReg) 
WHERE (esecuzione.titoloCanzone = autore.titoloCanzone AND (nazionalita IS NOT NULL AND nazionalita != 'Italia'));
/*Un cantante la cui nazionalita è NULL tecnicamente è un cantante di cui non possiamo dire nulla. Pertanto viene
escluso dai risultati perché non possiamo garantire al 100% che non sia Italiano. */


/* 3) Trovare i cantanti del disco che contiene il maggior numero di canzoni */
SELECT DISTINCT nome, nazionalita, dataNascita, sesso FROM artista
JOIN cantante ON cantante.nomeCantante = artista.nome
JOIN esecuzione USING (codiceReg) 
JOIN contiene USING (codiceReg)
WHERE (contiene.nroSerieDisco =ANY (
	SELECT nroSerie FROM disco
	JOIN contiene ON contiene.nroSerieDisco = disco.nroSerie 
	JOIN esecuzione USING (codiceReg) 
	GROUP BY nroSerie
	HAVING COUNT(esecuzione.codiceReg) >= ALL (
		SELECT COUNT(esecuzione.codiceReg) FROM disco
		JOIN contiene ON contiene.nroSerieDisco = disco.nroSerie 
		JOIN esecuzione USING (codiceReg) GROUP BY nroSerie
		)
	)
);
/*VERSIONE BIS*/
SELECT DISTINCT nome, nazionalita, dataNascita, sesso FROM artista
JOIN cantante ON cantante.nomeCantante = artista.nome
JOIN contiene USING (codiceReg)
WHERE (contiene.nroSerieDisco =ANY (
	SELECT nroSerie FROM disco
	JOIN contiene ON contiene.nroSerieDisco = disco.nroSerie  
	GROUP BY nroSerie
	HAVING COUNT(contiene.codiceReg) >= ALL (
		SELECT COUNT(contiene.codiceReg) FROM disco
		JOIN contiene ON contiene.nroSerieDisco = disco.nroSerie 
		GROUP BY nroSerie
		)
	)
);


/* 4) Trovare i cantanti che hanno registrato una canzone in tutti i dischi degli anni '90 mantenuti nella base di dati */

SELECT nome, nazionalita, dataNascita, sesso FROM artista AS x 
WHERE NOT EXISTS (
	SELECT * FROM disco AS y
	WHERE (anno IS NOT NULL AND anno >= '1990-01-01' AND anno <= '1999-12-31' AND NOT EXISTS (
		SELECT * FROM cantante AS z
		WHERE (z.nomeCantante = x.nome AND z.codiceReg =ANY (
			SELECT codiceReg FROM contiene 
			JOIN disco ON disco.nroSerie = contiene.nroSerieDisco
			WHERE (contiene.nroSerieDisco = y.nroSerie))
			)
		)
	)
);
/*VERSIONE BIS*/
SELECT nome, nazionalita, dataNascita, sesso FROM artista AS x 
WHERE NOT EXISTS (
	SELECT * FROM disco AS y
	WHERE (anno IS NOT NULL AND anno >= '1990-01-01' AND anno <= '1999-12-31' AND NOT EXISTS (
		SELECT * FROM (cantante JOIN contiene USING (codiceReg)) AS z
		WHERE (z.nomeCantante = x.nome AND z.nroSerieDisco = y.nroSerie)
		)
	)
);

/* In virtù dell'assenza in sql del quantificatore universale, la richiesta "per ogni disco degli anni '90 l'artista
deve aver cantato almeno una canzone" va riformulata in "non esistono dischi degli anni '90 in cui non esiste almeno
una canzone cantata dall'artista" */


/* ESERCIZIO 3 */


/* La richiesta della traccia è ambigua giacché l'attributo "numeroCanzoni" può riferirsi tanto alle canzoni cantate quanto
a quelle scritte. Pertanto verranno creati due attributi (uno per le canzoni cantate e uno per le canzoni scritte), con
relativi trigger di aggiornamento. Le funzioni associate sfruttano la variabile TG_OP per comportarsi in modo diverso
sulla base del se l'evento è una cancellazione, un aggiornamento o un inserimento */

CREATE DOMAIN Contatore AS Integer NOT NULL CHECK (Value >= 0) DEFAULT (0);
ALTER TABLE artista ADD COLUMN numeroCanzoniScritte Contatore, ADD COLUMN numeroCanzoniCantate Contatore;

CREATE FUNCTION aggiornaCanzoniCantate()
	RETURNS TRIGGER
AS $BODY$
	DECLARE
	BEGIN
		IF (TG_OP = 'DELETE' OR TG_OP = 'UPDATE') THEN
			UPDATE artista SET numeroCanzoniCantate = numeroCanzoniCantate - 1 WHERE (artista.nome = OLD.nomeCantante);
		END IF;
		IF (TG_OP = 'INSERT' OR TG_OP = 'UPDATE') THEN
			UPDATE artista SET numeroCanzoniCantate = numeroCanzoniCantate + 1 WHERE (artista.nome = NEW.nomeCantante);
		END IF;
		RETURN NEW;
	END
$BODY$
LANGUAGE PLPGSQL;

CREATE TRIGGER aggiornaCanzoniCantate 
	AFTER INSERT OR UPDATE OR DELETE ON cantante
	FOR EACH ROW
	EXECUTE PROCEDURE aggiornaCanzoniCantate();

CREATE FUNCTION aggiornaCanzoniScritte()
	RETURNS TRIGGER
AS $BODY$
	DECLARE
	BEGIN
		IF (TG_OP = 'DELETE' OR TG_OP = 'UPDATE') THEN
			UPDATE artista SET numeroCanzoniScritte = numeroCanzoniScritte - 1 WHERE (artista.nome = OLD.nomeAutore);
		END IF;
		IF (TG_OP = 'INSERT' OR TG_OP = 'UPDATE') THEN
			UPDATE artista SET numeroCanzoniScritte = numeroCanzoniScritte + 1 WHERE (artista.nome = NEW.nomeAutore);
		END IF;
		RETURN NEW;
	END
$BODY$
LANGUAGE PLPGSQL;

CREATE TRIGGER aggiornaCanzoniScritte 
	AFTER INSERT OR UPDATE OR DELETE ON autore
	FOR EACH ROW
	EXECUTE PROCEDURE aggiornaCanzoniScritte();

