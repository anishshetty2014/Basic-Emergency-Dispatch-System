#include "header.h"

fib_node* MAX_DEGREE_ARRAY[MAX_DEGREE] = {NULL};

void init_fib_heap(fib_heap* f) {
    f->head = NULL;
    f->tail = NULL;
    f->min = NULL;
    f->count = 0;
}

fib_node* create_fib_heap_node(int safe_id, float distance, float x, float y) {
    fib_node* nn = (fib_node*)malloc(sizeof(fib_node));
    if (!nn) return NULL;
    nn->safe_id = safe_id;
    nn->distance = distance;
    nn->x = x;  
    nn->y = y;  
    nn->parent = NULL;
    nn->child = NULL;
    nn->next = NULL;
    nn->prev = NULL;
    nn->degree = 0;
    nn->mark = 0;
    return nn;
}

int insert_fib_heap(fib_heap* f, int id, float dist,float x,float y) {
    fib_node* nn = create_fib_heap_node(id, dist, x, y);
    if (!nn) return -1;
    
    if (f->head == NULL) {
        f->head = nn;
        f->tail = nn;
        f->min = nn;
        nn->next = nn;
        nn->prev = nn;
    } 
    else {
        nn->next = f->head;
        nn->prev = f->tail;
        f->tail->next = nn;
        f->head->prev = nn;
        f->tail = nn;
        
        if (dist < f->min->distance) 
            f->min = nn;
    }
    f->count++;
    return 0;
}

void consolidate(fib_heap* f) {
    for (int i = 0; i < MAX_DEGREE; i++)
        MAX_DEGREE_ARRAY[i] = NULL;
    
    fib_node* current = f->head;
    if (!current) return;
    
    do {
        fib_node* x = current;
        int d = x->degree;
        
        while (MAX_DEGREE_ARRAY[d] != NULL) {
            fib_node* y = MAX_DEGREE_ARRAY[d];
            if (x->distance > y->distance) {
                fib_node* temp = x;
                x = y;
                y = temp;
            }
            
            if (y == f->head) f->head = y->next;
            if (y == f->tail) f->tail = y->prev;
            y->prev->next = y->next;
            y->next->prev = y->prev;
            
            y->parent = x;
            y->next = x->child;
            if (x->child) x->child->prev = y;
            x->child = y;
            x->degree++;
            y->mark = 0;
            
            MAX_DEGREE_ARRAY[d] = NULL;
            d++;
        }
        MAX_DEGREE_ARRAY[d] = x;
        current = current->next;
    } 
    while (current != f->head);
    
    f->head = NULL;
    f->min = NULL;
    
    for (int i = 0; i < MAX_DEGREE; i++) {
        if (MAX_DEGREE_ARRAY[i]) {
            if (f->head == NULL) {
                f->head = MAX_DEGREE_ARRAY[i];
                f->head->next = f->head;
                f->head->prev = f->head;
                f->tail = f->head;
            } 
            else {
                f->tail->next = MAX_DEGREE_ARRAY[i];
                MAX_DEGREE_ARRAY[i]->prev = f->tail;
                MAX_DEGREE_ARRAY[i]->next = f->head;
                f->head->prev = MAX_DEGREE_ARRAY[i];
                f->tail = MAX_DEGREE_ARRAY[i];
            }
            
            if (f->min == NULL || MAX_DEGREE_ARRAY[i]->distance < f->min->distance) f->min = MAX_DEGREE_ARRAY[i];
        }
    }
}

fib_node* extract_min(fib_heap* f) {
    if (f->min == NULL) return NULL;
    
    fib_node* min_node = f->min;
    fib_node* child = min_node->child;
    
    while (child) {
        fib_node* next_child = child->next;
        
        if (child->next == child) min_node->child = NULL;
        else {
            child->prev->next = child->next;
            child->next->prev = child->prev;
            if (min_node->child == child)
                min_node->child = child->next;
        }
        
        child->parent = NULL;
        child->prev = f->tail;
        child->next = f->head;
        f->tail->next = child;
        f->head->prev = child;
        f->tail = child;
        
        child = next_child;
    }
    
    if (min_node->next == min_node) {
        f->head = NULL;
        f->tail = NULL;
        f->min = NULL;
    } 
    else {
        min_node->prev->next = min_node->next;
        min_node->next->prev = min_node->prev;
        
        if (f->head == min_node) f->head = min_node->next;
        if (f->tail == min_node) f->tail = min_node->prev;
        
        f->min = min_node->next; 
    }
    
    f->count--;
    if (f->head) consolidate(f);
    
    return min_node;
}

void cut(fib_heap* f, fib_node* x, fib_node* y) {
    if (x->next == x) y->child = NULL;
    else {
        x->prev->next = x->next;
        x->next->prev = x->prev;
        if (y->child == x) y->child = x->next;
    }
    y->degree--;
    
    x->parent = NULL;
    x->mark = 0;
    x->prev = f->tail;
    x->next = f->head;
    f->tail->next = x;
    f->head->prev = x;
    f->tail = x;
    
    if (x->distance < f->min->distance) f->min = x;
}

