#ifndef GITCTX_H
#define GITCTX_H

#include <stddef.h>

#define GITCTX_BUF 2048

typedef struct {
    int   in_repo;
    int   merging;
    int   rebasing;
    char  branch[256];
    char  status[GITCTX_BUF];
    char  log[GITCTX_BUF];
    char  remotes[512];
    char  git_dir[512];
} GitContext;

/* Populate ctx from the current working directory.
 * Returns 1 if inside a git repo, 0 otherwise. */
int gitctx_collect(GitContext *ctx);

/* Pretty-print context to stdout. */
void gitctx_print(const GitContext *ctx);

/* Serialise context into a prompt-ready string. */
void gitctx_to_prompt(const GitContext *ctx, char *out, size_t max);

#endif /* GITCTX_H */
