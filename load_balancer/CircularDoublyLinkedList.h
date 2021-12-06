/* Copyright 2021 <Baiatu Bianca> */
#ifndef CIRCULARDOUBLYLINKEDLIST_H_
#define CIRCULARDOUBLYLINKEDLIST_H_
#define STRING_SIZE 256

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data;
    dll_node_t *prev, *next;
};

typedef struct doubly_linked_list
{
    dll_node_t* head;
    int data_size;
    int size;
}dll_t;

typedef struct planet {
    char name[STRING_SIZE];
    int kill;
    dll_t *shields;
    int nr_shields;
}planet_t;

dll_t* dll_create(int data_size);

dll_node_t* get_node(dll_t* list, int n);

void dll_add_nth_node(dll_t* list, int n, const void* data);

dll_node_t* dll_remove_nth_node(dll_t* list, int n);

int dll_get_size(dll_t* list);

void dll_free(dll_t** pp_list);

void dll_print_int_list(dll_t* list);

void dll_print_string_list(dll_t* list);

void dll_print_ints_left_circular(dll_node_t* start);

void dll_print_ints_right_circular(dll_node_t* start);
#endif  // CIRCULARDOUBLYLINKEDLIST_H_
