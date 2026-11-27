#include "constantes.h"
#include <cctype>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum estado_palabra { ENCONTRADA, NO_ENCONTRADA };

typedef struct sopa {

  char tablero[TAM][TAM];
  int lleno;

} Sopa;

typedef struct palabra {

  char texto[MAX_LEN];
  int xi, yi; // posicion inicial
  int xf, yf; // posicion final
  int encontrada;

} Palabra;

typedef struct jugador {

  char nombre[MAX_LEN];
  int tiempo;
  int palabras_encontradas;
} Jugador;

char letraAleatoria(void);
void mostrarTablero(Sopa *sopa);
void llenarTablero(Sopa *sopa);
int seleccionarPalabras(Palabra palabras[]);
int colocarPalabra(Sopa *sopa, Palabra *p, int idx);
void limpiarTablero(Sopa *sopa);

int main(void) {
  srand(time(NULL));
  int opt;
  Palabra palabras[NUM_PALABRAS];
  Sopa sopa;

  do {

    printf("==== Sopa de Letras ====\n"
           "[1] Jugar\n"
           "[2] Creditos\n"
           "[3] High Score\n"
           "[4] Anadir Palabras\n"
           "[5] Salir\n");
    scanf("%d", &opt);

    switch (opt) {

    case 1:
      seleccionarPalabras(palabras);
      limpiarTablero(&sopa);

      for (int i = 0; i < NUM_PALABRAS; i++) {
        colocarPalabra(&sopa, palabras, i);
      }
      for (int i = 0; i < NUM_PALABRAS; i++) {
        printf("%s\n", palabras[i].texto);
      }
      llenarTablero(&sopa);
      mostrarTablero(&sopa);

      break;
    case 2:
      printf("Creditos\n");
      break;
    case 3:
      printf("Highscore\n");
      break;
    case 4:
      printf("Anadir palabras\n");
    }

  } while (opt != 5);

  return 0;
}

enum direccion {
  ARRIBA_DERECHA,
  ARRIBA_IZQUIERDA,
  ABAJO_DERECHA,
  ABAJO_IZQUIERDA
};

int colocarPalabra(Sopa *sopa, Palabra p[], int idx) {

  int len = strlen(p[idx].texto);
  if (len == 0 || len > TAM)
    return 0;

  for (int intento = 0; intento < 100; intento++) {
    int horizontal = rand() % 2;
    int diagonal = rand() % 2;
    int direccion = rand() % 4;
    int fila = rand() % (horizontal ? TAM : TAM - len + 1);
    int col = rand() % (horizontal ? TAM - len + 1 : TAM);

    // checar que quepa (se vale cruzarse si es la misma letra)
    int cabe = 1;
    for (int k = 0; k < len; k++) {
      int r = horizontal ? fila : fila + k;
      int c = horizontal ? col + k : col;
      char actual = sopa->tablero[r][c];
      if (actual != '\0' && actual != p[idx].texto[k]) {
        cabe = 0;
        break;
      }
    }
    if (!cabe)
      continue;

    // ahora sí la escribimos letra por letra
    for (int k = 0; k < len; k++) {
      int r = horizontal ? fila : fila + k;
      int c = horizontal ? col + k : col;
      sopa->tablero[r][c] = toupper(p[idx].texto[k]);
    }

    p->yi = fila;
    p->xi = col;
    p->yf = horizontal ? fila : fila + len - 1;
    p->xf = horizontal ? col + len - 1 : col;
    p->encontrada = NO_ENCONTRADA;
    return 1;
  }

  return 0; // no cupo después de 100 intentos
}

char letraAleatoria(void) {

  int posicion = rand() % 26;
  char letra = 'A' + posicion;
  return letra;
}

void llenarTablero(Sopa *sopa) {

  for (int rows = 0; rows < TAM; rows++) {
    for (int cols = 0; cols < TAM; cols++) {
      if (sopa->tablero[rows][cols] == '\0')
        sopa->tablero[rows][cols] = letraAleatoria();
    }
  }
}

void limpiarTablero(Sopa *sopa) {
  memset(sopa->tablero, '\0', sizeof(sopa->tablero));
}

void mostrarTablero(Sopa *sopa) {

  printf("=============================================\n");
  for (int rows = 0; rows < TAM; rows++) {
    for (int cols = 0; cols < TAM; cols++) {
      printf(" %c ", sopa->tablero[rows][cols]);
    }
    printf("\n");
  }
  printf("=============================================\n");
}

int seleccionarPalabras(Palabra palabras[]) {
  char todas[MAX_PALABRAS_ARCHIVO][MAX_LEN];
  char temp[MAX_LEN];
  int total = 0;
  int cantidad;
  int i, j;
  FILE *archivo;

  archivo = fopen(ARCH_PALABRAS, "r");
  if (archivo == NULL) {
    printf("Error: no se pudo abrir %s\n", ARCH_PALABRAS);
    return 0;
  }

  while (total < MAX_PALABRAS_ARCHIVO &&
         fgets(todas[total], MAX_LEN, archivo) != NULL) {
    todas[total][strcspn(todas[total], "\r\n")] =
        '\0'; // Quitamos el salto de línea (\n o \r\n de Windows)

    if (strlen(todas[total]) > 0) // Solo contamos la línea si no quedó vacía
    {
      total++; // Avanzamos a la siguiente posición
    }
  }

  fclose(archivo); // Cerramos el archivo porque ya terminamos de leerlo

  // Revolvemos las palabras con el algoritmo de Fisher-Yates (así ninguna se
  // repite)
  for (i = total - 1; i > 0;
       i--) // Recorremos desde la última palabra hacia la primera
  {
    j = rand() % (i + 1); // Elegimos una posición al azar entre 0 e i

    strcpy(temp, todas[i]);     // Guardamos la palabra de la posición i
    strcpy(todas[i], todas[j]); // Ponemos la palabra de j en la posición i
    strcpy(todas[j], temp);     // Ponemos la palabra guardada en la posición j
  }

  // Si el archivo tiene menos de 10 palabras, tomamos solo las que haya
  if (total < MAX_SELECCION) {
    cantidad = total; // Tomamos todas las que hay
  } else {
    cantidad = MAX_SELECCION; // Tomamos solo 10, que es el máximo
  }

  // Copiamos las palabras ya revueltas al campo "texto" de cada estructura
  for (i = 0; i < cantidad; i++) {
    strcpy(palabras[i].texto, todas[i]);
  }

  return cantidad;
}
