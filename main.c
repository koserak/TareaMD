#include <stdio.h>
#include "estructuras.h"
int leer_archivo(char *nombre, Ciudad *ciudad) {
    FILE *f = fopen(nombre, "r");
    if (!f) {
        printf("Error: no se pudo abrir '%s'\n", nombre);
        return 0;
    }

    fscanf(f, "%d", &ciudad->num_calles);
    for (int i = 0; i < ciudad->num_calles; i++) {
        fscanf(f, "%s %lf %lf %lf %lf %c",
               ciudad->calles[i].nombre,
               &ciudad->calles[i].x0, &ciudad->calles[i].y0,
               &ciudad->calles[i].x1, &ciudad->calles[i].y1,
               &ciudad->calles[i].eje);
    }

    fscanf(f, "%d", &ciudad->num_turisticos);
    for (int i = 0; i < ciudad->num_turisticos; i++) {
        fscanf(f, "%s %s %lf",
               ciudad->turisticos[i].nombre,
               ciudad->turisticos[i].calle,
               &ciudad->turisticos[i].posicion);
        ciudad->turisticos[i].visitado = 0;
        ciudad->turisticos[i].nodo_id = -1;
    }
    //printf("se escaneo '%s'",nombre);
    
    fclose(f);
    return 1;
}
int main(){
char nombre[256];
Ciudad ciudad;
printf("Ingrese el nombre del archivo: ");
scanf("%255s", nombre);
leer_archivo(nombre, &ciudad);
/*printf("Calles (%d):\n", ciudad.num_calles);
    for (int i = 0; i < ciudad.num_calles; i++)
        printf("  %s (%.0f,%.0f)-(%.0f,%.0f) eje:%c\n",
               ciudad.calles[i].nombre,
               ciudad.calles[i].x0, ciudad.calles[i].y0,
               ciudad.calles[i].x1, ciudad.calles[i].y1,
               ciudad.calles[i].eje);

    // verificar turísticos
    printf("Turisticos (%d):\n", ciudad.num_turisticos);
    for (int i = 0; i < ciudad.num_turisticos; i++)
        printf("  %s en %s pos=%.0f\n",
               ciudad.turisticos[i].nombre,
               ciudad.turisticos[i].calle,
               ciudad.turisticos[i].posicion);*/
}