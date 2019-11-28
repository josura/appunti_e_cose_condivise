install.packages("mlbench")
install.packages("rpart")
install.packages("rpart.plot")
install.packages("randomForest")
install.packages("e1071")
install.packages("class")
install.packages("caret")


rm(list =ls())

#vediamo il dataset

library(mlbench)
data("BreastCancer")
View(BreastCancer)

#i valori di breast cancer sono stati discretizzati
#vogliamo vedere i tumori maligni e benigni classificandoli
#dobbiamo inanzitutto nascondere la classe da predire


breast.data <- BreastCancer[1:length(names(BreastCancer))]
#breast.data$Class<-NULL

#convertiamo le colonne di stringhe in numeri
for(i in 1:(length(names(breast.data))-1))(
  breast.data[,i]<-as.numeric(breast.data[,i])
)

breast.data$Id<-NULL

View(breast.data)

#dividiamo il nostro dataset in training s3et e test set

perc.split <- 0.75
nobs.training <- round(perc.split*nrow(breast.data))

#campioniamo con sample le posizioni casuali
sample.pos <-sample(1:nrow(breast.data),nobs.training)
breast.training <- breast.data[sample.pos,]

View(breast.training)

breast.test <- breast.data[-sample.pos,]
#non vogliamo l'ultima colonna nel testing set, ma vabeh se vogliamo vedere il tasso di errore dobbiamo comunque avere la classe vera

#oggetti formula, predictors e response, il tilde per la proporzionalità, + per aggiungere variabili alla classificazione, - per escludere
# : per far capire che due variabili interagiscono tra loro,^ interazione a coppie nel caso ^2(scorciatoia : sulle coppie, ^n sulle nuple)
#%in% i valori di una variabile sono contenuti in un altra variabile, | per simil bayes
#possiamo pure fare le trasformazioni dei predictor con varie funzioni (I per avere la potenza vera e propria e altro)
#le formule ci tornano più utili nella regressione

#come algoritmo iniziale vediamo CART
library(rpart)
?rpart

breast.cart <- rpart(Class ~ . ,data = breast.training)
breast.cart
#gli asterischi sono i nodi terminali

library(rpart.plot)
printcp(breast.cart)
#cost complexity values per ogni splitting

?rpart.plot
rpart.plot(breast.cart)

#tabella cp (cost complexity?)
breast.cart$cptable
plotcp(breast.cart)
#linea tratteggiata è la soglia, prendiamo gli alberi con uno split vicino alla soglia

best.cp <- breast.cart$cptable[2,"CP"]
best.cp
?prune
#scegliamo l'albero con la migliore cost complexity
breast.cart.prun <- prune(breast.cart,cp=best.cp)
breast.cart.prun
rpart.plot(breast.cart.prun)
#guardiamo le regole associate all'albero decisionale

rpart.rules(breast.cart, cover = T,extra =4)

#predict per la predizione, quelle specifiche anteponendo il nome del tipo di classiicatore-regressatore
?predict
predict(breast.cart,breast.test)
rpart.predict()

#dobbiamo vedere l'accuratezza

true.class <- BreastCancer[-sample.pos,"Class"]
cart.predict <- predict(breast.cart,breast.test,type = "class")
cart.results <- data.frame(real= true.class,predicted=cart.predict)
cart.results
#i booleani sono visti come 1 e 0, sommiamo tutto per vedere i risultati giusti
tuplegiuste<-sum(cart.results$real==cart.results$predicted)
accuratezza<- tuplegiuste/nrow(breast.test)
accuratezza

library(randomForest)
randomForest(Class~.,breast.training)
#non gestisce gli na, se non si specifica niente fa 500 try con tre possibili variabili per ogni spi+lit dove prende il risultato migliore
breast.rf <- randomForest( Class ~. , breast.training,na.action = "na.omit")
plot(breast.rf)
#dal grafico notiamo che dopo un certo numero di trial non abbiamo più un miglioramento sensibile
#calcoliamo la accuratezza
rf.predict <- predict(breast.rf, breast.test, type = "class")
rf.predict
rf.results <- data.frame(real= true.class,predicted=rf.predict)
rf.results
rf.giuste <- sum(rf.results$real==rf.results$predicted,na.rm = T)
accuratezza.rf <- rf.giuste/nrow(breast.test)
accuratezza.rf

