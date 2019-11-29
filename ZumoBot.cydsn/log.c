/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "log.h"


element *list_alloc_element(element *prev, element *next, const void *content, size_t size);
    

list new_list() {
    element *first = NULL;
    element *last = NULL;

    list list = {first, last, 0};
    return list;
}


element *list_alloc_element(element *prev, element *next, const void *content, size_t size) {
    element *e = malloc(sizeof(element));
    void *c = malloc(size);
    if (!e) {
        return NULL;
    }

    if (prev) {
        e->prev = prev;
    } else {
        e->prev = NULL;
    }
    if (next) {
        e->next = next;
    } else {
        e->next = NULL;
    }

    memcpy(c, content, size);
    e->content = c;
    return e;
}


int list_append(list *l, const void *content, size_t size) {
    element *e = list_alloc_element(l->last, NULL, content, size);
    if (!e) {
        return 1;
    }

    if (!l->first) {
        l->first = e;
    }

    if (l->last) {
        l->last->next = e;
    }

    l->last = e;
    ++l->e_count;
    return 0;
}


int list_pop(list *l, void *content, size_t size) {
    element *e = l->last;
    if (e) {
        memcpy(content, e->content, size);

        if (e->prev) {
            e->prev->next = NULL;
        } else {
            l->first = NULL;
        }
        l->last = e->prev;

        --l->e_count;
        free(e->content);
        free(e);
        return 0;
    } else {
        return 1;
    }
}


void list_wipe(list *l) {
    element *p = l->first;
    element *t;

    while (p) {
        t = p;
        p = p->next;
        free(t->content);
        free(t);
    }

    l->first = NULL;
    l->last = NULL;
    l->e_count = 0;
}


int list_get_size(list l) {
    return l.e_count;
}


void list_print_all(list l, void (*print_element)(const void *)) {
    int i = 0;
    if (!l.first) {
        mqtt_print("Log/Zumo", "List empty");
    } else {
        mqtt_print("Log/Zumo", "List elements:");
        for (element *p = l.first; p; p = p->next, i++) {
            print_element(p->content);
        }
    }
}

/* [] END OF FILE */
