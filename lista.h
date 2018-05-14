#ifndef LISTA_H
#define LISTA_H

#include <stddef.h>
#include <stdbool.h>

struct lista;
typedef struct lista lista_t;

typedef struct lista_iter lista_iter_t;

/* Primitivas de la Lista */

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t* lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t* lista);

// Agrega un nuevo elemento a la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t* lista, void* dato);

// Agrega un nuevo elemento a la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t* lista, void* dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void* lista_borrar_primero(lista_t* lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void* lista_ver_primero(const lista_t* lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void* lista_ver_ultimo(const lista_t* lista);

// Devuelve la cantidad de elementos
// Pre: la lista fue creada.
// Post: se devolvió la longitud de la lista.
size_t lista_largo(const lista_t* lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t* lista, void destruir_dato(void*));

/* Primitivas del Iterador Interno */

// Recorre la lista y le aplica la función visitar dada por parámetro, a cada uno
// de los elementos de la lista. Si visitar devuelve false deja de recorrer la lista.
// Pre: la lista fue creada. visitar es una función capaz de operar
// los datos de la lista.
// Post: Se recorrio la lista hasta que visitar devolvio false.
void lista_iterar(lista_t* lista, bool visitar(void*, void*), void* extra);

/* Primitivas del Iterador externo */

// Crea un iterador externo para la lista dada.
// Post: devuelve un nuevo iterador para la lista dada y apunta al 
// primer elemento de la lista.
lista_iter_t* lista_iter_crear(lista_t* lista);

// Avanza el puntero del iterador en la lista al siguiente elemento en la lista
// Pre: iterador fue creado
// Post: devuelve true si se pudo avanzar en la lista y false si esta al final
bool lista_iter_avanzar(lista_iter_t* iter);

// Devuelvo el dato guardado en la lista en el lugar donde apunta el iterador
// Pre: iterador fue creado
// Post: devuelve el dato al cual el iterador le esta apuntando
void* lista_iter_ver_actual(const lista_iter_t* iter);

// Indica is el iterador llego al final
// Pre: iterador fue creado
// Post: devuelve true si esta al final y false en otro caso
bool lista_iter_al_final(const lista_iter_t* iter);

// Destruye el iterador
// Pre: iterador fue creado
// Post: destruye el iterado
void lista_iter_destruir(lista_iter_t* iter);

// Inserta un elemento en la posicion donde esta apuntando el iterador y el resto de la
// lista sigue despues del elemento recien creado. El iterador apunta al elemento recien creado
// Pre: iterador fue creado y se entrego un dato para guardar
// Post: devuelve true si se pudo insertar el dato y false en otro caso. 
bool lista_iter_insertar(lista_iter_t* iter, void* dato);

// Elimina un elemento en la posicion donde esta apuntando el iterador y el
// resto de la lista sigue despues del elemento anterior. El iterador apunta al elemento siguiente
// Pre: iterador fue creado
// Post: devuelve el dato en el iterador o null si no pudo devolver nada
void* lista_iter_borrar(lista_iter_t* iter);

void pruebas_lista_alumno(void);
void ejemplo_iteradores();

#endif // LISTA_H
