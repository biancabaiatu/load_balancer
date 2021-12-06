/* Copyright 2021 <Baiatu Bianca> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CircularDoublyLinkedList.h"
#include "utils.h"

// functie care creeaza o lista dublu inlantuita circulara si o initializeaza

dll_t* dll_create(int data_size)
{
    dll_t *list;
    list = malloc(sizeof(dll_t));
    DIE(list == NULL, "Cannot alloc");
    list->data_size = data_size;
	list->head = NULL;
	list->size = 0;
    return list;
}

// functie care returneaza nodul de pe pozitia n

dll_node_t* get_node(dll_t* list, int n)
{
    int k;
    dll_node_t *node;
    if (!list) {
        printf("No list\n");
        return NULL;
    }
    if (n < 0) {
        printf("Wrong position\n");
        return NULL;
    }
    if (list->size == 0) {
        n = 0;
    } else {
        n = n % list->size;
    }
    node = list->head;
    for (k = 1; k <= n; k++) {
        node = node->next;
    }
    return node;
}

// functie care adauga un nod in lista pe pozitia n

void dll_add_nth_node(dll_t* list, int n, const void* new_data)
{
    if (!list) {
        printf("No list\n");
        return;
    }
    if (n < 0) {
        printf("Wrong position\n");
        return;
    }
    dll_node_t *new = malloc(sizeof(dll_node_t));
    DIE(new == NULL, "Cannot allocate");
    new->data = malloc(list->data_size);
    memcpy(new->data, new_data, list->data_size);
    if (n == 0) {
        if (list->head == NULL) {
            list->head = new;
            new->next = new;
            new->prev = new;
        } else {
            new->next = list->head;
            new->prev = list->head->prev;
            list->head->prev->next = new;
            list->head->prev = new;
            list->head = new;
        }
    } else if (n < list->size){
        int k = 0;
        dll_node_t *nod = list->head;
        if (nod == NULL) {
            list->head = new;
            new->next = new;
            new->prev = new;
        } else {
            while (k < n - 1) {
                nod = nod->next;
                k++;
            }
            nod->next->prev = new;
            new->next = nod->next;
            nod->next = new;
            new->prev = nod;
        }
    } else {
        if (list->head == NULL) {
            list->head = new;
            new->next = new;
            new->prev = new;
        } else {
            new->next = list->head;
            new->prev = list->head->prev;
            list->head->prev->next = new;
            list->head->prev = new;
        }
    }
    list->size++;
}

// functie care sterge din lista nodul de pe pozitia n si il returneaza

dll_node_t* dll_remove_nth_node(dll_t* list, int n)
{
    if (!list) {
        printf("No list\n");
        return NULL;
    }
    if (n < 0) {
        printf("Wrong position\n");
        return NULL;
    }
    if (n == 0) {
        dll_node_t *nod = list->head;
        if (!nod) {
            printf("Does not exist\n");
            return NULL;
        }
        nod->next->prev = nod->prev;
        nod->prev->next = nod->next;
        list->head = nod->next;
        list->size--;
        if (list->size == 0) {
            list->head = NULL;
        }
        return nod;
    } else if (n >= list->size - 1) {
        dll_node_t *nod = list->head->prev;
        if (!nod) {
            printf("Does not exist\n");
            return NULL;
        }
        nod->next->prev = nod->prev;
        nod->prev->next = nod->next;
        list->size--;
        if (list->size == 0) {
            list->head = NULL;
        }
        return nod;
    } else {
        int k = 0;
        dll_node_t *nod = list->head;
        if (nod == NULL) {
            printf("Does not exist\n");
            return NULL;
        } else {
            while (k <= n - 1) {
                nod = nod->next;
                k++;
            }
            nod->next->prev = nod->prev;
            nod->prev->next = nod->next;
            list->size--;
            return nod;
        }
    }
}

// functie care returneaza dimensiunea listei

int dll_get_size(dll_t* list)
{
    if (!list) {
        printf("List does not exist\n");
        exit(-1);
    }
    return list->size;
}

// functie care goleste memoria alocata unei liste

void dll_free(dll_t** pp_list)
{
    int i;
    dll_node_t *nod;
    dll_t *list = *pp_list;
    if (list->size == 0) {
        free(list);
        list = NULL;
        return;
    }
    if (list->size == 1) {
        free(list->head->data);
        free(list->head);
        free(list);
        list = NULL;
        return;
    }
    nod = list->head;
    for (i = 1; i < list->size; i++) {
        nod = nod->next;
        free(nod->prev->data);
        free(nod->prev);
    }
    free(nod->data);
    free(nod);
    free(list);
    list = NULL;
}

void dll_print_int_list(dll_t* list)
{
    int i;
    dll_node_t *nod = list->head;
    if (!nod) {
        printf("Does not exist\n");
        return;
    }
    for (i = 0; i < list->size; i++) {
        printf("%d ", *((int *)nod->data));
        nod = nod->next;
    }

    printf("\n");
}

