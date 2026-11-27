#ifndef CONSTANTES_H

#define CONSTANTES_H
#define TAM 15         /* tamano del tablero TAM x TAM            */
#define NUM_PALABRAS 10 /* palabras por partida                    */
#define MAX_LEN 16     /* longitud maxima de palabra (+ '\0')     */
#define MIN_LEN 3      /* longitud minima de palabra              */
#define MAX_NOMBRE 50
#define MAX_BANCO 500 /* palabras maximas leidas del archivo     */
#define TOP_GUARDADOS 10
#define TOP_MOSTRAR 5
#define MAX_INTENTOS 100 /* intentos para colocar una palabra       */

/* Colores ANSI para marcar las palabras encontradas */
#define VERDE "\033[1;32m"
#define AMARI "\033[1;33m"
#define RESET "\033[0m"

#define ARCH_PALABRAS "palabras.txt"
#define ARCH_SCORES "highscores.d"

#endif