# stabilità
una transazione è una certa unità di lavoro, etc...
un sistema **resiliente** continua ad elaborare transazione persino quando ci sono dei guasti nei componenti che disturbano l'elaborazione. inteso come **stabilità**.
# timeout
qualsiasi applicazione può essere soggetta a fallimento, specialmente per applicazioni in rete, dato che i dispositivi sono molteplici e i fault di conseguenza maggiori.
il codice non può aspettare indefinitivamente, il timeout è un semplice meccanismo che permette di smettere di aspettare una risposta ed agire di conseguenza.
timeout ben posizionati permettono di isolare guasti.
evitiamo fallimenti in cascata
molte librerie non hanno impostazioni o opzioni di timeout.
i timeout sono importanti per qualsiasi chiamata bloccante dipendente(dalla rete o da operazioni onerose e complesse).
quando si usa il metodo wait() di object bisogna usare la versione con timeout.
con java.util.concurrent usare poll(), offer(), tryLock();
codice invaso dalla gestione di errori per timeout.
Utilizzare uno strato di codice ulteriore di metodi con timeout handling riutilizzabili e modulari.
usare un generico **gateway** che fornisce un template per la gestione di connessione, errori, esecuzione di query e organizzazione dei risultati, il gateway imposta e gestisce i parametri.
inserire tutte queste interazioni da controllare rende più facile applicare 
[Circuit Breaker](#circuit breaker)
spesso si riprova ad eseguire di nuovo l'operazione fallita per timeout.
riprovare immediatamente dopo il fallimento non è una buona strategia, riprovare dopo un periodo.


# circuit breaker(interrruttore di circuito)
prende spunto da un ambiente fisico, se c'è un assorbimento eccessivo di corrente, troppo riscaldamento, rischiano anche altri componenti, la soluzione è stata il fusibile, un componente che si brucia prima del sistema, interrompendo il circuito.
in sostanza l'interruzione del circuito permette al sistema di fallire senza distruggere l'intero sistema, dopo il fallimento deve esserci un controllo e correzione di errori, e ripristino del fusibile-interruttore.
Questo design pattern mira ad interrompere le comunicazioni tra una parte e l'altra.
nello stato di chiuso il circuit breaker  permette le operazioni, se le operazioni falliscono il circuit breaker annota il fallimento e gestisce l'insieme di operazioni di conseguenza.
Quando il circuito è aperto le chiamate del circuit breaker falliscono senza eseguire l'operazione.
di fondo potrebbe essere modellato come una macchina a stati.
I cambiamenti di stato sono registrati in log, e lo stato corrente in monitoring.
utili un po' per tutti i punti critici e per i fallimenti multipli(a cascata, indipendenti, etc...)
E' il pattern fondamentale per i peroblemi derivanti dai punti di interazione.
Usare insieme al pattern timeout ovviamente(sono tutti e due parte integrante per fornire stabilità)

# bulkheads (paratie)
il concetto si basa sulle navi, le paratie servono per dividere la nave in compartimenti a tenuta, le paratie prevengono lo spostamento di acqua da una sezione all'altra, una singola falla nello scafo non affonda la nave.
Principio di contenimento del danno.
la ridondanza fisica è la forma più comune di paratia, il fallimento di un server non coinvolge altri.
Architettura orientata ai servizi è una buona strategia per l'implementazione di paratie e per l'efficienza;
virtualizzazione per permettere efficienza e limitazione dei servizi, isolazione etc...
Se non si vuole usare virtualizzazione CPU binding.
il pattern permette di preservare una parte di funzionalità quando capita qualcosa di brutto.
Dice che è un punto debole per la sicurezza, dato che violare una singola parte del sistema compromette l'intero sistema.

# sistemi reattivi
si dice reattivo un sistema che risponde in tempi ragionevoli anche durante fault e problemi vari.
classica resilienza.
Messaggistica per affidarsi ad un sistema di passaggio di messaggi asincroni per stabilire una connessione fra componenti che assicura un lasco accoppiamento isolamento e location transparency
stiamo rivedendo Circuit Breaker nello specifico come design pattern.

# circuit breaker design pattern
<p align="center"><img src="https://github.com/josura/appunti_e_cose_condivise/blob/master/IngegneriaSistemiDistribuiti/immagini/circuitBreaker.png" /></p>
stato del servizio, monitorazione dell'esecuzione.
