/*
    Author: gustavopezzi
    Link: https://github.com/gustavopezzi/dynamicarray/tree/master

    This code is a fork of Zhou Le's darray.h. 
    original MIT license for Zhou's code : 
    https://github.com/zauonlok/renderer/blob/master/LICENSE

*/


#ifndef ARRAY_H
#define ARRAY_H

#define array_push(array, value)                                              \
    do {                                                                      \
        (array) = array_hold((array), 1, sizeof(*(array)));                   \
        (array)[array_length(array) - 1] = (value);                           \
    } while (0);

void* array_hold(void* array, int count, int item_size);
int array_length(void* array);
void array_free(void* array);

#endif
