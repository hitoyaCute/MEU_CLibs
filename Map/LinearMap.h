// vim: ft=c
/***********************************************************************
type of map where we use linear search

how to use

// first create a type
LINEARMAP_TYPE(const char*, float) char_float_map;     // create key_value pair
LINEARMAP_NAME_TYPE(const char*, int, char_int_map) d; // create the type and specify typename


***********************************************************************/

#ifndef INCLUDE_MAP_HASHMAP_H_
#define INCLUDE_MAP_HASHMAP_H_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define OR ||
#define AND &&
#define NOT !

#ifndef MAP_INIT_SIZE
#define MAP_INIT_SIZE 16
#endif

typedef struct linearmap_header_impl {
    // function that compares the data of 2 ref of values
    int (*compare_func)(void*, void*);
    // the index to the last valid element
    size_t last_elem_idx;
    // ammount of elements
    uint32_t count;
    // ammount of allocated space
    uint32_t capacity;
} linearmap_header;

/////////////////////////////////////////////////////////////////////////////
/// cast map_ref to header*
/////////////////////////////////////////////////////////////////////////////
#define _to_Lmap_header(map_ref) ((linearmap_header*)(map_ref) - 1)

/////////////////////////////////////////////////////////////////////////////
/// number of elements
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_len(map_ref) (_to_Lmap_header((map_ref))->last_elem_idx)

/////////////////////////////////////////////////////////////////////////////
/// compare 2 data depending on each type perform special case, return 1 if same, else 0
/////////////////////////////////////////////////////////////////////////////
#define _LINEARMAP_special_compare(map_ref, idx, KEY2) ({ \
    /* first check if theres a user defined function */   \
    typeof(KEY2) KEY1 = (map_ref)[idx].key;               \
    int res = 0;                                          \
    if (_to_Lmap_header(map_ref)->compare_func != NULL)   \
        res = _to_Lmap_header(map_ref)->compare_func(     \
                (void*)&KEY1,                             \
                (void*)&(KEY2)                            \
        );                                                \
    else {                                                \
        res = _Generic(1? (KEY1) : (KEY2),                \
            char*      : (strcmp(KEY1, (KEY2)) == 0),     \
            const char*: (strcmp(KEY1, (KEY2)) == 0),     \
            default    : (KEY1 == (KEY2))                 \
        );                                                \
    }                                                     \
    res;                                                  \
})

#define LINEAR_MAP_ELEM(Key_T, Value_T) { \
    Key_T key;                            \
    Value_T value;                        \
}*

/////////////////////////////////////////////////////////////////////////////
/// create a struct that contails the information about the map
///
/////////////////////////////////////////////////////////////////////////////
#define LINEARMAP_TYPE(Key_T, Value_T) struct LINEAR_MAP_ELEM(Key_T, Value_T)

/////////////////////////////////////////////////////////////// //////////////
/// create a struct that contails the information about the map, but give it struct name
///
/////////////////////////////////////////////////////////////////////////////
#define LINEARMAP_NAME_TYPE(Key_T, Value_T, name) struct name LINEAR_MAP_ELEM(Key_T, Value_T)

/////////////////////////////////////////////////////////////////////////////
/// initialize a LinearMap
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_init(map_ref, size) do {                                      \
    if ((map_ref) != NULL) break;                                               \
    /* cast the map_ref to header type and put necesary header data*/           \
    linearmap_header* header ;                                                  \
    /*        cast to header         sizeof type mul size plus size of header */\
    header = (typeof(header))malloc(sizeof(*map_ref) * size + sizeof(*header)); \
    header->count = 0;                                                          \
    header->capacity = size;                                                    \
    header->last_elem_idx = 0;                                                  \
    header->compare_func = NULL;             /* set conpare function to null */ \
    (map_ref) = (typeof(map_ref))(header + 1); /* [header][map_ref] */          \
} while(0)


/////////////////////////////////////////////////////////////////////////////
/// DO NOT USE push a new key,elemement pair to the array at its end aka array[.last_elem_idx]
/////////////////////////////////////////////////////////////////////////////
#define _LinearMap_appendElem(map_ref, _key, _elem) do {                                                      \
    if ((map_ref) == NULL) {printf("_LinearMap_appendElem: map_ref is not initiated");exit(-1);}              \
    linearmap_header* header = _to_Lmap_header(map_ref);                                                      \
    /* malke sure theres enouch space */                                                                      \
    if (header->last_elem_idx >= header->capacity) {                                                          \
        int size = header->capacity *= 1.5;                                                                   \
        linearmap_header* temp = (typeof(temp))realloc((map_ref), sizeof(*(map_ref)) * size + sizeof(*temp)); \
        if (!temp) {                                                                                          \
            fprintf(stderr, "unable to reallocate memory for LinearMap_appendElem\n");exit(-1);               \
        }                                                                                                     \
        /* update the header and map_ref incase it reallocates */                                             \
        header = temp;                                                                                        \
        (map_ref) = (typeof(map_ref))(header + 1);                                                            \
    }                                                                                                         \
    /* append the new ellement */                                                                             \
    (map_ref)[header->last_elem_idx] = \
    (typeof(*(map_ref))){(_key),(_elem)}; header->count++; header->last_elem_idx++;  \
} while(0)


