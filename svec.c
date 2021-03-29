/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

// Makes an svec with default values.
svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(2 * sizeof(char*));
    sv->size = 0;
    sv->capacity = 2;
    return sv;
}

// Frees allocated memory for the svec.
void
free_svec(svec* sv)
{
    assert(sv != NULL);

    for (int i = 0; i < sv->size; i++) {
        if (sv->data[i] != 0)
            free(sv->data[i]);
    }

    free(sv->data);
    free(sv);
}

// Gets the iith element in the svec.
char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

// Puts the given item at the iith element in the svec.
void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = strdup(item);
}

// Puts the given item at the back of the svec, and increases capacity once half filled.
void
svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    if (ii >= sv->capacity) {
        sv->capacity *= 2;
        sv->data = realloc(sv->data, sv->capacity * sizeof(char*));
    }

    sv->size = ii + 1;
    svec_put(sv, ii, item);
}

// Swaps the first item with the second item.
void
svec_swap(svec* sv, int ii, int jj)
{
    char* temp = svec_get(sv, ii);
    sv->data[ii] = sv->data[jj];
    sv->data[jj] = temp;

}
