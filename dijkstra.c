#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "estructuras.h"
 
#define INF 1e30
 
double dijkstra(Ciudad *ciudad, int inicio_id, int fin_id, int *camino, int *largo_camino) {
    int n = ciudad->grafo.num_Vertices;
    double distancia[MAX_VERTICES];
    int    previo[MAX_VERTICES];
    bool   visitado[MAX_VERTICES];

    for (int i = 0; i < n; i++) {
        distancia[i] = INF;
        previo[i]    = -1;
        visitado[i]  = false;
    }
    distancia[inicio_id] = 0.0;

    for (int iter = 0; iter < n; iter++) {
        int u = -1;
        for (int j = 0; j < n; j++) {
            if (!visitado[j] && (u == -1 || distancia[j] < distancia[u]))
                u = j;
        }
        if (u == -1 || distancia[u] >= INF) break;
        visitado[u] = true;
        if (u == fin_id) break;

        Arista *a = ciudad->grafo.adyacencia[u];
        while (a != NULL) {
            int v = a->destino;
            double peso = calcular_distancia(ciudad->grafo.Vertices[u],
                                             ciudad->grafo.Vertices[v]);
            if (distancia[u] + peso < distancia[v]) {
                distancia[v] = distancia[u] + peso;
                previo[v]    = u;
            }
            a = a->sig;
        }
    }

    if (distancia[fin_id] >= INF) {
        *largo_camino = 0;
        return -1.0;
    }

    int tmp[MAX_VERTICES];
    int len = 0;
    for (int cur = fin_id; cur != -1; cur = previo[cur])
        tmp[len++] = cur;

    *largo_camino = len;
    for (int k = 0; k < len; k++)
        camino[k] = tmp[len - 1 - k];

    return distancia[fin_id];
}


