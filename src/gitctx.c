#include "gitctx.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void run_cmd(const char *cmd, char *out, size_t max)
{
    out[0] = '\0';

    /* verbose: show the git command, stripped of shell plumbing noise */
    if (g_verbose && strncmp(cmd, "git", 3) == 0) {
        /* copy just up to first '2>' or '|' — those are shell noise */
        char display[256];
        const char *end = cmd;
        while (*end && !(end[0] == '2' && end[1] == '>') && end[0] != '|')
            end++;
        size_t len = (size_t)(end - cmd);
        /* trim trailing spaces */
        while (len > 0 && cmd[len-1] == ' ') len--;
        snprintf(display, sizeof display, "%.*s", (int)len, cmd);
        fprintf(stderr, "\033[2m  [gitpal runs: %s]\033[0m\n", display);
    }

    FILE *fp = popen(cmd, "r");
    if (!fp) return;
    size_t n = fread(out, 1, max - 1, fp);
    out[n] = '\0';
    pclose(fp);
    /* trim trailing newline */
    size_t len = strlen(out);
    while (len > 0 && (out[len-1] == '\n' || out[len-1] == '\r'))
        out[--len] = '\0';
}

int gitctx_collect(GitContext *ctx)
{
    memset(ctx, 0, sizeof *ctx);

    /* are we inside a git repo? */
    FILE *fp = popen("git rev-parse --is-inside-work-tree 2>/dev/null", "r");
    if (!fp) return 0;
    char check[8] = {0};
    size_t nr = fread(check, 1, sizeof check - 1, fp);
    (void)nr;
    pclose(fp);
    if (strncmp(check, "true", 4) != 0) return 0;
    ctx->in_repo = 1;

    if (g_verbose)
        fprintf(stderr, "\033[2m[reading repo context:]\033[0m\n");

    run_cmd("git branch --show-current 2>/dev/null",
            ctx->branch, sizeof ctx->branch);

    run_cmd("git status --short 2>/dev/null",
            ctx->status, sizeof ctx->status);

    run_cmd("git log --oneline -8 2>/dev/null",
            ctx->log, sizeof ctx->log);

    run_cmd("git remote -v 2>/dev/null | head -4",
            ctx->remotes, sizeof ctx->remotes);

    /* detect in-progress operations */
    run_cmd("git rev-parse --git-dir 2>/dev/null",
            ctx->git_dir, sizeof ctx->git_dir);

    if (ctx->git_dir[0]) {
        char path[1600];
        snprintf(path, sizeof path, "test -f %s/MERGE_HEAD && echo yes", ctx->git_dir);
        char tmp[8]; run_cmd(path, tmp, sizeof tmp);
        ctx->merging = (strncmp(tmp, "yes", 3) == 0);

        snprintf(path, sizeof path,
                 "test -d %s/rebase-merge -o -d %s/rebase-apply && echo yes",
                 ctx->git_dir, ctx->git_dir);
        run_cmd(path, tmp, sizeof tmp);
        ctx->rebasing = (strncmp(tmp, "yes", 3) == 0);
    }

    return 1;
}

/* Return a short legend string for a git status --short XY code */
static const char *status_legend(char x, char y)
{
    /* untracked */
    if (x == '?' && y == '?') return "untracked";
    /* merge conflicts */
    if (x == 'U' || y == 'U') return "merge conflict (unmerged)";
    if (x == 'A' && y == 'A') return "merge conflict (both added)";
    if (x == 'D' && y == 'D') return "merge conflict (both deleted)";
    /* staged + unstaged combos */
    if (x == 'M' && y == 'M') return "modified (staged + unstaged changes)";
    if (x == 'M' && y == ' ') return "modified (staged)";
    if (x == ' ' && y == 'M') return "modified (not staged)";
    if (x == 'A' && y == ' ') return "new file (staged)";
    if (x == 'A' && y == 'M') return "new file (staged, also modified)";
    if (x == 'D' && y == ' ') return "deleted (staged)";
    if (x == ' ' && y == 'D') return "deleted (not staged)";
    if (x == 'R')              return "renamed";
    if (x == 'C')              return "copied";
    if (x == '!')              return "ignored";
    return "";
}

void gitctx_print(const GitContext *ctx)
{
    printf("  Branch : %s\n", ctx->branch[0] ? ctx->branch : "(detached HEAD)");
    if (ctx->merging)  printf("  State  : \033[33mMERGING\033[0m  — resolve conflicts then: git merge --continue\n");
    if (ctx->rebasing) printf("  State  : \033[33mREBASING\033[0m — resolve conflicts then: git rebase --continue\n");

    if (ctx->status[0]) {
        printf("  Status :\n");
        const char *p = ctx->status;
        while (*p) {
            const char *nl = strchr(p, '\n');
            int len = nl ? (int)(nl - p) : (int)strlen(p);
            if (len >= 2) {
                char x = p[0], y = p[1];
                const char *leg = status_legend(x, y);
                if (leg[0])
                    printf("    %.*s  \033[2m(%s)\033[0m\n", len, p, leg);
                else
                    printf("    %.*s\n", len, p);
            }
            p += len + (nl ? 1 : 0);
        }
    } else {
        printf("  Status : clean\n");
    }

    if (ctx->log[0]) {
        printf("  Recent :\n");
        const char *p = ctx->log;
        while (*p) {
            const char *nl = strchr(p, '\n');
            int len = nl ? (int)(nl - p) : (int)strlen(p);
            printf("    %.*s\n", len, p);
            p += len + (nl ? 1 : 0);
        }
    }

    if (ctx->remotes[0]) {
        /* just show unique remote names, not the full fetch/push pairs */
        printf("  Remotes:\n");
        const char *p = ctx->remotes;
        char last_name[64] = {0};
        while (*p) {
            const char *nl = strchr(p, '\n');
            int len = nl ? (int)(nl - p) : (int)strlen(p);
            /* extract remote name (first token) */
            char name[64] = {0};
            int i = 0;
            while (i < len && i < 63 && p[i] != '\t' && p[i] != ' ') {
                name[i] = p[i];
                i++;
            }
            if (strcmp(name, last_name) != 0) {
                printf("    %.*s\n", len, p);
                snprintf(last_name, sizeof last_name, "%s", name);
            }
            p += len + (nl ? 1 : 0);
        }
    }
}

/* Build a compact context string for AI prompt injection */
void gitctx_to_prompt(const GitContext *ctx, char *out, size_t max)
{
    int n = snprintf(out, max,
        "Current git context:\n"
        "Branch: %s%s%s\n"
        "Status:\n%s\n"
        "Recent commits:\n%s\n"
        "Remotes:\n%s\n",
        ctx->branch[0] ? ctx->branch : "(detached)",
        ctx->merging  ? " [MERGING]"  : "",
        ctx->rebasing ? " [REBASING]" : "",
        ctx->status[0]  ? ctx->status  : "(clean)",
        ctx->log[0]     ? ctx->log     : "(none)",
        ctx->remotes[0] ? ctx->remotes : "(none)"
    );
    (void)n;
}
