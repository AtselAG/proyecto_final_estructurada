#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

char letraAleatoria(void); //Prototipo da função que retorna una letra aleatória

int main(void) {
    
    srand(time(NULL)); // Inicializa o gerador de números aleatórios con base no tiempo actual

    char letra = letraAleatoria(); // Chama a função para obter uma letra aleatória
    printf("Letra aleatoria: %c\n", letra); // Imprime a la letra aleatora generada

    return 0;
}

char letraAleatoria(void) {

    int posicion = rand() % 26; // Crea un número aleatoro entre 0 y 25
    char letra = 'A' + posicion; // Convierte el número aleatorio en una letra mayúscula correspondiente (A-Z)
    return letra; // Retorna a letra aleatoria
}

