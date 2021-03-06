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

#ifndef LOG_H_
    #define LOG_H_
    
    #include <string.h>
    #include <stdlib.h>
    #include "smqtt.h"

    typedef struct node_ {
        void *content;
        struct node_ *next;
        struct node_ *prev;
    } element;

    typedef struct list_ {
        element *first;
        element *last;
        int e_count;
    } list;

    
    list new_list();
    
    int list_append(list *l, const void *content, size_t size);
    int list_pop(list *l, void *content, size_t size);
    void list_wipe(list *l);
    int list_get_size(list l);
    void list_print_all(list l, void (*print_element)(const void *element));

#endif

/* [] END OF FILE */