#define LinearMap_setCmpFunc(map_ref, FUNC) (_to_Lmap_header((map_ref))->compare_func = (FUNC))

/////////////////////////////////////////////////////////////////////////////
/// check if given key is available and return its index, else return -1
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_find(map_ref, KEY) ({                            \
    assert(map_ref != NULL && "used _LinearMap_find with NULL\n"); \
    size_t idx = 0; /* return value */                             \
    /* search for `map_ref`*/                                      \
    size_t len = LinearMap_len((map_ref));                         \
    for (;                                                         \
        idx < len AND NOT _LINEARMAP_special_compare(              \
           (map_ref),                                              \
           idx,                                                    \
           (KEY)                                                   \
        );                                                         \
        idx++                                                      \
    )                        ;                                     \
    idx == (len - 1) ? -1 : idx;                                   \
})

/////////////////////////////////////////////////////////////////////////////
/// search for free slot and return the first usable slot, if non found reurn -1
/////////////////////////////////////////////////////////////////////////////
#define _LinearMap_findFree(map_ref) ({                                \
    assert(map_ref != NULL && "used _LinearMap_findFree with NULL\n"); \
    size_t idx = 0;                                                    \
    typeof((map_ref)->key) blank;                                      \
    size_t len = LinearMap_len((map_ref));                             \
    for (;                                                             \
        idx < len AND NOT _LINEARMAP_special_compare(                  \
            (map_ref),                                                 \
            idx,                                                       \
            blank /* empty contructor*/                                \
        );                                                             \
        idx++                                                          \
    );                                                                 \
    /* if the key on idx is empty return idx else -1 */                \
    _LINEARMAP_special_compare((map_ref), idx, blank) ? idx : -1;      \
})

/////////////////////////////////////////////////////////////////////////////
/// set a value
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_set(map_ref, _key, _elem) do {       \
    assert(map_ref != NULL && "used LinearMap_set with NULL\n");\
    /* first check if the key is alrealy set*/         \
    size_t found = LinearMap_find((map_ref), (_key));  \
    /* if found set _elem to that index*/              \
    if (found != -1) {                                 \
        (map_ref)[found].value = (_elem); break;       \
    }                                                  \
    /* else check for empty slot and put there*/       \
    found = _LinearMap_findFree((map_ref));            \
    if (found != -1) {                                 \
        (map_ref)[found].key = (_key);                 \
        (map_ref)[found].value = (_elem);              \
        break;                                         \
    }                                                  \
    /* if no free space make one*/                     \
    _LinearMap_appendElem((map_ref),(_key),(_elem));   \
} while(0)


#define LinearMap_get(map_ref, _key, _elem) do {                   \
    assert((map_ref) != NULL && "used LinearMap_get with NULL\n"); \
    /* first check if the key is alrealy set*/                     \
    size_t found = LinearMap_find((map_ref), (_key));              \
    /* if found set _elem to that index*/                          \
    if (found != -1) {                                             \
        (_elem) = (map_ref)[found].value; break;                   \
    }                                                              \
    /* else check for empty slot and put there*/                   \
    found = _LinearMap_findFree((map_ref));                        \
    if (found != -1) {                                             \
        (map_ref)[found].value = (_elem); break;                   \
    }                                                              \
    /* if no free space make one*/                                 \
    _LinearMap_appendElem((map_ref), (_key), (_elem));             \
} while(0) 

#define LinearMap_free(map_ref) do {\
    free(_to_Lmap_header((map_ref)));\
    (map_ref) = NULL;\
} while(0)

static void _internal_testing_LinearMap_kyjh4imfqvni2hc40ftgctvqbgdmop7ghk5whux1rvoddy313u() {
    printf("============== STARTED TESTING LinearMap =================\n");
    LINEARMAP_NAME_TYPE(const char*, int, char_int_map) map = NULL;
    LinearMap_init(map, 1);
    {
        const char* d = "32", *s="meh";
        LinearMap_set(map, d, 34);
        LinearMap_set(map, s, 30);
        printf("done setting...\n");

        int data = 0;
        const char* key = "meh"; // this dont exist why it giving 32
        // LinearMap_get(map, key, data);
        data  = LinearMap_find(map, key);

        data = _to_Lmap_header((map))->count;
        printf(">>> testing insert and get\n success=%d\n",data);
    }
    printf("============== DONE TESTING LinearMap =================\n");
    LinearMap_free(map);
}

#endif  // INCLUDE
