#include "pokeapi.h"
#include "cJSON.h"
#include <ctype.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_POKEMON "https://pokeapi.co/api/v2/pokemon/"

// Aqui se va juntando lo que manda el servidor (llega en pedazos).
typedef struct {
  char *datos;  // el texto de la respuesta (el JSON)
  size_t largo; // cuantos caracteres llevamos
} Respuesta;

void iniciarPokeAPI(void) { curl_global_init(CURL_GLOBAL_DEFAULT); }

void cerrarPokeAPI(void) { curl_global_cleanup(); }

// curl llama esta funcion cada vez que le llega un pedazo de la respuesta.
// Lo pegamos al final de lo que ya teniamos.
static size_t guardarPedazo(void *pedazo, size_t tam, size_t n, void *destino) {
  size_t total = tam * n;
  Respuesta *r = (Respuesta *)destino;

  char *nuevo = realloc(r->datos, r->largo + total + 1); // +1 para el '\0'
  if (nuevo == NULL) {
    return 0; // sin memoria: regresar 0 le dice a curl que pare
  }

  r->datos = nuevo;
  memcpy(r->datos + r->largo, pedazo, total);
  r->largo += total;
  r->datos[r->largo] = '\0';
  return total;
}

// Descarga una URL y regresa el texto (hay que hacerle free despues).
// Regresa NULL si no hay internet, se tardo mucho o el servidor dio error (404,
// etc.).
static char *descargar(const char *url) {
  CURL *curl = curl_easy_init();
  Respuesta r = {NULL, 0};
  CURLcode resultado;

  if (curl == NULL) {
    return NULL;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, guardarPedazo);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "sopa-de-letras/1.0");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); // 404 cuenta como error
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L); // maximo 8 s, para no colgarse

  resultado = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if (resultado != CURLE_OK) {
    free(r.datos);
    return NULL;
  }
  return r.datos;
}

// La API da los stats en ingles; aqui los traducimos para imprimirlos.
static const char *traducirStat(const char *stat) {
  if (strcmp(stat, "hp") == 0)
    return "PS";
  if (strcmp(stat, "attack") == 0)
    return "Ataque";
  if (strcmp(stat, "defense") == 0)
    return "Defensa";
  if (strcmp(stat, "special-attack") == 0)
    return "At. Especial";
  if (strcmp(stat, "special-defense") == 0)
    return "Def. Especial";
  if (strcmp(stat, "speed") == 0)
    return "Velocidad";
  return stat;
}

// Imprime la descripcion en espanol que viene en el JSON de la especie.
static void imprimirDescripcion(const char *jsonEspecie) {
  cJSON *especie = cJSON_Parse(jsonEspecie);
  cJSON *entradas, *entrada;

  if (especie == NULL) {
    return;
  }

  // flavor_text_entries trae la descripcion de cada juego en muchos idiomas.
  // Agarramos la primera que este en espanol ("es").
  entradas = cJSON_GetObjectItem(especie, "flavor_text_entries");
  cJSON_ArrayForEach(entrada, entradas) {
    cJSON *idioma = cJSON_GetObjectItem(entrada, "language");
    cJSON *codigo = cJSON_GetObjectItem(idioma, "name");
    cJSON *texto = cJSON_GetObjectItem(entrada, "flavor_text");

    if (cJSON_IsString(codigo) && strcmp(codigo->valuestring, "es") == 0 &&
        cJSON_IsString(texto)) {
      // El texto trae saltos de linea y '\f' raros de los juegos viejos:
      // los cambiamos por espacios.
      char limpio[512];
      int i;
      strncpy(limpio, texto->valuestring, sizeof(limpio) - 1);
      limpio[sizeof(limpio) - 1] = '\0';
      for (i = 0; limpio[i] != '\0'; i++) {
        if (limpio[i] == '\n' || limpio[i] == '\f' || limpio[i] == '\r') {
          limpio[i] = ' ';
        }
      }
      printf("\n%s\n", limpio);
      break;
    }
  }

  cJSON_Delete(especie);
}

