#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "hash.h"

#define CAPACIDAD_INICIAL 10

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
struct hash {
	lista_t** tabla;
	size_t cantidad;
	size_t capacidad;
	hash_destruir_dato_t destruir_dato;
};

typedef struct nodo {
	const char* clave;
	void* dato;
}nodo_t;

struct hash_iter {
	size_t pos;
	const hash_t* hash;
	lista_iter_t* iter_actual;
};


/* ******************************************************************
 *                			HASH FUNCTION
 * *****************************************************************/
//One-at-a-time hash

unsigned long funcion_hash(const char* clave, size_t capacidad) {
  unsigned char *p = clave;
  unsigned h = 0;

  for (int i=0; i<capacidad; i++) {
	  h += p[i];
	  h += (h << 10);
	  h ^= (h >> 6);
  }

  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);

  return h%capacidad;
}




/* ******************************************************************
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {

	hash_t* tabla_hash = malloc(sizeof(hash_t));

	if (!tabla_hash) {
		return NULL;
	}

	tabla_hash->tabla = malloc(CAPACIDAD_INICIAL * sizeof(lista_t*));

	if (!tabla_hash->tabla){
		free(tabla_hash);
		return NULL;
	}

	for (int i = 0; i < CAPACIDAD_INICIAL; i++) {
		tabla_hash->tabla[i] = lista_crear();
	}
	
	tabla_hash->cantidad = 0;
	tabla_hash->capacidad = CAPACIDAD_INICIAL;
	tabla_hash->destruir_dato = destruir_dato;
	
	return tabla_hash;

}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}


/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/
size_t siguiente_posicion_con_elementos(const hash_t *hash, size_t pos) {
	while(lista_esta_vacia(hash->tabla[pos]) && pos < hash->capacidad) {
		pos++;
	}

	return pos;

}



hash_iter_t *hash_iter_crear(const hash_t *hash) {
	hash_iter_t* iterador = malloc(sizeof(hash_iter_t));

	if (!iterador) {
		return NULL;
	}

	iterador->hash = hash;

	//si el hash esta vacio el iterador esta al final
	if(iterador->hash->cantidad == 0){
		iterador->pos = iterador->hash->capacidad - 1;
		iterador->iter_actual = NULL;
	} else {
		//si tiene elementos busca la primera lista que tenga elementos
		//le asigna la posicion de esa lista en el haintsh y crea el iterador de esa lista
		int i= 0;

		/*
		while(lista_esta_vacia(hash->tabla[i]) && i < hash->capacidad) {
			i++;
		}

		iterador->pos = i;
		*/
		iterador->pos = siguiente_posicion_con_elementos(hash,i);
		iterador->iter_actual = lista_iter_crear(hash->tabla[i]);

	}

	return iterador;
}


bool hash_iter_al_final(const hash_iter_t *iter){
	size_t siguiente_lista = siguiente_posicion_con_elementos(iter->hash, iter->pos);
	if((iter->pos == siguiente_lista) || (iter->hash->cantidad == 0)){
		return true;
	}
	return false;
}

bool hash_iter_avanzar(hash_iter_t *iter) {
	//si esta al final devuelve false
	if (hash_iter_al_final(iter)){
		return false;
	}

	//avanza y si el "sub-iterador" de la lista esta al final destruye el "sub-iterador"
	//y va a la siguiente lista con elementos
	if(!lista_iter_avanzar(iter->iter_actual)) {
		lista_iter_destruir(iter->iter_actual);

		//busca la siguiente pos con elementos
		size_t nueva_posicion = siguiente_posicion_con_elementos(iter->hash, iter->pos);

		//si es la misma posicion que la anterior, entonces no hay listas con elementos y devuelve false
		if(hash_iter_al_final(iter)){
			return false;
		}

		//si avanzó bien, se crea el iterador para la siguiente lista con elementos
		iter->pos = nueva_posicion;
		iter->iter_actual = lista_iter_crear(iter->hash->tabla[iter->pos]);
	}

	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	if(hash_iter_al_final(iter)){
		return NULL;
	}

	return iter->iter_actual->actual->clave;
}

void hash_iter_destruir(hash_iter_t* iter) {
	if (iter->iter_actual) {
		lista_iter_destruir(iter->iter_actual);
	}

	free(iter);
}
