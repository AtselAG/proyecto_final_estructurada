#include "constantes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum estado_palabra { ENCONTRADA, NO_ENCONTRADA };

typedef struct tablero {

  char tablero[TAM][TAM];
  int lleno;

} Tablero;

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
void mostrarTablero(char tablero[TAM][TAM]);
void llenarTablero(char tablero[TAM][TAM]);
int seleccionarPalabras(Palabra palabras[]);

int main(void) {
  srand(time(NULL));
  int opt;
  Palabra palabras[NUM_PALABRAS];

  do {

    printf("==== Sopa de Letras ====\n"
           "[1] Jugar\n"
           "[2] Créditos\n"
           "[3] High Score\n"
           "[4] Añadir Palabras\n"
           "[5] Salir\n");
    scanf("%d", &opt);

    switch (opt) {

    case 1:
      seleccionarPalabras(palabras);
      for (int i = 0; i < NUM_PALABRAS; i++) {
        printf("%s\n", palabras[i].texto);
      }
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

void putInBoard(Palabra palabras[]) {}

char letraAleatoria(void) {

  int posicion = rand() % 26;
  char letra = 'A' + posicion;
  return letra;
}

void llenarTablero(char tablero[TAM][TAM]) {

  for (int rows = 0; rows < TAM; rows++) {
    for (int cols = 0; cols < TAM; cols++) {
      tablero[rows][cols] = letraAleatoria();
    }
  }
}

void mostrarTablero(char tablero[TAM][TAM]) {

  printf("=============================================\n");
  for (int rows = 0; rows < TAM; rows++) {
    for (int cols = 0; cols < TAM; cols++) {
      printf(" %c ", tablero[rows][cols]);
    }
    printf("\n");
  }
  printf("=============================================\n");
}

#define MAX_PALABRAS_ARCHIVO 100
#define MAX_SELECCION 10

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
