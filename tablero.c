#include "tablero.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char letraAleatoria(void) {

  int posicion = rand() % 26;
  char letra = 'A' + posicion;
  return letra;
}

void llenarTablero(char tablero[MAX_ROWS][MAX_COLS]) {

  for (int rows = 0; rows < MAX_ROWS; rows++) {
    for (int cols = 0; cols < MAX_COLS; cols++) {
      tablero[rows][cols] = letraAleatoria();
    }
  }
}

void mostrarTablero(char tablero[MAX_ROWS][MAX_COLS]) {

  printf("=============================================\n");
  for (int rows = 0; rows < MAX_ROWS; rows++) {
    for (int cols = 0; cols < MAX_COLS; cols++) {
      printf(" %c ", tablero[rows][cols]);
    }
    printf("\n");
  }
  printf("=============================================\n");
}