#include "constantes.h"
#include "tablero.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  srand(time(NULL));

  int opt;
  char tablero[MAX_ROWS][MAX_COLS];

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
      llenarTablero(tablero);
      mostrarTablero(tablero);
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
