/* Copyright 2021 <Baiatu Bianca> */
#include <stdlib.h>
#include <string.h>

#include "Server.h"

server_memory* init_server_memory() {
	server_memory *server = malloc(sizeof(server_memory));
	server->ht = ht_create((unsigned int)10000, hash_servers, compare_strings);
	DIE(server == NULL, "Cannot allocate server");
	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	if(server->ht == NULL) {
		printf("Server not allocated\n");
		exit(-1);
	}
	int k = strlen(key) + 1;
	int v = strlen(value) + 1;
	ht_put(server->ht, key, sizeof(char) * k, value, sizeof(char) * v);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server->ht, key);
}

char* server_retrieve(server_memory* server, char* key) {
	return ht_get(server->ht, key);
}

void free_server_memory(server_memory* server) {
	ht_free(server->ht);
}
