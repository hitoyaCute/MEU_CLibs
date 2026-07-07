// vim: ft=c
/********************************************************************
dynamic array for any type

how to use

// define the dype
int* int_array = NULL; // create a array type int, NULL is optional but good practice

// initialization
dynarr_init(int_array, 10); // init int_array and prealocate it with 10 elements

// usage
dynarr_append(int_array, 0); // push 0 after the last element on the int_array
                             // int_array -> [0]


  ********************************************************************/
#ifndef INCLUDE_DYARR_DYNARR_H_
#define INCLUDE_DYARR_DYNARR_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef EXPAND_RATE
#define EXPAND_RATE 1.5
#endif // !EXPAND_RATE

#define OR ||


typedef struct dynarr_header_impl{
    uint32_t count;
    uint32_t capacity;
} dynarr_header;

// cast array reference to header type
#define _to_dynarr_header(arr_ref) ((dynarr_header*)(arr_ref) - 1)

/////////////////////////////////////////////////////////////////////////////
/// \brief creates a new dynarr object
/// 
/// \param arr_ref reference to the array type
/// \param size count of elements to pree allocate
///
/// \return dynarr
/////////////////////////////////////////////////////////////////////////////
#define dynarr_init(arr_ref, size) do {                                                          \
    if ((arr_ref) != NULL) break;                                                                \
    dynarr_header* header = (dynarr_header*)malloc(sizeof(*(arr_ref))* (size)+ sizeof(*header)); \
    header->count = 0;                                                                           \
    header->capacity = (size);                                                                   \
    (arr_ref) = (typeof(arr_ref))(header + 1);                                                   \
} while(0)

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define dynarr_append(arr_ref, element) do {                                                                          \
    if ((arr_ref) == NULL) dynarr_init((arr_ref),32);                                                                 \
    dynarr_header* header = _to_dynarr_header((arr_ref));                                                             \
    /* make sure theres enough capacity*/                                                                             \
    if (header->count >= header->capacity) {                                                                          \
        header->capacity *= 1.5;                                                                                      \
        dynarr_header* temp = (dynarr_header*)realloc(header, sizeof(*(arr_ref)) * header->capacity + sizeof(*temp)); \
        if (!temp) {                                                                                                  \
            fprintf(stderr, "unable to reallocate memory for dynarr_append\n");exit(-1);                              \
        }                                                                                                             \
        /* refresh the arr_ref to handle the case on which the realloc reallocated the array on different address*/   \
        header = temp;                                                                                                \
        (arr_ref) = (typeof(arr_ref))(header + 1);                                                                    \
    }                                                                                                                 \
    /* append and increasing the count*/                                                                              \
    (arr_ref)[header->count] = (element); header->count++;                                                            \
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
///
/// \param arr_ref reference to the array
/// \count amount of element to reserve
/////////////////////////////////////////////////////////////////////////////
#define dynarr_reserve(arr_ref, count) do {                                                                     \
    dynarr_header* header = _to_dynarr_header((arr_ref));                                                       \
    if ((count) <= header->capacity OR (arr_ref) == NULL) break;                                                \
    header->capacity = (count);                                                                                 \
    dynarr_header* temp = (dynarr_header*)realloc(header, sizeof(*(arr_ref))*header->capacity + sizeof(*temp)); \
    if (!temp) {                                                                                                \
        fprintf(stderr, "unable to reallocate memory for dynarr_append\n");exit(-1);                            \
    }                                                                                                           \
    /* refresh the arr_ref to handle the case on which the realloc reallocated the array on different address*/ \
    header = temp;                                                                                              \
    (arr_ref) = (typeof(arr_ref))(header + 1);                                                                  \
} while(0)

/////////////////////////////////////////////////////////////////////////////
/// extend - put the other aray to the end of this array
/// 
/// \param arr_ref the reference to the first array
/// \param other_arr the reference to the other array
/////////////////////////////////////////////////////////////////////////////
#define dynarr_extend(arr_ref, other) do {               \
    int other_count = _to_dynarr_header((other))->count; \
    /* copy yhe elements from the other to the arr_ef*/  \
    for (int i = 0; i < other_count; i++) {              \
        dynarr_append((arr_ref), (other)[i]);            \
    }                                                    \
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

/////////////////////////////////////////////////////////////////////////////
/// free the array and reset the reference to it
/////////////////////////////////////////////////////////////////////////////
#define dynarr_free(arr_ref) do {       \
    free(_to_dynarr_header((arr_ref))); \
    (arr_ref) = NULL;                   \
} while (0)


/////////////////////////////////////////////////////////////////////////////
/// compare 2 dynarray
///
/// \param arr_ref1 first array
/// \param arr_ref2 second array
/// \param n number elements to compare
///          (fallback to min(arr_ref1.count, arr_ref2.count) if n is larger than count of any array)
///
/// \return 1 if the 2 array upto n'th element is same
/////////////////////////////////////////////////////////////////////////////
#define dynarr_ncmp(arr_ref1, arr_ref2, n) ({})

// just prints the data and header
#define dynarr_debug(arr_ref, type_format_str) do {                                                      \
    const dynarr_header* header = _to_dynarr_header((arr_ref));                                          \
    printf("header [%d(count),%d(capacity)][",(int)header->count,(int)header->capacity); fflush(stdout); \
    for (int i = 0; i < header->count; i++) {                                                            \
        printf("%" type_format_str",", arr_ref[i]);                                                      \
    }                                                                                                    \
    printf("]");                                                                                         \
} while (0)

static void _internal_testing_dynarr_kkzq4x33mqvn3ubpcfm1qhcipys0k5t9szsm9uicr5zdwqtzqd() {
    int* s = NULL;
    dynarr_init(s, 3);
    dynarr_reserve(s, 20);

    printf("probing first append should be [...][10,]\n>>> "); fflush(stdout);
    dynarr_append(s, 10); // [10,]
    dynarr_debug(s, "d"); printf("\n");

    printf("probing first extend should be [...][10,10,]\n>>> "); fflush(stdout);
    dynarr_extend(s, s);  // [10,10,]
    dynarr_debug(s, "d"); printf("\n");

    printf("appending data 0..10\n>>> "); fflush(stdout);
    for (int i = 0; i < 10; i++)
        dynarr_append(s, i);
    dynarr_debug(s, "d"); printf("\n");

    printf("probing second extend\n>>> "); fflush(stdout);
    dynarr_extend(s, s);  // [10,10,]
    dynarr_debug(s, "d"); printf("\n");

    printf("probing first pop\n>>> "); fflush(stdout);
    dynarr_pop(s);
    dynarr_debug(s, "d"); printf("\n");

    dynarr_free(s);
}

#endif  // INCLUDE_DYARR_DYNARR_H_
