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
 * calcular_pos: proyeccion del punto (px,py) sobre la calle ci,
 * devuelve distancia desde el inicio del segmento.
 * Funciona para calles horizontales, verticales y diagonales.
 * --------------------------------------------------------------- */
static double calcular_pos(Calle *ci, double px, double py) {
    double dx = ci->x1 - ci->x0;
    double dy = ci->y1 - ci->y0;
    double len = sqrt(dx*dx + dy*dy);
    if (len < EPSILON) return 0.0;
    return ((px - ci->x0)*dx + (py - ci->y0)*dy) / len;
}

/* ---------------------------------------------------------------
 * agregar_vertice: busca si ya existe un vertice en (x,y); si no,
 * lo crea. Normaliza coordenadas para evitar duplicados por error
 * de punto flotante. Devuelve el id del vertice.
 * --------------------------------------------------------------- */
static int agregar_vertice(Grafo *g, double x, double y) {
    /* Normalizar para evitar duplicados por error de punto flotante */
    x = round(x * 1e6) / 1e6;
    y = round(y * 1e6) / 1e6;

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
    g->Vertices[id].num_turisticos = 0;
    g->adyacencia[id] = NULL;
    return id;
}

/* ---------------------------------------------------------------
 * agregar_arista: agrega arista no dirigida entre u y v si no existe.
 * --------------------------------------------------------------- */
static void agregar_arista(Grafo *g, int u, int v) {
    if (u == v) return;
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

    for (int i = 0; i < ciudad->num_calles; i++) {
        Calle *ci = &ciudad->calles[i];
        PosId sobre[MAX_VERTICES];
        int n_sobre = 0;

        /* Extremos de la calle como vertices */
        {
            int id0 = agregar_vertice(g, ci->x0, ci->y0);
            sobre[n_sobre++] = (PosId){ calcular_pos(ci, ci->x0, ci->y0), id0 };

            int id1 = agregar_vertice(g, ci->x1, ci->y1);
            sobre[n_sobre++] = (PosId){ calcular_pos(ci, ci->x1, ci->y1), id1 };
        }

        /* Intersecciones con otras calles */
        for (int j = 0; j < ciudad->num_calles; j++) {
            if (i == j) continue;
            Calle *cj = &ciudad->calles[j];

            double px = -1, py = -1;
            int hay_interseccion = 0;

            /* Detectar tipo de calle por geometria, no por campo eje,
             * para soportar diagonales marcadas como 'X' o 'Y' en el archivo. */
            int ci_horiz = fabs(ci->y1 - ci->y0) < EPSILON;
            int ci_vert  = fabs(ci->x1 - ci->x0) < EPSILON;
            int cj_horiz = fabs(cj->y1 - cj->y0) < EPSILON;
            int cj_vert  = fabs(cj->x1 - cj->x0) < EPSILON;

            if (ci_horiz && cj_vert) {
                px = cj->x0;
                py = ci->y0;
                hay_interseccion = 1;
            }
            else if (ci_vert && cj_horiz) {
                px = ci->x0;
                py = cj->y0;
                hay_interseccion = 1;
            }
            else {
                /* Cualquier otro caso (diagonal vs cualquiera): parametrico */
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

                /* Normalizar antes de insertar */
                px = round(px * 1e6) / 1e6;
                py = round(py * 1e6) / 1e6;

                int id = agregar_vertice(g, px, py);
                sobre[n_sobre++] = (PosId){ calcular_pos(ci, px, py), id };
            }
        }

        /* Puntos turisticos sobre esta calle */
        for (int t = 0; t < ciudad->num_turisticos; t++) {
            PuntoTuristico *pt = &ciudad->turisticos[t];
            if (strcmp(pt->calle, ci->nombre) != 0) continue;

            double px, py;

            /* Calcular coordenadas del punto turistico segun geometria real de la calle.
             * Se detecta por coordenadas, no por el campo eje, para soportar diagonales
             * que en el archivo vengan marcadas como 'X' o 'Y'. */
            if (fabs(ci->y1 - ci->y0) < EPSILON) {
                /* Horizontal real: Y es constante, posicion es coordenada X */
                px = pt->posicion;
                py = ci->y0;
            } else if (fabs(ci->x1 - ci->x0) < EPSILON) {
                /* Vertical real: X es constante, posicion es coordenada Y */
                px = ci->x0;
                py = pt->posicion;
            } else {
                /* Diagonal: x e y aumentan juntas, posicion vale para ambas */
                px = pt->posicion;
                py = pt->posicion;
            }

            if (!punto_en_segmento(px, py, ci->x0, ci->y0, ci->x1, ci->y1)) {
                fprintf(stderr, "Advertencia: punto turistico '%s' fuera de calle '%s'\n",
                        pt->nombre, ci->nombre);
                continue;
            }

            /* Normalizar antes de insertar */
            px = round(px * 1e6) / 1e6;
            py = round(py * 1e6) / 1e6;

            int id = agregar_vertice(g, px, py);
            Vertice *v = &g->Vertices[id];
            v->es_turistico = true;

            int existe = 0;
            for (int k = 0; k < v->num_turisticos; k++) {
                if (v->idx_turisticos[k] == t) {
                    existe = 1;
                    break;
                }
            }
            if (!existe && v->num_turisticos < MAX_TURISTICOS) {
                v->idx_turisticos[v->num_turisticos] = t;
                v->num_turisticos++;
            }

            pt->vertice_id = id;
            sobre[n_sobre++] = (PosId){ calcular_pos(ci, px, py), id };
        }

        /* Ordenar por posicion y conectar consecutivos */
        qsort(sobre, n_sobre, sizeof(PosId), cmp_posid);
        for (int k = 0; k + 1 < n_sobre; k++)
            agregar_arista(g, sobre[k].id, sobre[k+1].id);
    }
}