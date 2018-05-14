#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "hash.h"

#define CAPACIDAD_INICIAL 8
#define MAX_ESPACIO_USADO 2
#define MIN_ESPACIO_USADO 0.5


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
	unsigned h = 0;

	for (int i=0; i<capacidad; i++) {
		h += (unsigned)clave[i];
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

nodo_t* crear_nodo(const char* clave,void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo){
		return NULL;
	}
	nodo->clave = clave;
	nodo->dato = dato;
	
	return nodo;
}

//va a cada elemento y lo rehashe y lo guarda donde corresponde
bool hash_redimensionar(hash_t* hash,size_t new_tam){
	nodo_t* nodo;
	unsigned long clave_hash;
	lista_t** new_tablas = malloc(new_tam * sizeof(lista_t*));
	for (int i = 0; i < new_tam; i++) {
		new_tablas[i] = lista_crear();
	}
	for(int i = 0;i < hash->capacidad;i++){
		while (!lista_esta_vacia(hash->tabla[i])){
			nodo = lista_borrar_primero(hash->tabla[i]);
			clave_hash = funcion_hash(nodo->clave,new_tam);
			lista_insertar_ultimo(new_tablas[clave_hash],nodo);
		}
		lista_destruir(hash->tabla[i], NULL);
	}
	hash->capacidad = new_tam;
	free(hash->tabla);
	hash->tabla = new_tablas;
	return true;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	// Si nos pasamos del limite hay que redimensionarlo
	if (hash->cantidad/hash->capacidad > MAX_ESPACIO_USADO){
		if (!hash_redimensionar(hash,hash->capacidad*2)){
			return false;
		}
	}
	unsigned long clave_hash = funcion_hash(clave,hash->capacidad);
	nodo_t* nodo = crear_nodo(clave,dato);
	if(!lista_insertar_ultimo(hash->tabla[clave_hash],nodo)){
		//si fallo al insertarlo por algun motivo
		return false;
	}
	hash->cantidad++;
	
	return true;
}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave){
	// Si nos pasamos del limite hay que redimensionarlo
	if (hash->cantidad/hash->capacidad < MIN_ESPACIO_USADO && hash->capacidad > CAPACIDAD_INICIAL){
		if (!hash_redimensionar(hash,hash->capacidad/2)){
			return NULL;
		}
	}
	unsigned long clave_hash = funcion_hash(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla[clave_hash]);
	nodo_t* nodo = lista_iter_ver_actual(iter);
	while(!lista_iter_al_final(iter) && nodo->clave != clave){
		//iterar hasta encontrar la clave
		nodo = lista_iter_ver_actual(iter);
		lista_iter_avanzar(iter);
	}
	if(lista_iter_al_final(iter)){
		return NULL;
	}
	nodo = lista_iter_borrar(iter);
	void* dato = nodo->dato;
	free(nodo);
	lista_iter_destruir(iter);
	hash->cantidad--;
	
	return dato;
}

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave){
	unsigned long clave_hash = funcion_hash(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla[clave_hash]);
	nodo_t* nodo = lista_iter_ver_actual(iter);
	while(!lista_iter_al_final(iter) && nodo->clave != clave){
		//iterar hasta encontrar la clave
		nodo = lista_iter_ver_actual(iter);
		lista_iter_avanzar(iter);
	}
	if(lista_iter_al_final(iter)){
		return NULL;
	}
	lista_iter_destruir(iter);
	return nodo->dato;
}

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave){
	unsigned long clave_hash = funcion_hash(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla[clave_hash]);
	nodo_t* nodo;
	while(!lista_iter_al_final(iter)){
		nodo = lista_iter_ver_actual(iter);
		if (nodo->clave == clave){
			return true;
		}
		//iterar hasta encontrar la clave
		lista_iter_avanzar(iter);
	}
	return false;	
}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}

void destruir_nodo(void* nodo,void destruir_dato(void*)){
	nodo_t* nodo_cast = nodo;
	if (destruir_dato){
		destruir_dato(nodo_cast->dato);
	}
	free(nodo);
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
	for (int i = 0; i < hash->capacidad;i++){
		while(!lista_esta_vacia(hash->tabla[i])){
			destruir_nodo(lista_borrar_primero(hash->tabla[i]),hash->destruir_dato);
		}
	}
}



/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/
size_t siguiente_posicion_con_elementos(const hash_t *hash, size_t pos) {
	while(pos < hash->capacidad && lista_esta_vacia(hash->tabla[pos])) {
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
		size_t i= 0;

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
	if((iter->hash->cantidad == 0) || (iter->pos == siguiente_lista && lista_iter_al_final(iter->iter_actual))){
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
	nodo_t* actual = lista_iter_ver_actual(iter->iter_actual);

	return actual->clave;
}

void hash_iter_destruir(hash_iter_t* iter) {
	if (iter->iter_actual) {
		lista_iter_destruir(iter->iter_actual);
	}

	free(iter);
}
