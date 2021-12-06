/* Copyright 2021 <Baiatu Bianca> */
#ifndef HASH_RING_H_
#define HASH_RING_H_

#include "CircularDoublyLinkedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct hash_rings {
    dll_t *hash_ring;
}hash_ring_t;

typedef struct hash_ring_node {
    unsigned int hash;
    unsigned int server_id;
}hash_ring_node_t;

hash_ring_t* init_hash_ring();
void add_server_ring(hash_ring_t *hr,
                     unsigned int server_id,
                      unsigned int (*hash_function)(void*));
int get_server_position(hash_ring_t *hr, unsigned int server_id);
void remove_server_ring(hash_ring_t *hr, unsigned int server_id);
int get_server(hash_ring_t *hr,
                 void *key,
                  unsigned int (*hash_function)(void*));
void free_hr(hash_ring_t *hr);

#endif  /* HASH_RING_H_ */