void cascading_cut(fib_heap* f, fib_node* y) {
    fib_node* z = y->parent;
    if (z) {
        if (!y->mark) y->mark = 1;
        else {
            cut(f, y, z);
            cascading_cut(f, z);
        }
    }
}

void decrease_key(fib_heap* f, fib_node* x, float new_distance) {
    if (new_distance > x->distance) return;
    x->distance = new_distance;
    fib_node* y = x->parent;
    
    if (y && x->distance < y->distance) {
        cut(f, x, y);
        cascading_cut(f, y);
    }
    
    if (x->distance < f->min->distance) f->min = x;
}

void delete_node(fib_heap* f, fib_node* x) {
    decrease_key(f, x, -INFINITY);
    fib_node* extracted = extract_min(f);
    if (extracted) free(extracted);
}

void display_fib_heap(fib_node* node, int level, fib_node* start) {
    if (node == NULL) return;
    
    fib_node* current = node;
    do {
        for (int i = 0; i < level; i++) printf("    ");
        printf("ID: %d, Distance: %.2f, Coordinates: (%.2f, %.2f), Degree: %d, Mark: %d\n", 
               current->safe_id, current->distance, current->x, current->y, 
               current->degree, current->mark);
        
        if (current->child) {display_fib_heap(current->child, level + 1, current->child);
        
        current = current->next;
        } 
    }
    while (current != node && current != start);
}

void init_treap(treap* t) {
    *t = NULL;
}

treap_node* create_treap_node(int id, int priority, int timestamp) {
    treap_node* nn = (treap_node*)malloc(sizeof(treap_node));
    if (!nn) return NULL;
    nn->id = id;
    nn->priority = priority;
    nn->timestamp = timestamp;
    nn->left = NULL;
    nn->right = NULL;
    nn->parent = NULL;
    return nn;
}

void rotate_left(treap* t, treap_node* x) {
    treap_node* y = x->right;
    if (!y) return;

    x->right = y->left;
    if (y->left) y->left->parent = x;

    y->parent = x->parent;
    if (!x->parent) *t = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rotate_right(treap* t, treap_node* x) {
    treap_node* y = x->left;
    if (!y) return;

    x->left = y->right;
    if (y->right) y->right->parent = x;

    y->parent = x->parent;
    if (!x->parent) *t = y;
    else if (x == x->parent->right) x->parent->right = y;
    else x->parent->left = y;

    y->right = x;
    x->parent = y;
}

void balance_treap(treap* t, treap_node* curr) {
    while (curr && curr->parent) {
        if (curr->priority > curr->parent->priority || 
           (curr->priority == curr->parent->priority && curr->timestamp < curr->parent->timestamp)) {
            
            if (curr == curr->parent->left)rotate_right(t, curr->parent);
            else rotate_left(t, curr->parent); 
        } 
        else break; 
    }
}

int insert_treap(treap* t, int id, int priority, int timestamp) {
    treap_node* nn = create_treap_node(id, priority, timestamp);
    if (!nn) return -1;

    if (*t == NULL) {
        *t = nn;
        return 0;
    }

    treap_node* p = *t;
    treap_node* q = NULL;
    while (p) {
        q = p;
        if (p->id > id) p = p->left;
        else if (p->id < id) p = p->right;
        else {
            free(nn);
            return -1; 
        }
    }

    if (q->id > id) q->left = nn;
    else q->right = nn;

    nn->parent = q;
    balance_treap(t, nn);

    return 0;
}

treap_node* search_treap(treap t, int id) {
    treap_node* p = t;
    while (p) {
        if (p->id > id) p = p->left;
        else if (p->id < id) p = p->right;
        else return p;
    }
    return NULL;
}

int remove_treap(treap* t, int id) {
    treap_node* target = search_treap(*t, id);
    if (!target) return -1;

    while (target->left || target->right) {
        if (!target->left) rotate_left(t, target);
        else if (!target->right) rotate_right(t, target);
        else if (target->left->priority > target->right->priority) rotate_right(t, target);
        else if (target->left->priority < target->right->priority) rotate_left(t, target);
        else { 
            if (target->left->timestamp < target->right->timestamp) rotate_right(t, target);
            else rotate_left(t, target);
        }
    }

    if (target->parent) {
        if (target == target->parent->left) target->parent->left = NULL;
        else target->parent->right = NULL;
    } 
    else *t = NULL;  

    free(target);
    return 0;
}

void display_treap(treap t, int level) {
    if (t == NULL) return;
    
    display_treap(t->right, level + 1);
    
    for (int i = 0; i < level; i++) printf("    ");
    printf("ID: %d, Priority: %d, Timestamp: %d\n", t->id, t->priority, t->timestamp);
    
    display_treap(t->left, level + 1);
}