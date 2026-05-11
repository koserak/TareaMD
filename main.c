/*
 * main.c
 * Programa principal: lee el archivo, construye el grafo, busca rutas
 * entre puntos turisticos y muestra las instrucciones al usuario.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "estructuras.h"

typedef enum {
    DIR_RECTO,
    DIR_IZQUIERDA,
    DIR_DERECHA,
    DIR_MEDIA_VUELTA
} Direccion;

static Direccion calcular_giro(Vertice A, Vertice B, Vertice C) {
    double ax = B.x - A.x, ay = B.y - A.y;
    double bx = C.x - B.x, by = C.y - B.y;
    double cross = ax * by - ay * bx;
    double dot   = ax * bx + ay * by;

    if (fabs(cross) < EPSILON && dot < 0)
        return DIR_MEDIA_VUELTA;
    if (fabs(cross) < EPSILON)
        return DIR_RECTO;
    return (cross > 0) ? DIR_IZQUIERDA : DIR_DERECHA;
}

/* Prototipos de funciones en otros modulos */
int    leer_archivo(char *nombre, Ciudad *ciudad);
void   construir_grafo(Ciudad *ciudad);
double dijkstra(Ciudad *ciudad, int inicio_id, int fin_id,
                int *camino, int *largo_camino);

/* ---------------------------------------------------------------
 * imprimir_camino: muestra el recorrido nodo a nodo con coordenadas.
 * Ademas, si el camino pasa por un punto turistico aun no visitado,
 * lo marca como visitado.
 * --------------------------------------------------------------- */
static void imprimir_camino(Ciudad *ciudad, int *camino, int largo) {
    Grafo *g = &ciudad->grafo;
    for (int k = 0; k < largo; k++) {
        int id = camino[k];
        Vertice *v = &g->Vertices[id];
        if (k > 0 && k < largo - 1) {
            Vertice A = g->Vertices[camino[k-1]];
            Vertice B = g->Vertices[camino[k]];
            Vertice C = g->Vertices[camino[k+1]];
            Direccion d = calcular_giro(A, B, C);

            switch (d) {
                case DIR_RECTO:       printf("  | Continua recto\n");     break;
                case DIR_IZQUIERDA:   printf("  | Dobla a la izquierda\n"); break;
                case DIR_DERECHA:     printf("  | Dobla a la derecha\n");  break;
                case DIR_MEDIA_VUELTA:printf("  | Da media vuelta\n");     break;
            }
        }

        if (v->es_turistico) {
            int t = v->idx_turistico;
            printf("  Nodo %3d  (%.1f, %.1f)  <-- %s\n",
                   id, v->x, v->y, ciudad->turisticos[t].nombre);
            /* Marcar como visitado si no lo estaba */
            if (!ciudad->turisticos[t].visitado) {
                ciudad->turisticos[t].visitado = 1;
                printf("             [Punto turistico '%s' visitado!]\n",
                       ciudad->turisticos[t].nombre);
            }
        } else {
            printf("  Nodo %3d  (%.1f, %.1f)\n", id, v->x, v->y);
        }
    }
}

/* ---------------------------------------------------------------
 * buscar_rutas: recorre todos los puntos turisticos en orden y busca
 * ruta del ultimo visitado al siguiente no visitado.
 * --------------------------------------------------------------- */
static void buscar_rutas(Ciudad *ciudad) {
    int n_t = ciudad->num_turisticos;

    /* Reiniciar estado de visitas */
    for (int i = 0; i < n_t; i++)
        ciudad->turisticos[i].visitado = 0;

    int camino[MAX_VERTICES];
    int largo;
    double dist_total = 0.0;

    /* Punto de partida: primer turistico */
    ciudad->turisticos[0].visitado = 1;
    printf("\nInicio en: %s (%.1f, %.1f)\n",
           ciudad->turisticos[0].nombre,
           ciudad->grafo.Vertices[ciudad->turisticos[0].vertice_id].x,
           ciudad->grafo.Vertices[ciudad->turisticos[0].vertice_id].y);

    int origen = ciudad->turisticos[0].vertice_id;

    for (int i = 1; i < n_t; i++) {
        if (ciudad->turisticos[i].visitado) continue;

        int destino = ciudad->turisticos[i].vertice_id;
        printf("\n--- Ruta de '%s' a '%s' ---\n",
               ciudad->turisticos[i - 1].nombre,   /* ultimo visitado */
               ciudad->turisticos[i].nombre);

        double d = dijkstra(ciudad, origen, destino, camino, &largo);

        if (d < 0) {
            printf("  ERROR: No existe ruta entre '%s' y '%s'.\n",
                   ciudad->turisticos[i-1].nombre,
                   ciudad->turisticos[i].nombre);
            /* Avanzar de todas formas para intentar los siguientes */
        } else {
            printf("  Distancia: %.2f unidades\n", d);
            dist_total += d;
            imprimir_camino(ciudad, camino, largo);
            origen = destino;
        }

        /* Marcar destino como visitado aunque no haya ruta */
        ciudad->turisticos[i].visitado = 1;
    }

    printf("\n=== Recorrido completado. Distancia total: %.2f unidades ===\n",
           dist_total);
}

