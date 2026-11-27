#include "constantes.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum estado_palabra { NO_ENCONTRADA, ENCONTRADA };
enum direccion {
  ARRIBA_DERECHA,
  ARRIBA_IZQUIERDA,
  ABAJO_DERECHA,
  ABAJO_IZQUIERDA
};
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
void mostrarTablero(Sopa *sopa, int filaMarca, int colMarca);
void llenarTablero(Sopa *sopa);
int seleccionarPalabras(Palabra palabras[]);
int colocarPalabra(Sopa *sopa, Palabra p[], int idx);
void limpiarTablero(Sopa *sopa);
int validarCoordenadas(char tablero[TAM][TAM], int filaIni, int colIni,
                       int filaFin, int colFin, Palabra palabras[]);
int existePalabra(const char *pokemon);
int agregarPalabras(void);
void creditos();
int saveHS(Jugador *jugador);
int readHS(void);
int line_count(FILE *file);
void to_uppercase(char cad[]);
int esNumero(char texto[]);
void leerCoordenada(int *x, int *y);
int contarEncontradas(Palabra palabras[]);

int main(void) {
  srand(time(NULL));
  int opt;
  Palabra palabras[NUM_PALABRAS];
  Sopa sopa;
  int xi, xf, yi, yf;
  int encontradas; // cuantas palabras lleva encontradas el jugador
  int antes;       // cuantas llevaba antes del ultimo intento

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

      llenarTablero(&sopa);
      //  Repetimos hasta que el jugador encuentre todas las palabras.
      encontradas = 0;
      while (encontradas < NUM_PALABRAS) {

        // Mostramos solo las palabras que todavia faltan por encontrar.
        printf("\nPalabras por encontrar (llevas %d de %d):\n", encontradas,
               NUM_PALABRAS);
        for (int i = 0; i < NUM_PALABRAS; i++) {
          if (palabras[i].encontrada == NO_ENCONTRADA) {
            printf("%s\n", palabras[i].texto);
          }
        }

        mostrarTablero(&sopa, -1,
                       -1); // -1, -1 = todavia no hay ninguna letra en amarillo
        printf("Ingresa una coordenada inicial (x,y): ");
        // leerCoordenada lee lo escrito como 4,8 y guarda la columna en xi y
        // la fila en yi.
        leerCoordenada(&xi, &yi);

        mostrarTablero(&sopa, yi, xi);
        printf("Ingresa una coordenada final (x,y): ");
        leerCoordenada(&xf, &yf);

        antes = encontradas;
        validarCoordenadas(sopa.tablero, yi, xi, yf, xf, palabras);
        encontradas = contarEncontradas(palabras);

        if (encontradas > antes) {
          printf("Encontraste una palabra nueva! (%d de %d)\n", encontradas,
                 NUM_PALABRAS);
        } else {
          printf(
              "No encontraste ninguna palabra nueva con esas coordenadas.\n");
        }
      }

      printf("\nEncontraste las %d palabras!\n", NUM_PALABRAS);

      break;
    case 2:
      creditos();
      break;
    case 3:
      readHS();
      printf("Highscore\n");
      break;
    case 4:
      agregarPalabras();
      break;
    }

  } while (opt != 5);

  return 0;
}

