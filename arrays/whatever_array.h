// vim: ft=c
/* how to use:

first make your array type with these members

struct ARRAY {
    void* items;
    int count;
    int capacity;

};


*/

#ifndef INCLUDE_ARRAYS_WHATEVER_ARRAY_H_
#define INCLUDE_ARRAYS_WHATEVER_ARRAY_H_

#include <stdlib.h>
#include <stdio.h>

#define whatever_array_init(whatever_array_ref, size) do {                         \
    whatever_array_ref.capacity = size;                                            \
    whatever_array_ref.items = malloc(sizeof(whatever_array_append.items) * size); \
    if (whatever_array_ref.items == NULL) {                                        \
        fprintf(stderr, "whatever_array_init: failed to init\n");                  \
        exit(-1);                                                                  \
    }                                                                              \
} while(0)

#define whatever_array_append(whatever_array_ref, element) do {\
    \
} while(0)


#endif  // INCLUDE_ARRAYS_WHATEVER_ARRAY_H_
