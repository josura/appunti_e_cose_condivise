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