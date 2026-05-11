# ============================================================
#  Makefile - Mapa Turistico
#  Compilar: make
#  Ejecutar: make run
#  Limpiar:  make clean
# ============================================================

CC     = gcc
CFLAGS = -Wall -std=c11
LIBS   = -lm
SRCS   = main.c leer_archivo.c construir_grafo.c dijkstra.c

# --- Detectar sistema operativo ---
ifeq ($(OS), Windows_NT)
    TARGET = mapa_turistico.exe
    RM     = del /Q
else
    TARGET = mapa_turistico.out
    RM     = rm -f
endif

# --- Regla principal (sin .o) ---
$(TARGET): $(SRCS) estructuras.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# --- Ejecutar ---
run: $(TARGET)
	./$(TARGET)

# --- Limpiar ---
clean:
	$(RM) $(TARGET)

.PHONY: run clean
