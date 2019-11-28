plot(
  x=1:10,
  y = 1:10
)
list(x=10,y=20)

install.packages("cluster")
install.packages("cluster.datasets")
install.packages("dbscan")

library(cluster.datasets)
data("all.mammals.milk.1956")
head(all.mammals.milk.1956)  #all mammals milk first 10 records
input <- all.mammals.milk.1956[,2:6]
dist()      #per la matrice delle distanze
cluster.hclust=hclust(dist(input)) #clustering gerarchico, presente già in stats come libreria, primo parametro è la matrice delle distanze, il secondo è il tipo di distanza, cioe completa, sinistra e destra? non mi  ricordo
plot(cluster.hclust)  #dendrogramma
plot(cluster.hclust,labels = all.mammals.milk.1956[,1])  #dendrogramma con labels
cutree(cluster.hclust,k=3)
cluster.dendrogram <-  cutree(cluster.hclust,k=3)
cluster.kmeans <- kmeans(input, centers = 3, nstart = 20)   #centers è il numero di cluster o una matrice di centroidi

wss<-c() #within sum of squares vector for past values of k
nc<-20  #number of trials for every kmeans
for(k in 1:nc)
{
  set.seed(123)
  wss[k]<-sum(kmeans(input,centers = k)$withinss)
}

plot(1:20,wss,ylab = "withinss",xlab = "k",type = "b")

library(cluster)
cluster.dbscan <- dbscan(input,minPts = 6,eps = 10)
cluster.dbscan$cluster

kNNdist(input,k=6)
kNNdistplot(input,k=6)
abline(h = 7) 

cluster.dbscan <- dbscan(input,minPts = 6,eps = 7)  #ripetiamo con 7 come distanza dato che abbiamo visto dal grafico che la curva non cambia molto dopo 7
cluster.dbscan

library(cluster)
clusplot(input,cluster.dendrogram,color=TRUE,lines=1)     #disegna un'ellisse attorno ai nostri cluster, da hclust con 3 cluster,dbscan e kmeans
clusplot(input,cluster.dbscan$cluster,color=TRUE,lines=1)
clusplot(input,cluster.kmeans$cluster,color=TRUE,lines=1)
clusplot(input,cutree(cluster.hclust,4),color=TRUE,lines=1)
clusplot(input,cutree(cluster.hclust,5),color=TRUE,lines=1)
clusplot(input,cutree(cluster.hclust,6),color=TRUE,lines=1)


silhouette(cluster.kmeans$cluster,dist(input))
plot(silhouette(cluster.kmeans$cluster,dist(input)))
plot(silhouette(cluster.kmeans$cluster,dist(input)),col = c("red","green","blue"))

describe(all.mammals.milk.1956)


library(dplyr)  #non mi funziona rip
cluster.hier <- mtcars %<% 
  dist %<% 
  hclust
