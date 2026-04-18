#include "lookup.h"
#include "gitref.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

/* ── helpers ────────────────────────────────────────────────────────────── */

static void str_lower(const char *src, char *dst, size_t n)
{
    size_t i;
    for (i = 0; i < n - 1 && src[i]; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

/* Stop words — too generic to contribute signal */
static int is_stop_word(const char *w)
{
    static const char *stops[] = {
        "the","a","an","to","in","of","for","and","or","my","i",
        "how","do","can","get","set","use","via","with","from",
        "git","this","that","it","is","be","on","at","as","by",NULL
    };
    for (int i = 0; stops[i]; i++)
        if (strcmp(w, stops[i]) == 0) return 1;
    return 0;
}

/*
 * Scoring tiers:
 *
 * Keywords (per slot):
 *   +6  full keyword phrase found in query   ("stash my changes" in query)
 *   +3  full query found inside keyword      ("stash" inside "stash my changes")
 *   +1  per non-stop query word in keyword
 *
 * Explanation full-text (+2 per meaningful query word found):
 *   Explanation is now a first-class search field.  Lets natural-language
 *   queries like "see who wrote a line" hit the git-blame entry even without
 *   an explicit keyword, because the explanation mentions "who last modified".
 *
 * Command full-text (+1 per non-stop query word found in command string):
 *   Lets "restore --staged" or "reset --hard" queries find the right entry.
 */
static int entry_score(const GitEntry *e, const char *query)
{
    char q[512];
    str_lower(query, q, sizeof q);

    int score = 0;

    /* ── keyword matching ────────────────────────────────────────────── */
    for (int i = 0; i < 8 && e->keywords[i]; i++) {
        char kw[256];
        str_lower(e->keywords[i], kw, sizeof kw);

        if (strstr(q, kw)) {
            score += 6;
        } else if (strstr(kw, q)) {
            score += 3;
        } else {
            char qcopy[512];
            snprintf(qcopy, sizeof qcopy, "%s", q);
            char *sv = NULL;
            char *tok = strtok_r(qcopy, " \t", &sv);
            while (tok) {
                if (!is_stop_word(tok) && strlen(tok) > 4 && strstr(kw, tok))
                    score += 1;
                tok = strtok_r(NULL, " \t", &sv);
            }
        }
    }

    /* ── explanation full-text search (+1 per matching word) ────────── */
    /* Only fires if keywords already scored something — prevents explanation
     * noise in unrelated entries from drowning out correct results.       */
    if (score > 0) {
        char expl[4096];
        str_lower(e->explanation, expl, sizeof expl);
        char qcopy[512];
        snprintf(qcopy, sizeof qcopy, "%s", q);
        char *sv = NULL;
        char *tok = strtok_r(qcopy, " \t", &sv);
        while (tok) {
            if (!is_stop_word(tok) && strlen(tok) > 3 && strstr(expl, tok))
                score += 1;
            tok = strtok_r(NULL, " \t", &sv);
        }
    }

    /* ── command full-text search (+1 per matching word) ────────────── */
    /* Same guard — only fires if something already scored.              */
    if (score > 0) {
        char cmd[2048];
        str_lower(e->command, cmd, sizeof cmd);
        char qcopy[512];
        snprintf(qcopy, sizeof qcopy, "%s", q);
        char *sv = NULL;
        char *tok = strtok_r(qcopy, " \t", &sv);
        while (tok) {
            if (!is_stop_word(tok) && strlen(tok) > 2 && strstr(cmd, tok))
                score += 1;
            tok = strtok_r(NULL, " \t", &sv);
        }
    }

    return score;
}

/* ── public API ─────────────────────────────────────────────────────────── */

#define MAX_RESULTS 5

int lookup_query(const char *query, LookupResult *results, int max_results)
{
    if (max_results > MAX_RESULTS) max_results = MAX_RESULTS;

    /* score all entries */
    typedef struct { int score; int idx; } Scored;
    Scored scored[GIT_DB_SIZE];

    for (size_t i = 0; i < GIT_DB_SIZE; i++) {
        scored[i].score = entry_score(&GIT_DB[i], query);
        scored[i].idx   = (int)i;
    }

    /* partial sort: bubble top N to front */
    for (int i = 0; i < max_results; i++) {
        for (size_t j = (size_t)i + 1; j < GIT_DB_SIZE; j++) {
            if (scored[j].score > scored[i].score) {
                Scored tmp = scored[i];
                scored[i]  = scored[j];
                scored[j]  = tmp;
            }
        }
    }

    /* minimum score threshold + gap pruning:
     * - drop anything below 3 (noise floor)
     * - stop when a result scores less than half the best result (big drop = noise) */
    int n = 0;
    int best = scored[0].score;
    for (int i = 0; i < max_results; i++) {
        if (scored[i].score < 4) break;
        /* gap pruning: stop if this result is less than 60% of best score */
        if (n > 0 && scored[i].score * 10 < best * 6) break;
        results[n].entry = &GIT_DB[scored[i].idx];
        results[n].score = scored[i].score;
        n++;
    }
    return n;
}