#naivebayes
library(e1071)
breast.naive <- naiveBayes(Class ~. ,breast.training,type = c("class","raw"))
#con valori continui abbiamo media e varianza, con valori discreti abbiamo direttamente la prob a priori
breast.naive

naive.predict <- predict(breast.naive, breast.test, type = "class")
naive.predict
naive.results <- data.frame(real= true.class,predicted=naive.predict)
naive.giuste <- sum(naive.results$real==naive.results$predicted,na.rm = T)
accuratezza.naive <- naive.giuste/nrow(breast.test)
accuratezza.naive


#SVM
#è gia in e1071
breast.svm <- svm(Class ~. ,breast.training, probability = T)
breast.svm
breast.svm.linear <- svm(Class ~. ,breast.training, kernel = "linear")
breast.svm.polinomial <- svm(Class ~. ,breast.training, kernel = "polynomial")
breast.svm.sigmoid <- svm(Class ~. ,breast.training, kernel = "sigmoid")
svm.predict <- predict(breast.svm,breast.test, type = "class")
svm.predict
svm.predict.linear <- predict(breast.svm.linear,breast.test, type = "class")
svm.predict.polinomial <- predict(breast.svm.polinomial,breast.test, type = "class")
svm.predict.sigmoid <- predict(breast.svm.sigmoid,breast.test, type = "class")
svm.predict
#svm ha direttamente tolto gli na(nel caso di na.omit, in teoria dicono che si potrebbe usare na.exclude nella costruzione del modello, oppure complete cases teoricamente, bisogna cercare bene, di default), dobbiamo andare a trovare il numero dei record, cioè dobbiamo identificarli
true.class.no.na <- BreastCancer[as.numeric(names(svm.predict)),"Class"]
svm.results <- data.frame(real= true.class.no.na,predicted=svm.predict)
svm.results.linear <- data.frame(real= true.class.no.na,predicted=svm.predict.linear)
svm.results.polinomial <- data.frame(real= true.class.no.na,predicted=svm.predict.polinomial)
svm.results.sigmoid <- data.frame(real= true.class.no.na,predicted=svm.predict.sigmoid)
svm.giuste <- sum(svm.results$real==svm.results$predicted,na.rm = T)
svm.linear.giuste <- sum(svm.results.linear$real==svm.results.linear$predicted,na.rm = T)
svm.linear.polinomial <- sum(svm.results.polinomial$real==svm.results.polinomial$predicted,na.rm = T)
svm.giuste.sigmoid <- sum(svm.results.sigmoid$real==svm.results.sigmoid$predicted,na.rm = T)
accuratezza.svm <- svm.giuste/nrow(breast.test)
accuratezza.svm.linear <- svm.linear.giuste/nrow(breast.test)
accuratezza.svm.polinomial <- svm.linear.polinomial/nrow(breast.test)
accuratezza.svm.sigmoid <- svm.giuste.sigmoid/nrow(breast.test)
accuratezza.svm
accuratezza.svm.linear
accuratezza.svm.polinomial
accuratezza.svm.sigmoid

#è meglio radiale e lineare

#knn
library(class)
breast.knn <- knn(breast.training[complete.cases(breast.training),-ncol(breast.training)],breast.test[complete.cases(breast.test),-(ncol(breast.test))], cl= breast.training[complete.cases(breast.training),ncol(breast.training)],k=10)
breast.knn
#da rivedere e rifare

#caret
#non mi carica klaR


#regressione generica
fit.cars <- lm(dist ~ speed, data = cars)
fit.cars.all <- lm(dist~.,data = cars)

#Visualizziamo i coefficienti della regressione
fit.cars
#Informazioni più dettagliate possono essere ottenuto con la funzione summary
#da cui è possibile capire se il fit è significativo o no
summary(fit.cars)

#Plottiamo i risultati della regressione
plot(cars$speed,cars$dist,pch=21,bg="red")
abline(fit.cars)
