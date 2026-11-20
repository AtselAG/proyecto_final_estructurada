#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ROWS 15
#define MAX_COLS 15

char letraAleatoria(void);
void mostrarTablero(void);

int main(void) {

  mostrarTablero();

  return 0;
}

char letraAleatoria(void) {

  int posicion = rand() % 26;
  char letra = 'A' + posicion;
  return letra;
}

void mostrarTablero(void) {

  printf("=============================================\n");
  for (int rows = 0; rows < MAX_ROWS; rows++) {
    for (int cols = 0; cols < MAX_COLS; cols++) {
      printf(" %c ", letraAleatoria());
    }
    printf("\n");
  }
  printf("=============================================\n");
}