// Lee el JSON del pokemon e imprime los datos. Regresa la URL de la especie
// (para pedir la descripcion) o NULL. Hay que hacerle free a lo que regrese.
static char *imprimirPokemon(const char *jsonPokemon) {
  cJSON *poke = cJSON_Parse(jsonPokemon);
  cJSON *item, *lista;
  char *urlEspecie = NULL;

  if (poke == NULL) {
    printf("La respuesta de la PokeAPI vino rara, no se pudo leer.\n");
    return NULL;
  }

  cJSON *id = cJSON_GetObjectItem(poke, "id");
  cJSON *nombre = cJSON_GetObjectItem(poke, "name");
  cJSON *altura = cJSON_GetObjectItem(poke, "height"); // en decimetros
  cJSON *peso = cJSON_GetObjectItem(poke, "weight");   // en hectogramos

  printf("\n------------- POKEDEX -------------\n");
  if (cJSON_IsNumber(id) && cJSON_IsString(nombre)) {
    printf("#%03d %s\n", id->valueint, nombre->valuestring);
  }

  // Tipos: "types": [ { "type": { "name": "electric" } }, ... ]
  printf("Tipo:        ");
  lista = cJSON_GetObjectItem(poke, "types");
  cJSON_ArrayForEach(item, lista) {
    cJSON *tipo =
        cJSON_GetObjectItem(cJSON_GetObjectItem(item, "type"), "name");
    if (cJSON_IsString(tipo)) {
      printf("%s ", tipo->valuestring);
    }
  }
  printf("\n");

  if (cJSON_IsNumber(altura)) {
    printf("Altura:      %.1f m\n", altura->valueint / 10.0);
  }
  if (cJSON_IsNumber(peso)) {
    printf("Peso:        %.1f kg\n", peso->valueint / 10.0);
  }

  // Habilidades: "abilities": [ { "ability": { "name": "static" } }, ... ]
  printf("Habilidades: ");
  lista = cJSON_GetObjectItem(poke, "abilities");
  cJSON_ArrayForEach(item, lista) {
    cJSON *hab =
        cJSON_GetObjectItem(cJSON_GetObjectItem(item, "ability"), "name");
    if (cJSON_IsString(hab)) {
      printf("%s ", hab->valuestring);
    }
  }
  printf("\n");

  // Stats: "stats": [ { "base_stat": 35, "stat": { "name": "hp" } }, ... ]
  lista = cJSON_GetObjectItem(poke, "stats");
  cJSON_ArrayForEach(item, lista) {
    cJSON *valor = cJSON_GetObjectItem(item, "base_stat");
    cJSON *stat =
        cJSON_GetObjectItem(cJSON_GetObjectItem(item, "stat"), "name");
    if (cJSON_IsNumber(valor) && cJSON_IsString(stat)) {
      printf("  %-14s %3d\n", traducirStat(stat->valuestring), valor->valueint);
    }
  }

  // Guardamos la URL de la especie para pedir la descripcion despues.
  cJSON *url = cJSON_GetObjectItem(cJSON_GetObjectItem(poke, "species"), "url");
  if (cJSON_IsString(url)) {
    urlEspecie = malloc(strlen(url->valuestring) + 1);
    if (urlEspecie != NULL) {
      strcpy(urlEspecie, url->valuestring);
    }
  }

  cJSON_Delete(poke);
  return urlEspecie;
}

void mostrarInfoPokemon(const char *nombre) {
  char url[128];
  char nombreMinus[64];
  char *jsonPokemon, *jsonEspecie, *urlEspecie;
  int i;

  // La API pide el nombre en minusculas: PIKACHU -> pikachu
  for (i = 0; nombre[i] != '\0' && i < (int)sizeof(nombreMinus) - 1; i++) {
    nombreMinus[i] = tolower((unsigned char)nombre[i]);
  }
  nombreMinus[i] = '\0';

  snprintf(url, sizeof(url), "%s%s", URL_POKEMON, nombreMinus);

  printf("Buscando a %s en la Pokedex...\n", nombre);
  jsonPokemon = descargar(url);
  if (jsonPokemon == NULL) {
    printf("No se pudo traer la info de %s (sin internet o no existe en la "
           "PokeAPI).\n",
           nombre);
    return;
  }

  urlEspecie = imprimirPokemon(jsonPokemon);
  free(jsonPokemon);

  // Segunda peticion: la descripcion viene en otro endpoint (la especie).
  if (urlEspecie != NULL) {
    jsonEspecie = descargar(urlEspecie);
    if (jsonEspecie != NULL) {
      imprimirDescripcion(jsonEspecie);
      free(jsonEspecie);
    }
    free(urlEspecie);
  }
  printf("-----------------------------------\n");
}
