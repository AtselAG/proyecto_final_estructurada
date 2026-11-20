#ifndef palabras_H
#define palabras_H

#define ARCHIVO_PALABRAS "palabras.txt"
#define MIN_LONGITUD 3
#define MAX_LONGITUD 15

void convertirAMayusculas(char *palabra);

int esPalabraValida(const char *palabra);

int existePalabra(const char *palabra);

void agregarPalabra(const char *palabra);

#endif

