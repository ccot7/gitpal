#ifndef LOOKUP_H
#define LOOKUP_H

#include "gitref.h"

typedef struct {
    const GitEntry *entry;
    int score;
} LookupResult;

/* Search the DB.  Returns number of results written (0..max_results).
 * Caller provides results array of at least max_results elements. */
int lookup_query(const char *query, LookupResult *results, int max_results);

#endif /* LOOKUP_H */
