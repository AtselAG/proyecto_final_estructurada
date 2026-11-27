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
void mostrarTablero(Sopa *sopa, int filaMarca, int colMarca,
                    Palabra palabras[]);
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
int leerCoordenada(int *x, int *y);
int contarEncontradas(Palabra palabras[]);
int celdaEncontrada(Palabra palabras[], int fila, int col);
void mostrarListaPalabras(Palabra palabras[]);
void jugar();
void solicitarNombre(Jugador *jugador);

int main(void) {
  int opt;
  srand(time(NULL)); // la semilla se pone UNA sola vez al inicio del programa

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
      jugar();
      break;
    case 2:
      creditos();
      break;
    case 3:
      readHS();
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
      if (actual != '\0' && actual != toupper((unsigned char)w->texto[k])) {
        cabe = 0;
        break;
      }
    }
    if (!cabe)
      continue;

    // ahora sí la escribimos letra por letra
    for (int k = 0; k < len; k++)
      sopa->tablero[fila + k * dr][col + k * dc] =
          toupper((unsigned char)w->texto[k]); // letra en mayuscula en el
                                               // tablero, aunque el archivo la
                                               // tenga en minuscula

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

// palabras se usa para pintar en verde las casillas de las palabras ya
// encontradas y para imprimir la lista de palabras debajo del tablero.
void mostrarTablero(Sopa *sopa, int filaMarca, int colMarca,
                    Palabra palabras[]) {

  printf("=============================================\n");

  // Encabezado: el numero de cada columna (3 espacios al inicio para dejar
  // lugar al numero de fila).
  printf("   ");
  for (int cols = 0; cols < TAM; cols++) {
    printf("%2d ", cols + 1); // +1: el jugador ve del 1 al 15, por dentro el
                              // arreglo va de 0 a 14
  }
  printf("\n");

  for (int rows = 0; rows < TAM; rows++) {
    printf("%2d ", rows + 1); // numero de la fila al inicio del renglon (+1: se
                              // ve del 1 al 15)
    for (int cols = 0; cols < TAM; cols++) {
      if (rows == filaMarca && cols == colMarca) {
        // Esta es la letra marcada: se imprime en amarillo (AMARI y RESET
        // vienen de constantes.h).
        printf(" " AMARI "%c" RESET " ", sopa->tablero[rows][cols]);
      } else if (celdaEncontrada(palabras, rows, cols)) {
        // Casilla que pertenece a una palabra ya encontrada: se imprime en
        // verde (VERDE viene de constantes.h).
        printf(" " VERDE "%c" RESET " ", sopa->tablero[rows][cols]);
      } else {
        printf(" %c ", sopa->tablero[rows][cols]);
      }
    }
    printf("\n");
  }
  printf("=============================================\n");

  // Debajo del tablero: la lista de palabras (verde = encontrada).
  mostrarListaPalabras(palabras);
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
    seleccion[k] = toupper(
        (unsigned char)tablero[filaIni + k * pasoFila]
                              [colIni + k * pasoCol]); // toupper: se compara
                                                       // en mayusculas aunque
                                                       // el tablero tenga
                                                       // minusculas

  seleccion[largo] = '\0';

  for (i = 0; i < NUM_PALABRAS; i++) {
    // Si ya la habia encontrado, la saltamos (asi no se "mueve" a otra copia
    // de la misma palabra que haya en el tablero).
    if (palabras[i].encontrada == ENCONTRADA) {
      continue;
    }

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

      // Guardamos las casillas que ELIGIO el jugador, no donde la puso
      // colocarPalabra. Si las letras de relleno formaron otra copia de la
      // palabra y el jugador marco esa, el verde se pinta justo ahi y no en
      // otro lado.
      palabras[i].yi = filaIni;
      palabras[i].xi = colIni;
      palabras[i].yf = filaFin;
      palabras[i].xf = colFin;
      break; // ya la encontramos, no hay que revisar las demas
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
    if (scanf("%15s", pokemon) != 1) // 15 = MAX_LEN - 1
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
  FILE *highscore = fopen(ARCH_SCORES, "a");
  if (highscore == NULL)
    return 1;
  fprintf(highscore, "%s,%d\n", jugador->nombre, jugador->tiempo);
  fclose(highscore);
  return 0;
}

int readHS(void) {
  Jugador jugadores[MAX_SCORES]; // aqui guardamos todos los scores del archivo
  Jugador temp;             // auxiliar para intercambiar en el ordenamiento
  char linea[MAX_LEN + 20]; // una linea del archivo: "nombre,tiempo"
  char *coma;               // apunta a la coma que separa nombre y tiempo
  int total = 0;            // cuantos scores leimos
  int i, j;

  FILE *highscore = fopen(ARCH_SCORES, "r");
  if (highscore == NULL) {
    printf("Todavia no hay highscores guardados.\n");
    return 1;
  }

  // Leemos linea por linea hasta que se acabe el archivo o se llene el arreglo
  while (total < MAX_SCORES && fgets(linea, sizeof(linea), highscore) != NULL) {
    linea[strcspn(linea, "\r\n")] = '\0'; // quitamos el salto de linea

    // Buscamos la ULTIMA coma (por si el nombre trae alguna coma)
    coma = strrchr(linea, ',');
    if (coma == NULL) {
      continue; // linea mal formada, la saltamos
    }

    *coma = '\0'; // cortamos la cadena: lo de antes de la coma es el nombre
    strncpy(jugadores[total].nombre, linea, MAX_LEN - 1);
    jugadores[total].nombre[MAX_LEN - 1] = '\0';
    jugadores[total].tiempo = atoi(coma + 1); // lo de despues es el tiempo
    total++;
  }

  fclose(highscore);

  if (total == 0) {
    printf("Todavia no hay highscores guardados.\n");
    return 1;
  }

  // Ordenamos con burbuja de menor a mayor tiempo (el mas rapido primero)
  for (i = 0; i < total - 1; i++) {
    for (j = 0; j < total - 1 - i; j++) {
      if (jugadores[j].tiempo > jugadores[j + 1].tiempo) {
        temp = jugadores[j];
        jugadores[j] = jugadores[j + 1];
        jugadores[j + 1] = temp;
      }
    }
  }

  // Imprimimos la tabla
  printf("========== HIGH SCORES ==========\n");
  printf("%-4s %-20s %s\n", "#", "Nombre", "Tiempo");
  for (i = 0; i < total; i++) {
    printf("%-4d %-20s %d s\n", i + 1, jugadores[i].nombre,
           jugadores[i].tiempo);
  }
  printf("=================================\n");

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

int leerCoordenada(int *x, int *y) {
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

    // Si el jugador escribio salir (salir, Salir, SALIR...), se rinde:
    // retornamos 0. to_uppercase es la funcion que ya existia; pasa el texto a
    // mayusculas.
    to_uppercase(textoX);
    if (strcmp(textoX, "SALIR") == 0) {
      return 0;
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

  *x = atoi(textoX) - 1; // atoi convierte el texto "12" en el numero 12; el -1
                         // pasa de 1-15 (lo que ve el jugador) a 0-14 (el
                         // arreglo): la columna
  *y = atoi(textoY) - 1; // lo mismo para la fila

  // Como el tablero empieza en 1, la coordenada 0,0 (que queda en -1,-1) no
  // existe: la usamos para rendirse.
  if (*x == -1 && *y == -1) {
    return 0;
  }

  return 1; // se leyo una coordenada valida
}

void jugar() {
  time_t inicio;          // momento en que empieza a correr el tiempo
  Jugador jugador_actual; // nombre y tiempo de quien esta jugando
  Palabra palabras[NUM_PALABRAS];
  Sopa sopa;
  int xi, xf, yi, yf;
  int encontradas; // cuantas palabras lleva encontradas el jugador
  int antes;       // cuantas llevaba antes del ultimo intento
  int rendido;     // 1 si el jugador se rindio (salir o 0,0)
  seleccionarPalabras(palabras);
  limpiarTablero(&sopa);

  for (int i = 0; i < NUM_PALABRAS; i++) {
    colocarPalabra(&sopa, palabras, i);
  }

  llenarTablero(&sopa);

  // Pedimos el nombre ANTES de arrancar el tiempo, para que no cuente lo que
  // tarda en escribirlo.
  solicitarNombre(&jugador_actual);
  inicio = time(NULL);

  //  Repetimos hasta que el jugador encuentre todas las palabras.
  encontradas = 0;
  rendido = 0;
  while (encontradas < NUM_PALABRAS) {

    // La lista de palabras (en verde las ya encontradas) se imprime debajo del
    // tablero, dentro de mostrarTablero.
    printf("\nLlevas %d de %d palabras (escribe salir o 0,0 para rendirte):\n",
           encontradas, NUM_PALABRAS);

    mostrarTablero(
        &sopa, -1, -1,
        palabras); // -1, -1 = todavia no hay ninguna letra en amarillo
    printf("Ingresa una coordenada inicial (x,y): ");
    // leerCoordenada lee lo escrito como 4,8 y guarda la columna en xi y
    // la fila en yi.
    // Si el jugador se rinde, leerCoordenada retorna 0: salimos del while.
    if (!leerCoordenada(&xi, &yi)) {
      rendido = 1;
      break;
    }

    mostrarTablero(&sopa, yi, xi, palabras);
    printf("Ingresa una coordenada final (x,y): ");
    if (!leerCoordenada(&xf, &yf)) {
      rendido = 1;
      break;
    }

    antes = encontradas;
    validarCoordenadas(sopa.tablero, yi, xi, yf, xf, palabras);
    encontradas = contarEncontradas(palabras);

    if (encontradas > antes) {
      printf("Encontraste una palabra nueva! (%d de %d)\n", encontradas,
             NUM_PALABRAS);
    } else {
      printf("No encontraste ninguna palabra nueva con esas coordenadas.\n");
    }
  }

  if (rendido) {
    // Si se rindio NO se guarda en el highscore.
    printf("\nTe rendiste. Encontraste %d de %d palabras.\n", encontradas,
           NUM_PALABRAS);
  } else {
    // PRIMERO calculamos el tiempo y DESPUES guardamos (si no, se guarda
    // basura).
    jugador_actual.tiempo = time(NULL) - inicio;
    jugador_actual.palabras_encontradas = encontradas;

    // Ultimo dibujo del tablero: ya con las 10 palabras en verde.
    mostrarTablero(&sopa, -1, -1, palabras);
    printf("\nEncontraste las %d palabras en %d segundos!\n", NUM_PALABRAS,
           jugador_actual.tiempo);

    saveHS(&jugador_actual);
  }
}

void solicitarNombre(Jugador *jugador) {
  printf("Ingresa tu nombre: ");
  scanf(" %15s", jugador->nombre); // 15 = MAX_LEN - 1, para no desbordar
  getchar();
}

// Retorna 1 si la casilla (fila, col) pertenece a alguna palabra ya encontrada,
// 0 si no. Usa xi, yi, xf, yf de cada Palabra, que colocarPalabra ya guardo al
// acomodarla en el tablero.
int celdaEncontrada(Palabra palabras[], int fila, int col) {
  int i, k; // i recorre las palabras, k recorre las letras de una palabra
  int difFila, difCol;   // distancia entre la casilla inicial y la final de la
                         // palabra
  int pasoFila, pasoCol; // hacia donde avanza la palabra: -1, 0 o 1 en fila y
                         // en columna
  int largo;             // cuantas letras tiene la palabra en el tablero

  for (i = 0; i < NUM_PALABRAS; i++) {
    // Solo nos importan las que el jugador ya encontro.
    if (palabras[i].encontrada != ENCONTRADA) {
      continue;
    }

    // Mismo calculo que usa validarCoordenadas para recorrer una palabra.
    difFila = palabras[i].yf - palabras[i].yi;
    difCol = palabras[i].xf - palabras[i].xi;
    pasoFila = (difFila > 0) - (difFila < 0);
    pasoCol = (difCol > 0) - (difCol < 0);
    largo = (abs(difFila) > abs(difCol) ? abs(difFila) : abs(difCol)) + 1;

    // Recorremos las casillas de la palabra; si alguna es la que nos preguntan,
    // retornamos 1.
    for (k = 0; k < largo; k++) {
      if (palabras[i].yi + k * pasoFila == fila &&
          palabras[i].xi + k * pasoCol == col) {
        return 1;
      }
    }
  }

  return 0;
}

// Imprime las NUM_PALABRAS palabras de 5 en 5 por renglon:
// en verde las que ya se encontraron y en color normal las que faltan.
void mostrarListaPalabras(Palabra palabras[]) {
  for (int i = 0; i < NUM_PALABRAS; i++) {
    if (palabras[i].encontrada == ENCONTRADA) {
      printf(VERDE "%-14s" RESET, palabras[i].texto);
    } else {
      printf("%-14s", palabras[i].texto);
    }

    // Salto de renglon cada 5 palabras y al terminar la lista.
    if ((i + 1) % 5 == 0 || i == NUM_PALABRAS - 1) {
      printf("\n");
    }
  }
}