// vim: ft=c
/*
 * slot map, a map where we map indx to a item but the index is stable and unchanging
 *
 * this map works by having 3 container type, ID, Index, Data
 * where:
 *     Index - or data index stores the element index to be used
 *     ID    - stores the data index of the element
 *     Data  - stores the actual data
 * 
 *
 * here's a diagram describing the relation of Index and Data
 * the Index is maped with a Data where 0th element is maped
 * on 3 and "dasfd" corespondingly on Index and Data
 * structure
 *    IDX ID* Data*
 *    0    3  "dasfd"
 *    1    0  "lodass"
 *    2    1  "omnhy"
 *    3    2  STALE
 *            ^ this means its allocated but not occupied
 * 
 *    IDX Data Index*(interface to fetch Data)
 *    0       1
 *    1       2
 *    2       3
 *    3       0
 *
 *  on get we do this
 *  raw index -> Index[i] -> Data[Index[i]] -> output
 *  
 *  on pushing data
 *  Data[length(Data)-1] = new data
 *  ID[length(Data)-1] -> output
 *
 *  on removing data
 *  raw index -> mark removed Data[i] # or could be just to decrement the Data.length
 *  raw index -> swap (Data[i], Data[length(Data)-1])
 *  raw index -> swap (ID[i], ID[length(Data)-1])
 *
 *
 * source "The magic container" by Pezzza's Work (YT)
 *
 * */

#ifndef INCLUDE_MAP_SLOTMAP_H_
#define INCLUDE_MAP_SLOTMAP_H_

#include <cstdlib>
#include <stddef.h>
#include <stdint.h>

typedef struct SlotMap {
    uint32_t* Index;
    uint32_t* ID;
    // be carefull on type safety
    void* Data;
    // count of Indexes, used aslo as capacity
    uint32_t Index_len;
    // count of all elements
    uint32_t Data_len;
    // capacity
    uint32_t Data_cap;
} SlotMap;


#if defined(_LIB_TEST) || defined(SLOTMAP_IMPL)

/////////////////////////////////////////////////////////////////////////////
/// initialize a slot map, PLEASE USE SlotMap_free TO FREE AND NOT LEAK STUFF
///
/// param:
/// slot_ref  - reference to the map object
/// count     - count of oberall data slot to be allocated
/// size      - ammount of bytes of each object (be carefull on type safety)
///
/// return: void
/////////////////////////////////////////////////////////////////////////////
void SlotMap_init(SlotMap* slot_ref, size_t count, size_t size);

#endif  // defined(LIB_TEST) || defined(SLOTMAP_IMPL)

#endif  // INCLUDE_MAP_SLOTMAP_H_
