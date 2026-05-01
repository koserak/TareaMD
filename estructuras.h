#include <stdbool.h>
#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#define MAX_CALLES 50
#define MAX_NODOS 500
#define MAX_ARISTAS  2000
#define MAX_NOMBRE      64
#define MAX_TURISTICOS  100
#define EPSILON         1e-6

typedef struct {
    char   nombre[MAX_NOMBRE];
    double x0, y0;   /* inicio */
    double x1, y1;   /* fin    */
    char   eje;      // 'X' O 'Y'
} Calle;

typedef struct {
    double x, y;
    bool    es_turistico;   /* 1 si es punto de interés, 0 si es intersección */
    int    idx_turistico;  /* índice en arreglo de turísticos, -1 si no aplica */
} Nodo;

typedef struct {
    char   nombre[MAX_NOMBRE];
    char   calle[MAX_NOMBRE];
    double posicion;   /* coordenada según el eje de la calle */
    int    nodo_id;    /* id del nodo correspondiente en el grafo */
    int    visitado;
} PuntoTuristico;

typedef struct {
    Nodo   nodos[MAX_NODOS];
    Arista *adyacencia[MAX_NODOS];
    int    num_nodos;
} Grafo;

typedef struct {
    Calle     calles[MAX_CALLES];
    int       num_calles;
    PuntoTuristico turisticos[MAX_TURISTICOS];
    int       num_turisticos;
    Grafo     grafo;
} Ciudad;

typedef struct Arista {
    int           destino;
    struct Arista *sig;
} Arista;





#endif