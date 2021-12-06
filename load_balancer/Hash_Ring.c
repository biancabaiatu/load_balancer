/* Copyright 2021 <Baiatu Bianca> */

#include "Hash_Ring.h"



hash_ring_t* init_hash_ring() {
    hash_ring_t *hr = malloc(sizeof(hash_ring_t));
    hr->hash_ring = dll_create(sizeof(hash_ring_node_t));
    return hr;
}

void add_server_ring(hash_ring_t *hr,
                     unsigned int server_id,
                      unsigned int (*hash_function)(void*))
{
    int ok = 0;
    dll_node_t *node = hr->hash_ring->head;
    unsigned int hash = hash_function(&server_id);
    hash_ring_node_t *new_node = malloc(sizeof(hash_ring_node_t));
    new_node->hash = hash;
    new_node->server_id = server_id;
    for (int i = 0; i < hr->hash_ring->size && ok == 0; i++) {
        if (hash < ((hash_ring_node_t*)node->data)->hash) {
            dll_add_nth_node(hr->hash_ring, i, new_node);
            ok = 1;
        }
        node = node->next;
    }
    if (ok == 0) {
        dll_add_nth_node(hr->hash_ring, hr->hash_ring->size, new_node);
    }
    free(new_node);
}

int get_server_position(hash_ring_t *hr, unsigned int server_id)
{
    dll_node_t *node = hr->hash_ring->head;
    for (int i = 0; i < hr->hash_ring->size; i++) {
        if (server_id == ((hash_ring_node_t*)node->data)->server_id) {
            return i;
        }
        node = node->next;
    }
    return -1;
}

void remove_server_ring(hash_ring_t *hr, unsigned int server_id)
{
    int pos = get_server_position(hr, server_id);
    if (pos == -1) {
        printf("Hash ring error. Invalid server_id\n");
        exit(-1);
    }
    dll_node_t *removed_server = dll_remove_nth_node(hr->hash_ring, pos);
    free(removed_server->data);
    free(removed_server);
}

int get_server(hash_ring_t *hr,
                             void *key,
                              unsigned int (*hash_function)(void*))
{
    dll_node_t *node = hr->hash_ring->head;
    unsigned int hash = hash_function(key);
    for (int i = 0; i < hr->hash_ring->size; i++) {
        if (hash < ((hash_ring_node_t*)node->data)->hash) {
            return ((hash_ring_node_t*)node->data)->server_id;
        }
        node = node->next;
    }
    return (((hash_ring_node_t*)node->data)->server_id);
}

void free_hr(hash_ring_t *hr)
{
    dll_free(&hr->hash_ring);
    free(hr);
}
