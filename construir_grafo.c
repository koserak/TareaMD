/*
 * construir_grafo.c
 * Genera los vertices (intersecciones + puntos turisticos) y las aristas
 * del grafo a partir de las calles leidas del archivo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "estructuras.h"

/* ---------------------------------------------------------------
 * punto_en_segmento: devuelve 1 si el punto (px,py) esta sobre el
 * segmento (x0,y0)-(x1,y1), con tolerancia EPSILON.
 * --------------------------------------------------------------- */
static int punto_en_segmento(double px, double py,
                              double x0, double y0,
                              double x1, double y1) {
    double minx = x0 < x1 ? x0 : x1;
    double maxx = x0 > x1 ? x0 : x1;
    double miny = y0 < y1 ? y0 : y1;
    double maxy = y0 > y1 ? y0 : y1;
    return (px >= minx - EPSILON && px <= maxx + EPSILON &&
            py >= miny - EPSILON && py <= maxy + EPSILON);
}

/* ---------------------------------------------------------------
 * agregar_vertice: busca si ya existe un vertice en (x,y); si no,
 * lo crea. Devuelve el id del vertice.
 * --------------------------------------------------------------- */
static int agregar_vertice(Grafo *g, double x, double y) {
    for (int i = 0; i < g->num_Vertices; i++) {
        if (fabs(g->Vertices[i].x - x) < EPSILON &&
            fabs(g->Vertices[i].y - y) < EPSILON)
            return i;
    }
    if (g->num_Vertices >= MAX_VERTICES) {
        fprintf(stderr, "Error: demasiados vertices\n");
        return -1;
    }
    int id = g->num_Vertices++;
    g->Vertices[id].x = x;
    g->Vertices[id].y = y;
    g->Vertices[id].es_turistico = false;
    g->Vertices[id].idx_turistico = -1;
    g->adyacencia[id] = NULL;
    return id;
}

/* ---------------------------------------------------------------
 * agregar_arista: agrega arista no dirigida entre u y v si no existe.
 * --------------------------------------------------------------- */
static void agregar_arista(Grafo *g, int u, int v) {
    if (u == v) return;
    /* Verificar que no existe ya */
    Arista *a = g->adyacencia[u];
    while (a) {
        if (a->destino == v) return;
        a = a->sig;
    }
    Arista *nueva = (Arista *)malloc(sizeof(Arista));
    nueva->destino = v;
    nueva->sig = g->adyacencia[u];
    g->adyacencia[u] = nueva;

    nueva = (Arista *)malloc(sizeof(Arista));
    nueva->destino = u;
    nueva->sig = g->adyacencia[v];
    g->adyacencia[v] = nueva;
}

/* ---------------------------------------------------------------
 * comparar_double: comparador para qsort de posiciones sobre calle.
 * --------------------------------------------------------------- */
typedef struct { double pos; int id; } PosId;

static int cmp_posid(const void *a, const void *b) {
    double da = ((PosId*)a)->pos;
    double db = ((PosId*)b)->pos;
    return (da > db) - (da < db);
}

/* ---------------------------------------------------------------
 * construir_grafo: funcion principal.
 * Para cada calle:
 *   1. Calcula coordenada del punto turistico (si hay alguno en ella).
 *   2. Calcula intersecciones con todas las demas calles.
 *   3. Agrega todos los vertices sobre la calle.
 *   4. Los ordena por posicion y los une con aristas consecutivas.
 * --------------------------------------------------------------- */