/* ---------------------------------------------------------------
 * preguntar_si_no: pide al usuario S/N. Reintenta si la entrada
 * no es valida.
 * --------------------------------------------------------------- */
static int preguntar_si_no(const char *mensaje) {
    char buf[64];
    while (1) {
        printf("%s (S/N): ", mensaje);
        if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
        /* Buscar primer caracter no-espacio */
        int i = 0;
        while (buf[i] && isspace((unsigned char)buf[i])) i++;
        char c = toupper((unsigned char)buf[i]);
        if (c == 'S') return 1;
        if (c == 'N') return 0;
        printf("  Respuesta invalida. Por favor ingrese 'S' o 'N'.\n");
    }
}

/* ---------------------------------------------------------------
 * inicializar_ciudad: pone a cero los contadores de la estructura.
 * --------------------------------------------------------------- */
static void inicializar_ciudad(Ciudad *ciudad) {
    ciudad->num_calles     = 0;
    ciudad->num_turisticos = 0;
    ciudad->grafo.num_Vertices = 0;
    for (int i = 0; i < MAX_VERTICES; i++)
        ciudad->grafo.adyacencia[i] = NULL;
}

/* ---------------------------------------------------------------
 * liberar_grafo: libera la memoria de las listas de adyacencia.
 * --------------------------------------------------------------- */
static void liberar_grafo(Ciudad *ciudad) {
    for (int i = 0; i < ciudad->grafo.num_Vertices; i++) {
        Arista *a = ciudad->grafo.adyacencia[i];
        while (a) {
            Arista *sig = a->sig;
            free(a);
            a = sig;
        }
        ciudad->grafo.adyacencia[i] = NULL;
    }
}

/* ---------------------------------------------------------------
 * main
 * --------------------------------------------------------------- */
int main(void) {
    Ciudad ciudad;
    char nombre[256];

    printf("=== Mapa Turistico ===\n");

    do {
        inicializar_ciudad(&ciudad);

        /* Pedir nombre de archivo (reintentar si no se puede abrir) */
        while (1) {
            printf("\nIngrese el nombre del archivo: ");
            if (fgets(nombre, sizeof(nombre), stdin) == NULL) {
                printf("Error al leer entrada. Saliendo.\n");
                return 1;
            }
            /* Eliminar salto de linea */
            nombre[strcspn(nombre, "\n")] = '\0';
            if (strlen(nombre) == 0) {
                printf("  El nombre no puede estar vacio.\n");
                continue;
            }
            if (leer_archivo(nombre, &ciudad)) break;
            printf("  No se pudo abrir '%s'. Intente de nuevo.\n", nombre);
        }

        /* Construir el grafo */
        construir_grafo(&ciudad);
        printf("\nGrafo construido: %d vertices.\n", ciudad.grafo.num_Vertices);

        /* Verificar que todos los puntos turisticos tienen vertice asignado */
        int ok = 1;
        for (int i = 0; i < ciudad.num_turisticos; i++) {
            if (ciudad.turisticos[i].vertice_id == -1) {
                printf("Advertencia: el punto '%s' no pudo ubicarse en el grafo.\n",
                       ciudad.turisticos[i].nombre);
                ok = 0;
            }
        }

        if (ok && ciudad.num_turisticos > 0) {
            buscar_rutas(&ciudad);
        } else if (ciudad.num_turisticos == 0) {
            printf("No hay puntos turisticos en el archivo.\n");
        }

        liberar_grafo(&ciudad);

    } while (preguntar_si_no("\n¿Desea leer otro archivo?"));

    printf("\n¡Hasta luego! Esperamos que haya disfrutado el recorrido.\n");
    return 0;
}