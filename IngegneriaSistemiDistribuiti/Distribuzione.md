# strategie di distribuzione
tante operazioni remote, allora si raggruppano le azioni remote in una singola e viene mandato uno o più singoli messaggi(sempre meno della quantità di invocazioni)
dato che la OOP dice che gli oggetti devono avere la Separation of Concerns (fine grained), per oggetti remoti le invocazioni sono troppe e la latenza quindi immensa, bisogna trovare delle strategie utilizzabili.
Da un altro punto di vista se mettiamo troppe invocazioni in un singolo oggetto allora non si rispetterebbero le buone pratiche di programmazione( perché diventa tutto coarse-grained), quindi più difficile da implementare e controllare.
> Una separazione concreta è quella tra server e database, altre divisioni pratiche sono tra web server e application server, front-end e back-end, parte server e client (relativamente ovviamente)
# come separare
per progettare un sistema distribuito fine-grained bisogna
> - usare oggetti fine grained sullo stesso host
> - fornire una interfaccia course-grained 

## remote facade
### intento
fornire una facciata coarse-grained a oggetti fine-grained per migliorare l'efficenza sulla rete

# riepilogo
La volta scorsa avevamo parlato del remote facade, si pone sull'atto di ricezione chiamate, presenta dei metodi con coarse-granularity, tampona la latenza di troppe chiamate per operazioni comuni ed espone una interfaccia utilizzabile dai client.
# come funziona
Implementa una funzionalità formata da più operazioni locali in modo da fornire operazioni collegate con un singolo metodo, mandando i dati necessari in una singola chiamata, è sostanzialmente una interfaccia ai microservizi locali.
Meccanismo di serializzazione per mandare i dati sulla rete, classicone, DTO che raggruppano le richieste in messaggi, ed aiutano a trasmettere i dati al client tramite rete, possono essere serializzati.
Controlli di sicurezza aggiuntivi(visti precedentemente) possono essere messi per gestire le autorizzazioni e gli accessi.
Transazioni che devono essere consistenti (proprietà ACID, o l'altra di cui non mi ricordo il nome), esistono delle librerie a supporto per l'Atomicità (se ti ricordi o tutte le operazioni dipendenti o nessuna), poi commit e checkpoint, se succede qualche problema allora abort del commit corrente, ripristino da ultimo checkpoint. Una buona strategia è fare gli aggiornamenti su un clone, poi con il commit si switcha dalla copia non aggiornata al clone aggiornato, all'aborto si elimina la copia e si riprende dall'oggetto non aggiornato
API che racchiudono tutte le interfaccie che servono al client(la lista delle signature che il client può chiamare).
Come già detto DTO per intercomunicazione tra server e client, una sorta di ponte(non design pattern bridge però mi sembra)..
il lato server deve instanziare il DTO specifico e mandarlo al client(per questo deve essere serializzabile)
la slide precedente faceva vedere le associazioni-relazioni tra i package (rettangolo con rettangolino di nome del package, dove i metodi e le classi saranno collegati allo stesso nome), ora stiamo guardando le classi UML.
L'istanza di DTO si riempie tramite DTOAssembler, che istanzia, riempie e serializza l'istanza da mandare sulla rete al client che ha richiesto o a cui deve arrivare.
registry contiene le chiavi con i riferimenti ai dati che conosce solo il lato server(i dati che il client non dovrebbe conoscere)

# Data Transfer Object
Ci serviva per il **remote facade**
## intento
oggetto che trasporta dati tra processi per ridurre il numero di chiamate a metodo.
## problema
si lavora con una interfaccia remota, più dati ad ogni chiamata e non pochi dati e tante chiamate.
## soluzione
tenere tutti i dati della chiamate in un singolo oggetto da mandare, deve essere serializzabile( e deserializzabile per essere ricostruito) per essere trasmesso.
assemblatore per trasferire dati fra il DTO e gli oggetti del dominio
serializzazione in stringa(esempio xml), plaintext?, oppure tramite binario con formato apposito

## dettagli implementazione
RMI tipo di chiamata a procedura remota indipendente dalla rete implementata in Java, deriva da chiamate a procedura remota (RPC), mette in comunicazione il client ed il lato server, permettendo chiamate locali che in realtà svolgono una comunicazione di rete.
RMI interfaccia sia come sottotipo(di java.rmi.Remote) e chiamata remota, sia come signature.
Per localizzare un oggetto remoto dal client, si usa una operazione di lookup attraverso un identificatore di RMI Registry, questa operazione fornisce un proxy locale per accedere al servizio-oggetto remoto.
Da rivedere implementazione ed utilizzo RMI, DTO e altri.

# Gestione della sessione
Una sessione è una interazione a lungo termine fra un client ed un server. Formata da una o più richieste, inizia tipicamente con il login, tipicamente termina quando viene fatto log-out o va via(o time-out).
## stateful o stateless
stateless significa che non si mantiene uno stato tra le richieste(insieme di attributi che identificano lo stato degli utenti e della connessione), una richiesta non tiene conto di richieste precedenti, richieste isolate.
stateful se tiene traccia di informazioni di sessione tra le richieste della stessa, collezionando dati durante il corso della sessione.
Un server stateful usa più risorse di un server stateless, ricordo degli oggetti ed univocità associazione client->oggetto.
noi vogliamo server stateless e sessioni stateful?
dati di sessione in sessioni stateful possono essere diversi da dati permanenti tra le sessioni.
Ci sono tre scelte per conservare lo stato di sessione: client, server, database.

## client session state
conservare lo stato di sessione sul client e non sul server
### intento
conserva lo stato di sessione sul client.
### funzionamento
> - i dati di sessione sono sul client, quindi è lui che manda i dati di sessione al server, ed il server manda tutto lo stato con ogni risposta
> - DTO per tenere i dati
> - rich-client, i dati nel DTO
> - con interfaccia HTML tre modi: parametri URL, campi nascosti, cookie.
per dati di sessione su URL, dimensione URL limitata, con pochi dati funziona bene almeno finchè non si devono trasmettere dati importanti, associazione ID e risposte del server sempre con lo stesso meccanismo.
Per dati su campi nascosti, campi non visualizzati sulla pagina o sull'URL, 
    <input type="hidden">
dato nascosto codificato in XML o altro.
per dati su cookie, cookie inviati avanti e indietro automaticamente, stato serializzato sui cookie, disattivazione dei cookie significa non funzionamento del sito, il cookie funziona solo con un singolo nome di dominio, per siti separati i cookie non verranno visualizzati.
### quando usarlo
server stateless
l'utente sa che se va giù perde tutto, lo sa
identificazone della sessione, per evitare di catturare una sessione di un altro, ID hashato.
### svantaggi 
grandi quantità di dati inducono tempo di trasferimento maggiorato
dati inviati al client potrebbero essere visti o alterati, i dati inviati devono essere convalidati, criptaggio e decrittaggio potrebberò essere un problema di prestazioni.

## server session state
stato session nel server
### intento
conserva lo stato della sessione sul server in forma serializzata
### funzionamento
> - oggetto di sessione tenuto in memoria
> - mappa in memoria con riferimento agli oggetti di sessione tramite chiave (ID di sessione)
> - sufficente memoria sul server ed un solo app. server
> - per la memoria limitata si serializza l'oggetto di sessione e si immagazina in memoria non volatile(passivazione)
> - storage come filesystem o database dello stesso host
> - se lo storage è accessibile da più host, clustering e failover(simil ridondanza- fault handling).
Serialized LOB, 