void construir_grafo(Ciudad *ciudad) {
    Grafo *g = &ciudad->grafo;
    g->num_Vertices = 0;
    for (int i = 0; i < MAX_VERTICES; i++)
        g->adyacencia[i] = NULL;

    /* Para cada calle coleccionamos los vertices que caen sobre ella */
    for (int i = 0; i < ciudad->num_calles; i++) {
        Calle *ci = &ciudad->calles[i];
        PosId sobre[MAX_VERTICES];
        int n_sobre = 0;

        /* Extremos de la calle como vertices */
        {
            int id0 = agregar_vertice(g, ci->x0, ci->y0);
            double pos0 = (ci->eje == 'X') ? ci->x0 : ci->y0;
            sobre[n_sobre++] = (PosId){pos0, id0};

            int id1 = agregar_vertice(g, ci->x1, ci->y1);
            double pos1 = (ci->eje == 'X') ? ci->x1 : ci->y1;
            sobre[n_sobre++] = (PosId){pos1, id1};
        }

        /* Intersecciones con otras calles */
        for (int j = 0; j < ciudad->num_calles; j++) {
            if (i == j) continue;
            Calle *cj = &ciudad->calles[j];

            double px = -1, py = -1;
            int hay_interseccion = 0;

            /*
             * Caso 1: ci es horizontal (eje X) y cj es vertical (eje Y).
             * La interseccion es (cj->x0, ci->y0).
             */
            if (ci->eje == 'X' && cj->eje == 'Y') {
                px = cj->x0;
                py = ci->y0;
                hay_interseccion = 1;
            }
            /*
             * Caso 2: ci es vertical (eje Y) y cj es horizontal (eje X).
             */
            else if (ci->eje == 'Y' && cj->eje == 'X') {
                px = ci->x0;
                py = cj->y0;
                hay_interseccion = 1;
            }
            /*
             * Caso 3: ambas tienen eje X (pueden ser la diagonal vs horizontal).
             * Usamos interseccion de rectas parametricas.
             */
            else {
                /* Resolvemos sistema: ci(t) = cj(s) */
                double dx1 = ci->x1 - ci->x0, dy1 = ci->y1 - ci->y0;
                double dx2 = cj->x1 - cj->x0, dy2 = cj->y1 - cj->y0;
                double denom = dx1*dy2 - dy1*dx2;
                if (fabs(denom) > EPSILON) {
                    double t = ((cj->x0 - ci->x0)*dy2 - (cj->y0 - ci->y0)*dx2) / denom;
                    px = ci->x0 + t * dx1;
                    py = ci->y0 + t * dy1;
                    hay_interseccion = 1;
                }
            }

            if (hay_interseccion &&
                punto_en_segmento(px, py, ci->x0, ci->y0, ci->x1, ci->y1) &&
                punto_en_segmento(px, py, cj->x0, cj->y0, cj->x1, cj->y1)) {
                int id = agregar_vertice(g, px, py);
                double pos = (ci->eje == 'X') ? px : py;
                sobre[n_sobre++] = (PosId){pos, id};
            }
        }

        /* Puntos turisticos sobre esta calle */
        for (int t = 0; t < ciudad->num_turisticos; t++) {
            PuntoTuristico *pt = &ciudad->turisticos[t];
            if (strcmp(pt->calle, ci->nombre) != 0) continue;

            double px, py;
            if (ci->eje == 'X') {
                /* posicion es coordenada X; Y es constante */
                px = pt->posicion;
                py = ci->y0;  /* calle horizontal: y0==y1 */
            } else {
                /* posicion es coordenada Y; X es constante */
                px = ci->x0;  /* calle vertical: x0==x1 */
                py = pt->posicion;
            }

            /* Verificar que cae dentro del segmento */
            if (!punto_en_segmento(px, py, ci->x0, ci->y0, ci->x1, ci->y1)) {
                fprintf(stderr, "Advertencia: punto turistico '%s' fuera de calle '%s'\n",
                        pt->nombre, ci->nombre);
                continue;
            }

            int id = agregar_vertice(g, px, py);
            g->Vertices[id].es_turistico = true;
            g->Vertices[id].idx_turistico = t;
            pt->vertice_id = id;

            double pos = pt->posicion;
            sobre[n_sobre++] = (PosId){pos, id};
        }

        /* Ordenar por posicion y conectar consecutivos */
        qsort(sobre, n_sobre, sizeof(PosId), cmp_posid);
        for (int k = 0; k + 1 < n_sobre; k++)
            agregar_arista(g, sobre[k].id, sobre[k+1].id);
    }
}