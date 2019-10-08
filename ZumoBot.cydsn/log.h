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
    #include <FreeRTOS.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <mqtt_sender.h>
    
    #define LOG_LENGTH 32  // The longer the entry, the less entries will fit in memory
    
    typedef struct node_ {
        char str[LOG_LENGTH];
        struct node_ *next;
        struct node_ *prev;
    } element;


    typedef struct list_ {
        element *first;
        element *last;
        int e_count;
    } list;


    list new_list();
    element *list_alloc_element(char *str, element *prev, element *next);

    int list_append(list *l, char *str);
    int list_appendLeft(list *l, char *str);
    int list_pop(list *l, char *str);
    int list_popLeft(list *l, char *str);
    void list_wipe(list *l);

    int list_get_size(list l);
    void list_printAll(list l);

#endif

/* [] END OF FILE */