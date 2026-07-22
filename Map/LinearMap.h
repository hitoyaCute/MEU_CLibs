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
    // count of total alive items
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
#define LinearMap_len(map_ref) (_to_Lmap_header((map_ref))->count)

/////////////////////////////////////////////////////////////////////////////
/// compare 2 data depending on each type perform special case, return 1 if same, else 0
/////////////////////////////////////////////////////////////////////////////
#define _LINEARMAP_special_compare(compare_func, KEY1, KEY2) ({       \
    int res = 0;                                                      \
    if (compare_func == NULL)                                         \
        res = _Generic(1? (KEY1) : (KEY2),                            \
            char*      : (strcmp((char*)(KEY1), (char*)(KEY2)) == 0), \
            const char*: (strcmp((char*)(KEY1), (char*)(KEY2)) == 0), \
            default    : ((KEY1) == (KEY2))                           \
        );                                                            \
    else {                                                            \
        res = compare_func(                                           \
                (void*)&(KEY1),                                       \
                (void*)&(KEY2)                                        \
        );                                                            \
    }                                                                 \
    res;                                                              \
})

#define LINEAR_MAP_ELEM(Key_T, Value_T) { \
    Key_T key;                            \
    Value_T value;                        \
}*

/////////////////////////////////////////////////////////////////////////////
/// create a struct that contails the information about the map
/////////////////////////////////////////////////////////////////////////////
#define LINEARMAP_TYPE(Key_T, Value_T) struct LINEAR_MAP_ELEM(Key_T, Value_T)

/////////////////////////////////////////////////////////////// //////////////
/// create a struct that contails the information about the map, but give it struct name
/////////////////////////////////////////////////////////////////////////////
#define LINEARMAP_NAME_TYPE(Key_T, Value_T, name) struct name LINEAR_MAP_ELEM(Key_T, Value_T)

/////////////////////////////////////////////////////////////////////////////
/// initialize a LinearMap
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_init(map_ref, size) do {                                      \
    if ((map_ref) != NULL) break;                                               \
    /* cast the map_ref to header type and put necesary header data*/           \
    linearmap_header* header;                                                   \
    /*        cast to header         sizeof type mul size plus size of header*/ \
    header = malloc(sizeof(*map_ref) * size + sizeof(*header));                 \
    header->count = 0;                                                          \
    header->capacity = size;                                                    \
    header->compare_func = NULL;             /* set conpare function to null */ \
    (map_ref) = (void*)(header + 1); /* [header][map_ref] */                    \
} while(0) 


/////////////////////////////////////////////////////////////////////////////
/// DO NOT USE push a new key,elemement pair to the array at its end aka array[.count]
/////////////////////////////////////////////////////////////////////////////
#define _LinearMap_appendElem(map_ref, _key, _elem) do {                                                         \
    if ((map_ref) == NULL) {printf("_LinearMap_appendElem: map_ref is not initiated");exit(-1);}                 \
    linearmap_header* header = _to_Lmap_header(map_ref);                                                         \
    /* malke sure theres enouch space */                                                                         \
    if (header->count >= header->capacity) {                                                                     \
        int size = header->capacity *= 2;                                                                        \
        linearmap_header* temp = realloc(_to_Lmap_header((map_ref)), sizeof(*(map_ref)) * size + sizeof(*temp)); \
        if (!temp) {                                                                                             \
            fprintf(stderr, "unable to reallocate memory for LinearMap_appendElem\n");exit(-1);                  \
        }                                                                                                        \
        /* update the header and map_ref incase it reallocates */                                                \
        header = temp;                                                                                           \
        (map_ref) = (void*)(header + 1);                                                                         \
    }                                                                                                            \
    /* append the new ellement */                                                                                \
    (map_ref)[header->count].key = (_key); (map_ref)[header->count].value = (_elem);                             \
    header->count++;                                                                                             \
} while(0)

// set a compare function
#define LinearMap_setCmpFunc(map_ref, FUNC) (_to_Lmap_header((map_ref))->compare_func = (FUNC))

