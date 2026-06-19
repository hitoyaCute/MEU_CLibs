// vim: ft=c
#ifndef INCLUDE_STRINGVIEW_STRINGVIEW_H_
#define INCLUDE_STRINGVIEW_STRINGVIEW_H_

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

typedef struct string_view_impl {
    const char* data;
    size_t      len;
} StringView;

#if defined(STRINGVIEW_IMPL) || defined (_LIB_TEST)

#define StringViewPRINT(strview) (StringView)(strview).len, (StringView)(strview).data

StringView StringView_fromString(const char* str) {
    return (StringView) {
        .data = str,
        .len  = strlen(str)
    };
}

StringView StringView_popFront(StringView* strview, const size_t len = 1) {
    assert(len > 1);
    StringView out{*strview};
    if (len >= strview->len) {
        strview->data += strview->len;
        strview->len = 0;
    } else {
        strview->data += len;
        strview->len  -= len;

        out.len = len;
    }
    return out;
}

StringView StringView_popEnd(StringView* strview, const size_t len = 1) {
    assert(len > 1);
    StringView out{*strview};
    if (len >= strview->len)
        strview->len = 0;
    else {
        strview->len  -= len;

        out.data += len;
    }
    return out;
}

// split a string on the first found delim
// if no delim, return nothing
// else remove from beginning till next the delim and return the part from front till the delim
StringView StringView_popOnDelim(StringView* strview, const char delim) {
    // first search for the delim
    size_t i = 0;
    while (i < strview->len) {
        // if delim found, 
        if (strview->data[i] == delim) {
            StringView out {
                .data = strview->data,
                .len  = i
            };
            strview->data += i + 1;
            strview->len  -= (i + 1);

            return out;
        }
        i++;
    }
    // exit on while-loop means out of bound, return nothing
    return (StringView) {
        .data = strview->data,
        .len  = 0
    };
}

#endif // STRINGVIEW_IMPL

#endif  // INCLUDE_STRINGVIEW_STRINGVIEW_H_
