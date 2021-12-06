/* "Copyright 2021 <Baiatu Bianca>" */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "Hashtable.h"

#define MAX_BUCKET_SIZE 64

/*
 * Functii de comparare a cheilor:
 */
int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
unsigned int
hash_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int
hash_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char *) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare)(void*, void*))
{
	hashtable_t *ht = malloc(sizeof(hashtable_t));
	ht->hmax = hmax;
	ht->size = 0;
	ht->hash_function = hash_function;
	ht->compare = compare;
	ht->buckets = malloc(hmax * sizeof(linked_list_t *));
	for(unsigned int i = 0; i < hmax; i++) {
		ht->buckets[i] = ll_create(sizeof(struct info));
	}
	return ht;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune tipul ei), in momentul in care
 * se creeaza o noua intrare in hashtable (in cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie
 * a valorii la care pointeaza key si adresa acestei copii trebuie salvata in structura info asociata intrarii din ht.
 * Pentru a sti cati octeti trebuie alocati si copiati, folositi parametrul key_size_bytes.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel put(ht, key_actual, value_actual),
 * valoarea la care pointeaza key_actual poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia unei intrari din hashtable.
 * Nu ne dorim acest lucru, fiindca exista riscul sa ajungem in situatia in care nu mai stim la ce cheie este
 * inregistrata o anumita valoare.
 */
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	if(ht == NULL) {
		printf("hashtable doesn't exist\n");
		exit(-1);
	}
	if(key == NULL) {
		printf("key doesn't exist\n");
		exit(-1);
	}
	if(value == NULL) {
		printf("value doesn't exist\n");
		exit(-1);
	}
	unsigned int index = ht->hash_function(key);
	index = index % ht->hmax;
	int size_bucket = ht->buckets[index]->size;
	ll_node_t *nod = ht->buckets[index]->head;
	int i;
	for(i = 0; i < size_bucket - 1; i++) {
		if(ht->compare(key, ((struct info *)nod->data)->key) == 0) {
			memcpy(((struct info *)nod->data)->value, value, value_size);
			return;
		}
		nod = nod->next;
	}
	if(nod != NULL && ht->compare(key, ((struct info *)nod->data)->key) == 0) {
		memcpy(((struct info *)nod->data)->value, value, value_size);
		return;
	}
	struct info* data = malloc(sizeof(struct info));
	data->key = malloc(key_size);
	memcpy(data->key, key, key_size);
	data->value = malloc(value_size);
	memcpy(data->value, value, value_size);
	ll_add_nth_node(ht->buckets[index], ht->buckets[index]->size, data);
	ht->size++;
	free(data);
}

void *
ht_get(hashtable_t *ht, void *key)
{
	if(ht == NULL) {
		printf("hashtable doesn't exist\n");
		exit(-1);
	}
	unsigned int index = ht->hash_function(key);
	index = index % ht->hmax;
	unsigned int size_bucket = ht->buckets[index]->size;
	ll_node_t *nod = ht->buckets[index]->head;
	for(unsigned int i = 0; i < size_bucket; i++) {
		if(ht->compare(key, ((struct info *)nod->data)->key) == 0) {
			return ((struct info *)nod->data)->value;
		}
		nod = nod->next;
	}
	return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int
ht_has_key(hashtable_t *ht, void *key)
{
	if(ht == NULL) {
		printf("hashtable doesn't exist\n");
		exit(-1);
	}
	unsigned int index = ht->hash_function(key);
	index = index % ht->hmax;
	unsigned int size_bucket = ht->buckets[index]->size;
	ll_node_t *nod = ht->buckets[index]->head;
	for(unsigned int i = 0; i < size_bucket; i++) {
		if(ht->compare(key, ((struct info *)nod->data)->key) == 0) {
			return 1;
		}
		nod = nod->next;
	}
	return 0;
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o intrare din hashtable (adica memoria
 * pentru copia lui key --vezi observatia de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void
ht_remove_entry(hashtable_t *ht, void *key)
{
	if(ht == NULL) {
		printf("hashtable doesn't exist\n");
		exit(-1);
	}
	if(key == NULL) {
		printf("key doesn't exist\n");
		exit(-1);
	}
	unsigned int index = ht->hash_function(key);
	index = index % ht->hmax;
	unsigned int size_bucket = ht->buckets[index]->size;
	ll_node_t *nod = ht->buckets[index]->head;
	for(unsigned int i = 0; i < size_bucket; i++) {
		if(ht->compare(key, ((struct info *)nod->data)->key) == 0) {
			ll_node_t *removed = ll_remove_nth_node(ht->buckets[index], i);
			ht->size--;
			free(((struct info *)removed->data)->key);
			free(((struct info *)removed->data)->value);
			free(removed->data);
			free(removed);
			return;
		}
		nod = nod->next;
	}
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void
ht_free(hashtable_t *ht)
{
	for(unsigned int i = 0; i < ht->hmax; i++) {
		ll_node_t* currNode;
		linked_list_t *pp_list = ht->buckets[i];
		while (ll_get_size(pp_list) > 0) {
			currNode = ll_remove_nth_node(pp_list, 0);
			free(((struct info *)currNode->data)->key);
			free(((struct info *)currNode->data)->value);
			free(currNode->data);
			free(currNode);
		}
		ll_free(&pp_list);
		ht->buckets[i] = NULL;
	}
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
