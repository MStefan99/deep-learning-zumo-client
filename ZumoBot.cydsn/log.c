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

#include <log.h>

list new_list() {
    element *first = NULL;
    element *last = NULL;

    list list = {first, last, 0};
    return list;
}


element *list_alloc_element(char *str, element *prev, element *next) {
    element *e = malloc(sizeof(element));
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

    strcpy(e->str, str);
    return e;
}


int list_append(list *l, char *str) {
    element *e = list_alloc_element(str, l->last, NULL);
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


int list_appendLeft(list *l, char *str) {
    element *e = list_alloc_element(str, NULL, l->first);
    if (!e) {
        return 1;
    }

    if (!l->last) {
        l->last = e;
    }

    if (l->first) {
        l->first->prev = e;
    }

    l->first = e;
    ++l->e_count;
    return 0;
}


int list_pop(list *l, char *str) {
    element *e = l->last;
    if (e) {
        strcpy(str, e->str);

        if (e->prev) {
            e->prev->next = NULL;
        } else {
            l->first = NULL;
        }
        l->last = e->prev;
        free(e);
        return 0;
    } else {
        return 1;
    }
}


int list_popLeft(list *l, char *str) {
    element *e = l->first;
    if (e) {
        strcpy(str, e->str);

        if (e->next) {
            e->next->prev = NULL;
        } else {
            l->last = NULL;
        }

        l->first = e->next;
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
        free(t);
    }

    l->first = NULL;
    l->last = NULL;
    l->e_count = 0;
}


int list_get_size(list l) {
    return l.e_count;
}


void list_printAll(list l) {
    int i = 0;
    if (!l.first) {
        print_mqtt("Zumo/log", "List empty");
    } else {
        print_mqtt("Zumo/log", "=== LOG START ===");
        for (element *p = l.first; p; p = p->next, i++) {
            print_mqtt("Zumo/log", "Entry %i is \"%s\"", i + 1, p->str);
        }
        print_mqtt("Zumo/log", "=== LOG END ===");
    }
}

/* [] END OF FILE */