int colocarPalabra(Sopa *sopa, Palabra p[], int idx) {
  // {dr, dc}: horizontal, vertical y las 4 diagonales
  const int dirs[6][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  Palabra *w = &p[idx];
  int len = strlen(w->texto);
  if (len == 0 || len > TAM)
    return 0;

  for (int intento = 0; intento < 500; intento++) {
    int d = rand() % 6;
    int dr = dirs[d][0];
    int dc = dirs[d][1];
    int fila = rand() % TAM;
    int col = rand() % TAM;
    int filaFin = fila + (len - 1) * dr;
    int colFin = col + (len - 1) * dc;

    // si la palabra se sale del tablero, otro intento
    if (filaFin < 0 || filaFin >= TAM || colFin < 0 || colFin >= TAM)
      continue;

    // checar que quepa (se vale cruzarse si es la misma letra)
    int cabe = 1;
    for (int k = 0; k < len; k++) {
      char actual = sopa->tablero[fila + k * dr][col + k * dc];
      if (actual != '\0' && actual != w->texto[k]) {
        cabe = 0;
        break;
      }
    }
    if (!cabe)
      continue;

    // ahora sí la escribimos letra por letra
    for (int k = 0; k < len; k++)
      sopa->tablero[fila + k * dr][col + k * dc] = w->texto[k];

    w->yi = fila;
    w->xi = col;
    w->yf = filaFin;
    w->xf = colFin;
    w->encontrada = NO_ENCONTRADA;
    return 1;
  }

  return 0; // no cupo
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

void mostrarTablero(Sopa *sopa, int filaMarca, int colMarca) {

  printf("=============================================\n");

  // Encabezado: el numero de cada columna (3 espacios al inicio para dejar
  // lugar al numero de fila).
  printf("   ");
  for (int cols = 0; cols < TAM; cols++) {
    printf("%2d ", cols);
  }
  printf("\n");

  for (int rows = 0; rows < TAM; rows++) {
    printf("%2d ", rows); // numero de la fila al inicio del renglon
    for (int cols = 0; cols < TAM; cols++) {
      if (rows == filaMarca && cols == colMarca) {
        // Esta es la letra marcada: se imprime en amarillo (AMARI y RESET
        // vienen de constantes.h).
        printf(" " AMARI "%c" RESET " ", sopa->tablero[rows][cols]);
      } else {
        printf(" %c ", sopa->tablero[rows][cols]);
      }
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

  for (i = total - 1; i > 0;
       i--) // Recorremos desde la última palabra hacia la primera
  {
    j = rand() % (i + 1); // Elegimos una posición al azar entre 0 e i

    strcpy(temp, todas[i]);     // Guardamos la palabra de la posición i
    strcpy(todas[i], todas[j]); // Ponemos la palabra de j en la posición i
    strcpy(todas[j], temp);     // Ponemos la palabra guardada en la posición j
  }

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

int validarCoordenadas(char tablero[TAM][TAM], int filaIni, int colIni,
                       int filaFin, int colFin, Palabra palabras[]) {
  char seleccion[MAX_LEN];
  char palabraMayus[MAX_LEN];
  int difFila = filaFin - filaIni;
  int difCol = colFin - colIni;
  int pasoFila, pasoCol;
  int largo;
  int i, k;
  int largoPalabra;
  int coincide;
  int coincideInv;

  if (filaIni < 0 || filaIni >= TAM || colIni < 0 || colIni >= TAM ||
      filaFin < 0 || filaFin >= TAM || colFin < 0 || colFin >= TAM) {
    printf("Coordenadas fuera del tablero.\n");
    return -1;
  }

  if (difFila != 0 && difCol != 0 && abs(difFila) != abs(difCol))
    return -1;

  pasoFila = (difFila > 0) - (difFila < 0);
  pasoCol = (difCol > 0) - (difCol < 0);

  largo = (abs(difFila) > abs(difCol) ? abs(difFila) : abs(difCol)) + 1;

  if (largo >= MAX_LEN) {
    printf("La palabra seleccionada es demasiado larga y no existe.\n");
    return -1;
  }

  // Recorremos desde la coordenada inicial hasta la final, sumando o restando
  // 1 cada paso
  for (k = 0; k < largo; k++)
    seleccion[k] = tablero[filaIni + k * pasoFila][colIni + k * pasoCol];

  seleccion[largo] = '\0';

  for (i = 0; i < NUM_PALABRAS; i++) {
    largoPalabra = strlen(palabras[i].texto);

    if (largoPalabra != largo) {
      continue;
    }

    for (k = 0; k < largoPalabra; k++) {
      palabraMayus[k] = toupper((unsigned char)palabras[i].texto[k]);
    }
    palabraMayus[largoPalabra] = '\0';
    coincide = 1;
    coincideInv = 1;

    for (k = 0; k < largo; k++) // Comparamos letra por letra
    {
      if (seleccion[k] != palabraMayus[k]) // Al derecho: letra k con letra k
      {
        coincide = 0;
      }
      if (seleccion[k] != palabraMayus[largo - 1 - k]) // Al reves: letra k con
                                                       // la letra del final
      {
        coincideInv = 0;
      }
    }

    if (coincide || coincideInv) {
      palabras[i].encontrada = ENCONTRADA;
    }
  }

  return -1;
}

int existePalabra(const char *pokemon) {
  FILE *archivo = fopen(ARCH_PALABRAS, "r");
  if (archivo == NULL)
    return 0;

  char linea[MAX_LEN];
  while (fgets(linea, sizeof(linea), archivo) != NULL) {
    linea[strcspn(linea, "\r\n")] = '\0'; // quitar el salto de linea
    to_uppercase(linea);
    if (strcmp(linea, pokemon) == 0) {
      fclose(archivo);
      return 1;
    }
  }

  fclose(archivo);
  return 0;
}

int agregarPalabras(void) {
  char pokemon[MAX_LEN];

  while (1) {
    printf("Escribe un pokemon para agregar (salir para salir): ");
    if (scanf("%49s", pokemon) != 1) // 49 = MAX_LEN - 1
      break;
    to_uppercase(pokemon);

    if (strcmp(pokemon, "SALIR") == 0)
      break;

    if (existePalabra(pokemon)) {
      printf("Ese pokemon ya existe\n");
      continue;
    }

    FILE *archivo = fopen(ARCH_PALABRAS, "a");
    if (archivo == NULL) {
      printf("No se pudo abrir el archivo\n");
      return 1;
    }
    fprintf(archivo, "%s\n", pokemon);
    fclose(archivo);
    printf("Agregado: %s\n", pokemon);
  }

  return 0;
}

int saveHS(Jugador *jugador) {
  FILE *highscore = fopen(ARCH_SCORES, "ab");
  if (highscore == NULL)
    return 1;
  fprintf(highscore, "%s,%d\n", jugador->nombre, jugador->tiempo);
  fclose(highscore);
  return 0;
}

int readHS(void) {
  FILE *highscore = fopen(ARCH_SCORES, "rb");
  if (highscore == NULL)
    return 1;
  // while (fread(&buffer, sizeof(buffer), n, mi_archivo) != NULL) {
  //   printf("%s", buffer);
  // }

  fclose(highscore);
  return 0;
}

void creditos() {

  printf("Axel Emilio Avendaño Garcia\n");
  printf("Diego Alberto Rodriguez Palomar\n");
  printf("TODOS LOS DERECHOS RESERVADOS ©,\n");
}

int line_count(FILE *file) {

  int lines = 0;
  char cadena[255];
  rewind(file);
  while (fgets(cadena, sizeof(cadena), file) != NULL) {
    lines++;
  }

  return lines;
}

void to_uppercase(char cad[]) {
  for (int i = 0; cad[i] != '\0'; i++) {
    cad[i] = toupper((unsigned char)cad[i]);
  }
}

int esNumero(char texto[]) {
  int i; // posicion del caracter que se esta revisando dentro de texto

  // Un texto vacio no es un numero.
  if (texto[0] == '\0') {
    return 0;
  }

  // Revisamos caracter por caracter, de izquierda a derecha, hasta el '\0'.
  for (i = 0; texto[i] != '\0'; i++) {
    // Si el caracter no esta entre '0' y '9', no es un numero.
    if (texto[i] < '0' || texto[i] > '9') {
      return 0; // basta con un caracter malo para rechazar todo el texto
    }
  }

  return 1; // todos los caracteres eran digitos
}

// Cuenta cuantas palabras de la lista ya estan marcadas como ENCONTRADA.
int contarEncontradas(Palabra palabras[]) {
  int total = 0; // cuantas llevamos contadas
  int i;         // recorre las palabras de la lista

  for (i = 0; i < NUM_PALABRAS; i++) {
    if (palabras[i].encontrada == ENCONTRADA) {
      total++;
    }
  }

  return total;
}

void leerCoordenada(int *x, int *y) {
  char textoX[10]; // lo que escribio el jugador para la columna (hasta 9
                   // caracteres + '\0')
  char textoY[10]; // lo que escribio el jugador para la fila
  int valido;      // 1 si los dos textos son numeros, 0 si alguno no lo es
  int i;           // posicion dentro de un texto (para quitar los parentesis)
  int c;           // caracter que se usa para limpiar el teclado

  do {
    textoX[0] = '\0'; // dejamos los textos vacios por si scanf no alcanza a
                      // leer nada
    textoY[0] = '\0';

    scanf(" %9[^,\n],%9s", textoX, textoY);

    while ((c = getchar()) != '\n' && c != EOF) {
      // no hace nada, solo consume caracteres
    }

    // Si el jugador escribio los parentesis, como (4,8), los quitamos.
    if (textoX[0] == '(') {
      // recorremos el texto un lugar a la izquierda para borrar el '('
      for (i = 0; textoX[i] != '\0'; i++) {
        textoX[i] = textoX[i + 1];
      }
    }
    i = strlen(textoY); // largo de la fila escrita
    if (i > 0 && textoY[i - 1] == ')') {
      textoY[i - 1] = '\0'; // el ')' final se convierte en el fin de la cadena
    }

    // Los dos textos deben ser numeros.
    valido = esNumero(textoX) && esNumero(textoY);

    if (!valido) {
      printf("Formato incorrecto. Escribe la coordenada como 4,8 (solo "
             "numeros): ");
    }
  } while (!valido); // se repite hasta que los dos sean numeros

  *x = atoi(textoX); // atoi convierte el texto "12" en el numero 12: la columna
  *y = atoi(textoY); // lo mismo para la fila
}