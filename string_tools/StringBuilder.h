// vim: ft=c
// if INCLUDE_STRING_BUILDER_STRINGBUILDER_H_ is not defined
#ifndef INCLUDE_STRING_BUILDER_STRINGBUILDER_H_
// define INCLUDE_STRING_BUILDER_STRINGBUILDER_H_ and set nothing to it
#define INCLUDE_STRING_BUILDER_STRINGBUILDER_H_

#include <stdint.h>
#include <stddef.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../UTILS/UTILS.h"

typedef struct StringBuilder {
    char* buffer;
    uint32_t length;
    uint32_t capacity;
} StringBuilder;


#if defined(STRINGBUILDER_IMPL)
//////////////////////////////////////////////////////////////////////////
/// \brief A StringBuilder is a mutable string that can be built incrementally.
///
/// PLS FREE using StringBuilderFree
//////////////////////////////////////////////////////////////////////////
[[nodiscard]] StringBuilder StringBuilderInit() {
    StringBuilder sb = {0};
    sb.capacity = 64; // initial capacity
    sb.length = 0;
    sb.buffer = (char*)malloc(sb.capacity);
    assert(sb.buffer != NULL && "unable to allocate string builder");
    sb.buffer[0] = '\0'; // null terminate the string

    return sb;
}


//////////////////////////////////////////////////////////////////////////
/// \brief Append a string to the string builder.
///
/// \param sb The string builder to append to.
/// \param str The string to append.
//////////////////////////////////////////////////////////////////////////
void StringBuilderAppendStr(StringBuilder* const sb, const char* const str, size_t str_len) {
    if (str_len == 0) return;
    assert(str != NULL);
    if (sb->length + str_len + 1 > sb->capacity) {
        // allocate 1.5 times of the current size + the new string length
        sb->capacity = (size_t)(sb->capacity * 1.5) + str_len + 1;
        sb->buffer = (char*)realloc(sb->buffer, sb->capacity);
        assert(sb->buffer != NULL && "unable to reallocate string builder");
    }
    // append the new string to the buffer
    memcpy(sb->buffer + sb->length, str, str_len); // asan is complaining about this line
    sb->length += str_len;
} 

//////////////////////////////////////////////////////////////////////////
/// \brief Append another string builder to this string builder.
/// 
/// \param sb The string builder to append to.
/// \param other The string builder to append.
//////////////////////////////////////////////////////////////////////////
void StringBuilderAppend(StringBuilder* sb, const StringBuilder* other) {
    // append another StringBuilder to this StringBuilder
    StringBuilderAppendStr(sb, other->buffer, other->length);
}


//////////////////////////////////////////////////////////////////////////
/// \brief Create a string builder from a null terminated string.
/// PLS FREE using StringBuilderFree
///
/// \param str The string to create the string builder from.
///
/// \return A new string builder containing the string.
//////////////////////////////////////////////////////////////////////////
[[nodiscard]] StringBuilder StringBuilderFromString(const char* str, size_t str_len) {
    // create a new StringBuilder from a non null terminated string
    StringBuilder sb = StringBuilderInit();
    StringBuilderAppendStr(&sb, str, str_len);
    return sb;
} 

//////////////////////////////////////////////////////////////////////////
/// \brief Free the memory used by the string builder.
///
/// \param sb The string builder to free.
//////////////////////////////////////////////////////////////////////////
void StringBuilderFree(StringBuilder* sb) {
    free(sb->buffer);
    sb->buffer = NULL; // set the pointer to NULL after freeing
    sb->length = 0;
    sb->capacity = 0;
}


//////////////////////////////////////////////////////////////////////////
/// \brief Convert the string builder to a null terminated string.
/// PLS FREE using free
///
/// \param sb The string builder to convert.
/// \param output A pointer to a char* that will be set to the null terminated string. The caller is responsible for freeing the memory used by the output string.
///
/// \return A null terminated string containing the contents of the string builder. The caller is responsible for freeing the memory used by the output string.
//////////////////////////////////////////////////////////////////////////
[[nodiscard]] char* StringBuilderToString(const StringBuilder* sb) {
    // return a copy of the buffer to the caller, who is responsible for freeing it
    char* result = (char*)malloc(sb->length + 1);
    assert(result != NULL AND "unable to allocate string for StringBuilderToString");

    memcpy(result, sb->buffer, sb->length);
    result[sb->length] = '\0'; // null terminate the string
    return result;
}
#endif // STRINGBUILDER_IMPL

#endif  // INCLUDE_STRING_BUILDER_STRINGBUILDER_H_
