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
dynarr_pop(int_array); // removes the last element

int int_count = dyarr_len(int_array); // get the lenght of the array

dynarr_reserve(int_array, 10); // reserve 10

dynarr_extend(s,s); // [s][s]

int* int_elem_end = dynarr_end(int_array);

int* last_int_elem = dynarr_back(int_arr);

int* first_int_elem = dynarr_first(int_arr);

int raw_int_arr[] = {0,1,2,3,4,4,5,6,7,7};
// 0 = different, 1 = same
int result = dynarr_ncmp(int_arr, raw_int_arr, sizeof(raw_int_arr)/sizeof(*raw_int_arr));

dynarr_free(int_arr); // free the array

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
#define dynarr_extend(arr_ref, other) do {                \
    uint other_count = _to_dynarr_header((other))->count; \
    /* copy yhe elements from the other to the arr_ef*/   \
    for (uint32_t i = 0; i < other_count; i++) {          \
        dynarr_append((arr_ref), (other)[i]);             \
    }                                                     \
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
/// clear the array but not deallocate
/////////////////////////////////////////////////////////////////////////////
#define dynarr_clear(arr_ref) do{_to_dynarr_header((arr_ref))->count = 0;} while(0)

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
///          (if n is larger than any of the arr return false always)
///
/// \return 1 if the 2 array upto n'th element is same
/////////////////////////////////////////////////////////////////////////////
#define dynarr_ncmp(arr_ref1, arr_ref2, n) ({            \
    int arr1_count = _to_dynarr_header(arr_ref1)->count; \
    /* return 0 if n is larger than array*/              \
    (n > arr1_count) ? 0 : ({                            \
        /* else check if theres no difference */         \
        int result = 1;                                  \
        /* if found difference set false and break */    \
        for (uint32_t i = 0; i < n; i++) {               \
            if (arr_ref1[i] != arr_ref2[i]) {            \
                result = 0;                              \
                break;                                   \
            }                                            \
        };                                               \
        result;                                          \
    });                                                  \
})

// just prints the data and header
#define dynarr_debug(arr_ref, type_format_str) do {                                                      \
    const dynarr_header* header = _to_dynarr_header((arr_ref));                                          \
    printf("header [%d(count),%d(capacity)][",(int)header->count,(int)header->capacity); fflush(stdout); \
    for (uint32_t i = 0; i < header->count; i++) {                                                       \
        printf("%" type_format_str",", arr_ref[i]);                                                      \
    }                                                                                                    \
    printf("]");                                                                                         \
} while (0)

static void _internal_testing_dynarr_kkzq4x33mqvn3ubpcfm1qhcipys0k5t9szsm9uicr5zdwqtzqd() {
    printf("============== STARTING TO TEST dynarr =================\n");
    int* s = NULL;
    dynarr_init(s, 3);
    dynarr_debug(s,"d"); printf("\n");
    dynarr_reserve(s, 20);
    dynarr_debug(s,"d"); printf("\n");

    {
        printf(">>> appending 10\n"); fflush(stdout);
        dynarr_append(s, 10); // [10,]
        int test_data[] = {10,};
        printf(" success=%d\n",(int)dynarr_ncmp(s, test_data, sizeof(test_data)/sizeof(*test_data)));
    }

    {
        printf(">>> extending to self\n"); fflush(stdout);
        dynarr_extend(s, s);  // [10,10,]
        int test_data[] = {10,10,};
        printf(" success=%d\n",(int)dynarr_ncmp(s, test_data, sizeof(test_data)/sizeof(*test_data)));
    }

    {
        printf(">>> appending data 0..10\n"); fflush(stdout);
        for (int i = 0; i < 10; i++)
            dynarr_append(s, i);
        int test_data[] = {10,10,0,1,2,3,4,5,6,7,8,9};
        printf(" success=%d\n",(int)dynarr_ncmp(s, test_data, sizeof(test_data)/sizeof(*test_data)));
    }

    {
        printf(">>> extending ones again\n"); fflush(stdout);
        dynarr_extend(s, s);
        int test_data[] = {10,10,0,1,2,3,4,5,6,7,8,9,10,10,0,1,2,3,4,5,6,7,8,9,};
        printf(" success=%d\n",(int)dynarr_ncmp(s, test_data, sizeof(test_data)/sizeof(*test_data)));

    }

    {
        printf(">>> popping\n"); fflush(stdout);
        dynarr_pop(s);
        int test_data[] = {10,10,0,1,2,3,4,5,6,7,8,9,10,10,0,1,2,3,4,5,6,7,8,};
        printf(" success=%d\n",(int)dynarr_ncmp(s, test_data, sizeof(test_data)/sizeof(*test_data)));

    }
    dynarr_clear(s);
    dynarr_free(s);

    printf("============== DONE TESTING dynarr =================\n");
}

#endif  // INCLUDE_DYARR_DYNARR_H_
