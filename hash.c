#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "hash.h"

#define CAPACIDAD_INICIAL 8
#define MAX_ESPACIO_USADO 4
#define MIN_ESPACIO_USADO 0.5

struct hash {
	lista_t** tabla;
	size_t cantidad;
	size_t capacidad;
	hash_destruir_dato_t destruir_dato;
};

typedef struct nodo {
	char* clave;
	void* dato;
}nodo_t;

struct hash_iter {
	size_t pos;
	const hash_t* hash;
	lista_iter_t* iter_actual;
};

size_t funcion_hash(const char* clave, size_t capacidad) {
	size_t  h = 0;
	size_t largo = strlen(clave);

	for (int i=0; i<largo; i++) {
		h += (size_t)clave[i];
		h += (h << 10);
		h ^= (h >> 6);
	}

	h += (h << 3);
	h ^= (h >> 11);
	h += (h << 15);

	return h%capacidad;
}

bool tabla_crear_listas(lista_t** tabla, size_t capacidad){
	for (int i = 0; i < capacidad; i++) {
		tabla[i] = lista_crear();
		if (tabla[i] == NULL){
			for (; i > 0; i--){lista_destruir(tabla[i], NULL);}
			return false;
		}
	}
	return true;
}

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
	
	tabla_crear_listas(tabla_hash->tabla, CAPACIDAD_INICIAL);
	
	tabla_hash->cantidad = 0;
	tabla_hash->capacidad = CAPACIDAD_INICIAL;
	tabla_hash->destruir_dato = destruir_dato;
	
	return tabla_hash;

}

nodo_t* crear_nodo(const char* clave,void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo){
		return NULL;
	}
	nodo->clave = strdup(clave);
	nodo->dato = dato;
	
	return nodo;
}

nodo_t* buscar_nodo(const hash_t* hash,const char* clave){
	size_t clave_hash = funcion_hash(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla[clave_hash]);
	if (iter == NULL){return NULL;}
	nodo_t* nodo = lista_iter_ver_actual(iter);
	while(!lista_iter_al_final(iter) && strcmp(nodo->clave,clave) != 0){
		lista_iter_avanzar(iter);
		nodo = lista_iter_ver_actual(iter);
	}
	lista_iter_destruir(iter);
	
	return nodo;
}

void destruir_nodo(nodo_t* nodo,void destruir_dato(void*)){
	free(nodo->clave);
	if (destruir_dato){
		destruir_dato(nodo->dato);
	}
	free(nodo);
}

bool hash_redimensionar(hash_t* hash,size_t new_tam){
	nodo_t* nodo;
	size_t clave_hash;
	lista_t** new_tablas = malloc(new_tam * sizeof(lista_t*));
	if (!new_tablas){
		return false;
	}
	tabla_crear_listas(new_tablas, new_tam);
	for(int i = 0;i < hash->capacidad;i++){ 
		while (!lista_esta_vacia(hash->tabla[i])){ 
			nodo = lista_borrar_primero(hash->tabla[i]);
			clave_hash = funcion_hash(nodo->clave,new_tam);
			lista_insertar_primero(new_tablas[clave_hash],nodo);
		}
		lista_destruir(hash->tabla[i], NULL);
	}
	hash->capacidad = new_tam;
	free(hash->tabla);
	hash->tabla = new_tablas;
	return true;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if (hash->cantidad/hash->capacidad > MAX_ESPACIO_USADO){
		if (!hash_redimensionar(hash,hash->capacidad*2)){
			return false;
		}
	}
	size_t clave_hash = funcion_hash(clave,hash->capacidad);
	nodo_t* nodo = buscar_nodo(hash,clave);
	if (nodo != NULL && strcmp(nodo->clave,clave) == 0){
		if(hash->destruir_dato){
			hash->destruir_dato(nodo->dato);
		}
		nodo->dato = dato;
		return true;
	}
	nodo_t* nodo_nuevo = crear_nodo(clave,dato);
	lista_insertar_ultimo(hash->tabla[clave_hash],nodo_nuevo);
	hash->cantidad++;
	
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	if (hash->cantidad/hash->capacidad < MIN_ESPACIO_USADO && hash->capacidad > CAPACIDAD_INICIAL){
		if (!hash_redimensionar(hash,hash->capacidad/2)){
			return NULL;
		}
	}
	size_t clave_hash = funcion_hash(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla[clave_hash]);
	nodo_t* nodo = lista_iter_ver_actual(iter);
	while(!lista_iter_al_final(iter) && strcmp(nodo->clave,clave) != 0){
		lista_iter_avanzar(iter);
		nodo = lista_iter_ver_actual(iter);
	}
	if(lista_iter_al_final(iter)){
		lista_iter_destruir(iter);
		return NULL;
	}
	nodo = lista_iter_borrar(iter);
	void* dato = nodo->dato;
	destruir_nodo(nodo,NULL);
	lista_iter_destruir(iter);
	hash->cantidad--;
	
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	nodo_t* nodo = buscar_nodo(hash,clave);
	if(!nodo){
		return NULL;
	}
	return nodo->dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	nodo_t* nodo = buscar_nodo(hash,clave);
	if(!nodo){
		return false;
	}
	return true;
}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	for (int i = 0; i < hash->capacidad;i++){
		while(!lista_esta_vacia(hash->tabla[i])){
			destruir_nodo((nodo_t*)lista_borrar_primero(hash->tabla[i]),hash->destruir_dato);
		}
		lista_destruir(hash->tabla[i],NULL);
	}
	free(hash->tabla);
	free(hash);
}


size_t siguiente_posicion_con_elementos(const hash_t *hash, size_t pos) {
	pos++;
	while(pos < hash->capacidad && lista_esta_vacia(hash->tabla[pos])) {
		pos++;
	}
	return pos;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* iterador = malloc(sizeof(hash_iter_t));
	if (!iterador) {
		return NULL;
	}
	iterador->hash = hash;

	if(iterador->hash->cantidad == 0){
		iterador->pos = iterador->hash->capacidad - 1;
		iterador->iter_actual = NULL;
	} else {
		size_t i= 0;
		if (!lista_esta_vacia(hash->tabla[i])){
			iterador->pos = i;
		}else{
			iterador->pos = siguiente_posicion_con_elementos(hash,i);
		}	
		iterador->iter_actual = lista_iter_crear(hash->tabla[iterador->pos]);
	}

	return iterador;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_iter_al_final(iter)){
		return false;
	}
	
	lista_iter_avanzar(iter->iter_actual);
	if (lista_iter_al_final(iter->iter_actual)){
		lista_iter_destruir(iter->iter_actual);
		iter->pos = siguiente_posicion_con_elementos(iter->hash,iter->pos);
		if (iter->pos == iter->hash->capacidad){
			iter->iter_actual = NULL;
		} else {
			iter->iter_actual = lista_iter_crear(iter->hash->tabla[iter->pos]);
		}
	}
	
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	nodo_t* actual = lista_iter_ver_actual(iter->iter_actual);

	return actual->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if(iter->iter_actual == NULL){return true;}
	return false;
}

void hash_iter_destruir(hash_iter_t* iter) {
	if (iter->iter_actual) {
		lista_iter_destruir(iter->iter_actual);
	}

	free(iter);
}

