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
# intento
oggetto che trasporta dati tra processi per ridurre il numero di chiamate a metodo.
# problema
si lavora con una interfaccia remota, più dati ad ogni chiamata e non pochi dati e tante chiamate.
# soluzione
tenere tutti i dati della chiamate in un singolo oggetto da mandare, deve essere serializzabile( e deserializzabile per essere ricostruito) per essere trasmesso.
assemblatore per trasferire dati fra il DTO e gli oggetti del dominio
serializzazione in stringa(esempio xml), plaintext?, oppure tramite binario con formato apposito