// vim: ft=c
#ifndef INCLUDE_DYARR_DYNARR_H_
#define INCLUDE_DYARR_DYNARR_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef EXPAND_RATE
#define EXPAND_RATE 1.5
#endif // !EXPAND_RATE

typedef struct dynarr_header_impl{
    uint32_t count;
    uint32_t capacity;
} dynarr_header;

#define _to_dynarr_header(arr_ref) (((dynarr_header*)(arr_ref)) - 1)

/////////////////////////////////////////////////////////////////////////////
/// \brief creates a new dynarr object
/// 
/// \param size ammount of bytes to pree allocate
///
/// \return dynarr
/////////////////////////////////////////////////////////////////////////////
#define dynarr_init(arr_ref, size) do {                                                              \
    if ((arr_ref) != NULL) break;                                                                    \
    dynarr_header* header = (dynarr_header*)malloc(sizeof(*arr_ref) * size + sizeof(dynarr_header)); \
    header->count = 0;                                                                               \
    header->capacity = size;                                                                         \
    (arr_ref) = (typeof(arr_ref))header + 1;                                                        \
} while(0)

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define dynarr_append(arr_ref, element) do {                                                                         \
    if ((arr_ref) == NULL) dynarr_init((arr_ref),32);                                                                \
    dynarr_header* header = _to_dynarr_header(arr_ref);                                                           \
    /* make sure theres enough capacity*/                                                                            \
    if (header->count >= header->capacity) {                                                                         \
        header->capacity *= 1.5;                                                                                     \
        dynarr_header* temp = (dynarr_header*)realloc(header, sizeof(*arr_ref) * header->capacity + sizeof(header)); \
        if (!temp) {                                                                                                 \
            fprintf(stderr, "unable to reallocate memory for dynarr_append\n");exit(-1);                             \
        }                                                                                                            \
        /* refresh the arr_ref to handle the case on which the realloc reallocated the array on different address*/  \
        (arr_ref) = (typeof(arr_ref))(header+1);                                                                                \
    }                                                                                                                \
    /*append*/                                                                                                       \
    (arr_ref)[header->count] = (element); header->count++;                                                           \
} while(0)

/////////////////////////////////////////////////////////////////////////////
/// remove the last element
/////////////////////////////////////////////////////////////////////////////
#define dynarr_pop(arr_ref) do {_to_dynarr_header((arr_ref))->count--;} while(0)

/////////////////////////////////////////////////////////////////////////////
/// get the ammount of element
/////////////////////////////////////////////////////////////////////////////
#define dynarr_len(arr_ref) (_to_dynarr_header((arr_ref))->count

/////////////////////////////////////////////////////////////////////////////
/// reserve
/// if count is larger than the current capasity
/// reallocate
/// else do nothing
/////////////////////////////////////////////////////////////////////////////
#define dynarr_reserve(arr_ref, count) do {                                                                      \
    dynarr_header* header = _to_dynarr_header((arr_ref));                                                       \
    if ((count) <= header->capasity OR (arr_ref) == NULL) break;                                                 \
    header->capacity = (count)                                                                                   \
    dynarr_header* temp = (dynarr_header*)realloc(header, sizeof(*arr_ref) * header->capacity + sizeof(header)); \
    if (!temp) {                                                                                                 \
        fprintf(stderr, "unable to reallocate memory for dynarr_append\n");exit(-1);                             \
    }                                                                                                            \
    /* refresh the arr_ref to handle the case on which the realloc reallocated the array on different address*/  \
    (arr_ref) = (typeof(arr_ref))(header+1);                                                                                \
} while(0)

/////////////////////////////////////////////////////////////////////////////
/// get the 1 pass last element
/////////////////////////////////////////////////////////////////////////////
#define dynarr_end(arr_ref) ((arr_ref) + _to_dynarr_header((arr_ref))->count)
/////////////////////////////////////////////////////////////////////////////
/// get the last element
/////////////////////////////////////////////////////////////////////////////
#define dynarr_back(arr_ref) (dynarr_end(arr_ref) - 1)
/////////////////////////////////////////////////////////////////////////////
/// get the first element
/////////////////////////////////////////////////////////////////////////////
#define dynarr_first(arr_ref) (arr_ref)

#define dynarr_free(arr_ref) free(_to_dynarr_header((arr_ref)))

#endif  // INCLUDE_DYARR_DYNARR_H_
