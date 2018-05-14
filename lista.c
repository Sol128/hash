#include "lista.h"
#include <stdlib.h>
#include <stddef.h>

struct nodo {
    void* dato;
    struct nodo* proximo;
};

typedef struct nodo nodo_t;

struct lista {
    nodo_t* primero;
	nodo_t* ultimo;
	size_t tam;
};

struct lista_iter {
    lista_t* lista;
	nodo_t* anterior;
	nodo_t* actual;
};

nodo_t* nodo_crear(void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (nodo != NULL){ 
		nodo->dato = dato;
	}
	return nodo;
}

lista_t* lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));
	
	if (lista == NULL) {
        return NULL;
    }
	
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->tam = 0;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista){
	if (lista->tam == 0){
		return true;
	}
	return false;
}

bool lista_insertar_primero(lista_t *lista, void* dato){
	nodo_t* nodo = nodo_crear(dato);
	if(nodo == NULL){
		return false;
	}
	nodo->proximo = lista->primero;
	lista->primero = nodo;	
	if (lista->ultimo == NULL){
		lista->ultimo = nodo;
	}
	lista->tam++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void* dato){
	nodo_t* nodo = nodo_crear(dato);
	if(nodo == NULL){
		return false;
	}
	nodo->proximo = NULL;
	if (!lista->primero){
		lista->primero = nodo;
	}
	if (lista->ultimo){
		lista->ultimo->proximo = nodo;
	}
	lista->ultimo = nodo;
	lista->tam++;
	return true;
}

void* lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista)){
		return NULL;
	}
	if (lista->primero == lista->ultimo){
		lista->ultimo = NULL;
	}
	nodo_t* nodo = lista->primero;
	lista->tam--;
	lista->primero = nodo->proximo;
	void* dato = nodo->dato;
	free(nodo);
	return dato;
}

void* lista_ver_primero(const lista_t *lista){
	if (lista_esta_vacia(lista)){
		return NULL;
	}
	
	return lista->primero->dato;
}

void* lista_ver_ultimo(const lista_t *lista){
	if (lista_esta_vacia(lista)){
		return NULL;
	}
	return lista->ultimo->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->tam;
}

void lista_destruir(lista_t *lista, void destruir_dato(void*)){
	nodo_t* nodo;

	while (lista->primero){
		nodo = lista->primero;
		if(destruir_dato){
			destruir_dato(nodo->dato);
		}
		lista->primero = nodo->proximo;
		free(nodo);
	}
	
	free(lista);
}

void lista_iterar(lista_t* lista, bool visitar(void*, void*), void* extra){
	int i = 0;
	nodo_t* nodo;
	nodo = lista->primero;
	while (i<lista->tam && visitar(nodo->dato, extra)){
		nodo = nodo->proximo;
		i++;
	}
}

lista_iter_t* lista_iter_crear(lista_t* lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (iter != NULL){ 
		iter->lista = lista;
		iter->actual = lista->primero;
		iter->anterior = NULL;
	}
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (iter->actual != NULL){
		iter->anterior = iter->actual;
		iter->actual = iter->actual->proximo;
		return true;
	}
	return false;
}

void* lista_iter_ver_actual(const lista_iter_t *iter){
	if (!lista_iter_al_final(iter)){
		return iter->actual->dato;
	}
	return NULL;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	if (iter->actual == NULL){
		return true;
	}
	return false;
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if(nodo == NULL){
		return false;
	}
	nodo->proximo = iter->actual;
	if (lista_esta_vacia(iter->lista)){
		iter->lista->primero = nodo;
		iter->lista->ultimo = nodo;
		iter->anterior = NULL;
	} else {
		if (lista_iter_al_final(iter)){
			iter->anterior->proximo = nodo;
			iter->lista->ultimo = nodo;
		} else if (iter->anterior != NULL){
			iter->anterior->proximo = nodo;
		} else {
			iter->lista->primero = nodo;
		}
	}
	iter->actual = nodo;

	iter->lista->tam++;
	return true;
}

void* lista_iter_borrar(lista_iter_t *iter){
	if (lista_iter_al_final(iter)){
		return NULL;
	}
	void* dato = iter->actual->dato;
	if (iter->lista->primero == iter->actual){
		iter->lista->primero = iter->actual->proximo;
	}
	if (iter->lista->ultimo == iter->actual){
		iter->lista->ultimo = iter->anterior;
	}
	if (iter->anterior != NULL){
		iter->anterior->proximo = iter->actual->proximo;
	}
	free(iter->actual);
	if (iter->anterior == NULL){
		iter->actual = iter->lista->primero;
	} else {
		iter->actual = iter->anterior->proximo;
	}
	iter->lista->tam--;
	return dato;
}

