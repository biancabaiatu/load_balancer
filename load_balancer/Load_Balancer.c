/* Copyright 2021 <Baiatu Bianca> */
#include <stdlib.h>
#include <string.h>

#include "Load_Balancer.h"
#include "Hash_Ring.h"

#define REPLIC_ID 100000

struct load_balancer {
	hashtable_t *ht;
    hash_ring_t *hr;
};

unsigned int hash_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer* init_load_balancer() {
	load_balancer *load = malloc(sizeof(load_balancer));
	load->ht = ht_create(1000, hash_servers, compare_function_ints);
	DIE(load == NULL, "Cannot allocate main");
    load->hr = init_hash_ring();
	return load;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
    *server_id = get_server(main->hr, key, hash_string) % REPLIC_ID;
    server_memory *server = ht_get(main->ht, server_id);
    server_store(server, key, value);
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
    *server_id = get_server(main->hr, key, hash_string) % REPLIC_ID;
	server_memory *server = ht_get(main->ht, server_id);
    return server_retrieve(server, key);
}

void loader_add_server(load_balancer* main, int server_id) {
    int dummy_server_id;
	server_memory *new_serv = init_server_memory();
    ht_put(main->ht, &server_id, sizeof(int), new_serv, sizeof(server_memory));
    for (int i = 0; i < 3; i++) {
        add_server_ring(main->hr, i * REPLIC_ID + server_id, hash_int);
    }
	for(int j = 0; j < 3; j++) {
		int replica_id = j * REPLIC_ID + server_id;
		int old_server_id = get_server(main->hr, &replica_id, hash_int) % REPLIC_ID;
		server_memory *server = ht_get(main->ht, &old_server_id);
		for(unsigned int i = 0; i < server->ht->hmax; i++) {
			ll_node_t *node = server->ht->buckets[i]->head;
			while(node != NULL) {
                int length1 = strlen(((struct info *)node->data)->key) + 1;
                int length2 = strlen(((struct info *)node->data)->value) + 1;
				char *key = malloc(sizeof(char) * length1);
				char *value = malloc(sizeof(char) * length2);
				strcpy(key, ((struct info *)node->data)->key);
				strcpy(value, ((struct info *)node->data)->value);
				loader_store(main, key, value, &dummy_server_id);
				free(key);
				free(value);
				node = node->next;
			}
		}
	}
	free(new_serv);
}


void loader_remove_server(load_balancer* main, int server_id) {
    int new_server_id;
    for (int i = 0; i < 3; i++) {
        remove_server_ring(main->hr, server_id + i * REPLIC_ID);
    }
	server_memory *server = ht_get(main->ht, &server_id);
    for (unsigned int i = 0; i < server->ht->hmax; i++) {
        ll_node_t *node = server->ht->buckets[i]->head;
        while (node != NULL) {
            int length1 = strlen(((struct info *)node->data)->key) + 1;
            int length2 = strlen(((struct info *)node->data)->value) + 1;
            char *key = malloc(sizeof(char) * length1);
            char *value = malloc(sizeof(char) * length2);
            strcpy(key, ((struct info *)node->data)->key);
            strcpy(value, ((struct info *)node->data)->value);
            loader_store(main, key, value, &new_server_id);
            free(key);
            free(value);
            node = node->next;
        }
    }
    free_server_memory(server);
    ht_remove_entry(main->ht, &server_id);
}

void free_load_balancer(load_balancer* main) {
    for (unsigned int i = 0; i < main->ht->hmax; i++) {
        ll_node_t *node = main->ht->buckets[i]->head;
        while (node != NULL) {
            free_server_memory(((struct info *)node->data)->value);
            node = node->next;
        }
    }
    ht_free(main->ht);
    free_hr(main->hr);
    free(main);
}
