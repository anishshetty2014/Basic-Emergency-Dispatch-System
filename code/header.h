#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<math.h>
#include<string.h>

// Treap Functions and Struct definition

typedef struct treap_node{
    int timestamp;
    int priority;
    int id;
    struct treap_node* right;
    struct treap_node* left;
    struct treap_node* parent;
}treap_node;

typedef treap_node* treap;

void init_treap(treap*);
treap_node* create_treap_node(int,int,int);
int insert_treap(treap*,int,int,int);
int remove_treap(treap*,int);
void balance_treap(treap*,treap_node*);
void rotate_right(treap*,treap_node*);
void rotate_left(treap*,treap_node*);
treap_node* search_treap(treap,int);
void display_treap(treap, int);

// Fibonacci Heap Functions and Struct definition

typedef struct fib_node{
    int safe_id;
    float distance;
    float x; 
    float y; 
    struct fib_node* parent;
    struct fib_node* child;
    struct fib_node* next;
    struct fib_node* prev;
    int degree;
    int mark;
}fib_node;

typedef struct fib_heap{
    fib_node* head;
    fib_node* tail;
    fib_node* min;
    int count;
}fib_heap;

void init_fib_heap(fib_heap*);
fib_node* create_fib_heap_node(int,float,float,float);
int insert_fib_heap(fib_heap*,int,float,float,float);
void consolidate(fib_heap*);
fib_node* extract_min(fib_heap*);
void cut(fib_heap*,fib_node*,fib_node*);
void cascading_cut(fib_heap*,fib_node*);
void decrease_key(fib_heap*,fib_node*,float);
void delete_node(fib_heap*,fib_node*);
void display_fib_heap(fib_node*,int,fib_node*);

// Element to store records of individual

typedef struct element{
    float x;
    float y;
    fib_heap* f;
}element;

#define MAX_DEGREE 10
#define MAX_ELEMENTS 100

element ARRAY[MAX_ELEMENTS];