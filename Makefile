# ============================================================
#  Makefile - Mapa Turistico
#  Compilar: make
#  Ejecutar: make run
#  Limpiar:  make clean
# ============================================================

CC      = gcc
CFLAGS  = -Wall -std=c11
LIBS    = -lm
TARGET  = mapa_turistico
SRCS    = main.c leer_archivo.c construir_grafo.c dijkstra.c
OBJS    = $(SRCS:.c=.o)

# --- Regla principal ---
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# --- Compilar cada .c a .o ---
%.o: %.c estructuras.h
	$(CC) $(CFLAGS) -c $< -o $@

# --- Ejecutar ---
run: $(TARGET)
	./$(TARGET)

# --- Limpiar archivos generados ---
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: run clean
