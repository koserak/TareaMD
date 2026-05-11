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
#define INF 1e30

// Direcciones
typedef enum {
    DIR_RECTO,
    DIR_IZQUIERDA,
    DIR_DERECHA,
    DIR_MEDIA_VUELTA
} Direccion;

// Struct para Calle

typedef struct {
    char   nombre[MAX_NOMBRE];
    double x0, y0;   /* inicio */
    double x1, y1;   /* fin    */
    char   eje;      /* 'X', 'Y', 'D'  */
} Calle;

// Struct para Vertices (nodos)

typedef struct {
    double x, y;
    bool   es_turistico;    /* 1 si es punto de interes, 0 si es interseccion */
    int idx_turisticos[MAX_TURISTICOS];
    int num_turisticos;   /* indice en arreglo de turisticos, -1 si no aplica */
} Vertice;

// Struct para almacenar los puntos turisticos.
typedef struct {
    char   nombre[MAX_NOMBRE];
    char   calle[MAX_NOMBRE];
    double posicion;      /* coordenada segun el eje de la calle */
    int    vertice_id;    /* id del Vertice correspondiente en el grafo */
    int    visitado;
} PuntoTuristico;

// Struct para modelar las Aristas

typedef struct Arista {
    int           destino;
    struct Arista *sig;
} Arista;

// Struct para modelar el grafo

typedef struct {
    Vertice  Vertices[MAX_VERTICES];
    Arista  *adyacencia[MAX_VERTICES];
    int      num_Vertices;
} Grafo;

// Struct para modelar la Ciudad

typedef struct {
    Calle          calles[MAX_CALLES];
    int            num_calles;
    PuntoTuristico turisticos[MAX_TURISTICOS];
    int            num_turisticos;
    Grafo          grafo;
} Ciudad;

// Calcula la distancia entre dos vertices
static inline double calcular_distancia(Vertice a, Vertice b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx*dx + dy*dy);
}

#endif