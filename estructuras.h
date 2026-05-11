#include <stdbool.h>
#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <math.h>

#define MAX_CALLES      50
#define MAX_VERTICES    500
#define MAX_ARISTAS     2000
#define MAX_NOMBRE      64
#define MAX_TURISTICOS  100
#define EPSILON         1e-6

typedef struct {
    char   nombre[MAX_NOMBRE];
    double x0, y0;   /* inicio */
    double x1, y1;   /* fin    */
    char   eje;      /* 'X' o 'Y' */
} Calle;

typedef struct {
    double x, y;
    bool   es_turistico;    /* 1 si es punto de interes, 0 si es interseccion */
    int    idx_turistico;   /* indice en arreglo de turisticos, -1 si no aplica */
} Vertice;

typedef struct {
    char   nombre[MAX_NOMBRE];
    char   calle[MAX_NOMBRE];
    double posicion;      /* coordenada segun el eje de la calle */
    int    vertice_id;    /* id del Vertice correspondiente en el grafo */
    int    visitado;
} PuntoTuristico;

typedef struct Arista {
    int           destino;
    struct Arista *sig;
} Arista;

typedef struct {
    Vertice  Vertices[MAX_VERTICES];
    Arista  *adyacencia[MAX_VERTICES];
    int      num_Vertices;
} Grafo;

typedef struct {
    Calle          calles[MAX_CALLES];
    int            num_calles;
    PuntoTuristico turisticos[MAX_TURISTICOS];
    int            num_turisticos;
    Grafo          grafo;
} Ciudad;

/* Calcula distancia euclidiana entre dos vertices */
static inline double calcular_distancia(Vertice a, Vertice b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx*dx + dy*dy);
}

#endif