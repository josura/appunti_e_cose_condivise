#sviluppo reti neurali con keras, sta parlando di altre librerie come tensorflow(altre API)
#keras è una api ad alto livello sviluppata per semplificare la costruzione la sperimentazione e l'utilizzo di reti neurali
#i nostri modelli possiamo mandarli ed implementarli in cpu gpu o una tpu(tensor processing unit)
#interfaccia estremamente semplificata
#il problema è che keras è una libreria nativa di python, 
#keras è importante perchè astraiamo dal processo di passaggio da piattaforma a piattaforma, e molte altre cose

install.packages("keras",dependencies = TRUE)

library(keras)

install_keras()

#per realizzare una rete neurale dobbiamo prima definire il modello della nostra rete(sequenziale o multi-GPU), inoltre dobbiamo stabilire la struttura della rete(caratteristiche della rete e altro)
#dobbiamo compilare il modello, cioè istruire keras come addestrare il modello(algoritmo di learning e ottimizzazione(stochastic gradient descent preferito, arriva più velocemente a convergenza), funzione di errore MSE(gradient descent) e loss, metriche del nostro modello(accuracy e altro, nelle slide per vederle), le metriche non sono considerate nel training, solo la loss function viene considerata(loss function come MSE o cross-entropy))
#addestrare il modello, batch size per cross validation, epoche, dimensioni validation set
#valutazione delle performance
#usare il modello

#tutte le operazioni si fanno con %>% concatenatore delle operazioni

#un layer sono le operazioni svolte nelle reti neurali

#layer dropout per regolarizzazione, evitare overfitting, mette i pesi di un layer a 0?, funziona meglio di quello di cui avevamo parlato in passato(random azzeramento)

#binary per due classi e categorical per più classi

#fit per addestrare il modello
#plot(hystory) per vedere grafici della loss e della accuracy da noi scelte per il nostro modello, come input le epoche, possiamo vedere e notare l'overtraining con troppe epoche

#evaluate(x_test,y_test) per vedere le performance sul test set

#