/////////////////////////////////////////////////////////////////////////////
/// check if given key is available and return its index, else return -1
///
/// param:
///
/// map_ref - a pointer to the map arrays
/// KEY     - would be the key to find
///
/// return found - retuns the index of the entry with KEY, else -1 if it doesnt exist
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_find(map_ref, KEY) ({                            \
    assert(map_ref != NULL && "used _LinearMap_find with NULL\n"); \
    size_t found = -1; /* return value */                          \
    /* search for `map_ref`*/                                      \
    size_t len = LinearMap_len((map_ref));                         \
    for (size_t idx = 0; idx < len; idx++ ) {                      \
         if (_LINEARMAP_special_compare(                           \
           _to_Lmap_header((map_ref))->compare_func,               \
           (map_ref)[idx].key,                                     \
           (KEY))                                                  \
         ) {                                                       \
            found = idx;                                           \
        }                                                          \
    }                                                              \
    found;                                                         \
})

/////////////////////////////////////////////////////////////////////////////
/// set a value, this creates new entry if no _key is found, otherwise set a value to that entry
///
/// param:
///
/// map_ref - a pointer to the map arrays
/// _key    - would be the key
/// _elem   - would be the element
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_set(map_ref, _key, _elem) do {                   \
    assert((map_ref) != NULL && "used LinearMap_set with NULL\n"); \
    /* first check if the key is alrealy set*/                     \
    size_t found = LinearMap_find((map_ref), (_key));              \
    /* if found set _elem to that index*/                          \
    if (found == (size_t)-1) {                                     \
        /* if no free space make one*/                             \
        _LinearMap_appendElem((map_ref),(_key),(_elem)); }         \
    else                                                           \
        (map_ref)[found].value = (_elem);                          \
} while(0)

/////////////////////////////////////////////////////////////////////////////
/// remove a map entry using _key
///
/// param:
///
/// map_ref - a pointer to the map arrays
/// _key    - would be the key
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_remove(map_ref, _key) do {                              \
    assert((map_ref) != NULL && "used LinearMap_remove with NULL\n");     \
    /* first check if the key is alrealy set*/                            \
    size_t found = LinearMap_find((map_ref), (_key));                     \
    size_t len   = LinearMap_len((map_ref));                              \
    /* if found remove that element */                                    \
    if (found != (size_t)-1) {                                            \
        /* overwrites the entry at found with entry at len-1 */           \
        memcpy((map_ref) + found, (map_ref) + len-1, sizeof(*(map_ref))); \
        _to_Lmap_header((map_ref))->count--;                              \
    }                                                                     \
} while (0)

/////////////////////////////////////////////////////////////////////////////
/// fetch the entry with the _key, make new entry if no _key is found
///
/// param:
///
/// map_ref - a pointer to the map arrays
/// _key    - would be the key
/// _elem   - would be the element
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_get(map_ref, _key, _elem) do {                   \
    assert((map_ref) != NULL && "used LinearMap_get with NULL\n"); \
    /* first check if the key is alrealy set*/                     \
    size_t found = LinearMap_find((map_ref), (_key));              \
    /* if found set _elem to that index*/                          \
    if (found != (size_t)-1)                                       \
        (_elem) = (map_ref)[found].value;                          \
    else                                                           \
        /* if no free space make one*/                             \
        _LinearMap_appendElem((map_ref), (_key), (_elem));         \
} while(0) 


/////////////////////////////////////////////////////////////////////////////
/// free a LinearMap data and NULL it
/////////////////////////////////////////////////////////////////////////////
#define LinearMap_free(map_ref) do {  \
    free(_to_Lmap_header((map_ref))); \
    (map_ref) = NULL;                 \
} while(0)

static void _internal_testing_LinearMap_kyjh4imfqvni2hc40ftgctvqbgdmop7ghk5whux1rvoddy313u() {
    printf("============== STARTED TESTING LinearMap =================\n");
    LINEARMAP_NAME_TYPE(int, float, int_float_map) map = NULL;
    LinearMap_init(map, 1);
                            
    {
        int d = 32;
        int s = 67;
        LinearMap_set(map, d, 34.2);
        LinearMap_set(map, s, 30.3);
        printf(">>> testing set inserting 2 entry\n success=%d\n", LinearMap_len(map) == 2);
    }
    {
        int key = 67;
        float data  = 0;
        LinearMap_get(map, key, data);

        printf(">>> testing get\n success=%d\n",data == 30.3f);
    }
    {
        int key = 32;
        LinearMap_remove(map, key);
        printf(">>> testing deleting map entry\n success=%d\n", LinearMap_find(map, key) == (size_t)-1);
    }
    {
        int key = 67;
        int res = LinearMap_find(map, key);
        printf(">>> testing find\n success=%d\n", res == 0);
    }
    printf("============== DONE TESTING LinearMap =================\n");
    LinearMap_free(map);
}

#endif  // INCLUDE
