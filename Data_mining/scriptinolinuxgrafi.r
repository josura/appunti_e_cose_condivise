library(igraph)

plot(graph(edges = c(1,2,  3,4,  5,6)))

#per prendere gli edges
E(make_undirected_graph(edges = c(1,2  , 321,45,   33,33)))
#per prendere i vertex
V(make_undirected_graph(edges = c(1,2  , 321,45,   33,33)))

prova <- read_graph(file = "/home/josura/Desktop/universita/appunti_e_cose_condivise/Data_mining/prova.txt",format = "edgelist")

write_graph(prova,file = "/home/josura/Desktop/universita/appunti_e_cose_condivise/Data_mining/prova2.txt",format = "edgelist")

read.table("/home/josura/Desktop/universita/appunti_e_cose_condivise/Data_mining/prova.txt",sep = " ")
graph_from_data_frame()

#per convertirli come lista o adj matrix
as_adj_list(prova)
as_adjacency_matrix(prova)

#per vedere coefficente di clustering locale e globale(per i nodi e la media)
transitivity(prova,type = "local")
transitivity(prova,type = "global")
#per il coefficente usando i triangoli
triad_census(prova)

diameter(graph = prova)
diameter(graph = prova,weights = c(1,2,3,4))
distances(prova)
mean_distance(prova)
#solo un cammino o tutti i cammini minimi
shortest.paths(prova)
res <- all_shortest_paths(prova,from = 2)
res$res[[4]]
#per ricordarti che la singola parentesi quadra ti restituisce un elemento lista
class(res$res[[4]])
class(res$res[4])

#gradi dei nodi, restituisce un vettore di gradi per i nodi
degree(prova)
#distribuzione dei gradi
?degree.distribution
plot(0:(max(degree(prova))),degree.distribution(prova))  #log="xy" per la scala logaritmica su tutti e due gli assi


# archi incidenti al nodo 1, vettore di edges o lista 
incident(prova,2)
incident_edges(prova,2)
#archi adiacenti
neighborhood(prova)
neighbors(prova,2)
#adiacenti in un sottografo
neighbors(prova,c(2,3,4))   //non funzia, prende solo il primo
adjacent_vertices(prova,c(2,3,4))

#generalizzazione di neighbors, con distanza, intorno di raggio order
ego(prova,order = 2, nodes = 2)


#clique(componenti complete) del grafo
cliques(prova,min=3)


#metodi di centralità
centr_degree(prova)
centr_betw(prova)
