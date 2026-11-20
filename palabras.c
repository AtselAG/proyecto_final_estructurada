#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "palabras.h"

void convertirAMayusculas(char *palabra) {
    for (int i = 0; palabra[i] != '\0'; i++) {
        palabra[i] = toupper((unsigned char)palabra[i]);
    }
}

int esPalabraValida(const char *palabra) {
    int longitud = strlen(palabra);
    if (longitud < MIN_LONGITUD ||)
}
