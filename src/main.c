#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "globals.h"
#include "gitctx.h"
#include "lookup.h"
#include "ai.h"
#include "config.h"
#include "display.h"
#include "linedit.h"

#define LINE_MAX_LEN 1024
#define MAX_RESULTS   3

/* ── helpers ────────────────────────────────────────────────────────────── */

static void str_trim(char *s)
{
    char *p = s;
    while (*p == ' ' || *p == '\t') p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' ||
                     s[n-1] == ' '  || s[n-1] == '\t'))
        s[--n] = '\0';
}

/* ── core answer logic ──────────────────────────────────────────────────── */

typedef enum { MODE_AUTO, MODE_LOCAL, MODE_AI } QueryMode;

static void answer(const char *query, QueryMode mode,
                   const GitContext *ctx, const GpConfig *cfg)
{
    int ai_on = cfg->api_key && ai_available();

    if (mode == MODE_AI) {
        if (!ai_on) {
            fprintf(stderr, "AI not available (no API key or curl support).\n");
            return;
        }
        char ctx_str[8192];
        gitctx_to_prompt(ctx, ctx_str, sizeof ctx_str);
        display_ai_header();
        ai_query(ctx_str, query, &(AiConfig){
            .api_url = cfg->api_url,
            .api_key = cfg->api_key,
            .model   = cfg->model,
            .timeout = cfg->timeout
        });
        display_ai_footer();
        return;
    }

    /* local lookup */
    LookupResult results[MAX_RESULTS];
    int n = lookup_query(query, results, MAX_RESULTS);

    if (n == 0 || mode == MODE_AI) {
        display_no_results();
    } else {
        for (int i = 0; i < n; i++)
            display_result(&results[i], i + 1, n);
    }

    /* AI fallback when local score is low or no results */
    if (mode == MODE_AUTO && ai_on) {
        int best = (n > 0) ? results[0].score : 0;
        if (best < cfg->ai_thresh || n == 0) {
            printf("\n");
            display_separator();
            printf("Local confidence low — asking AI...\n");
            display_separator();
            char ctx_str[8192];
            gitctx_to_prompt(ctx, ctx_str, sizeof ctx_str);
            display_ai_header();
            ai_query(ctx_str, query, &(AiConfig){
                .api_url = cfg->api_url,
                .api_key = cfg->api_key,
                .model   = cfg->model,
                .timeout = cfg->timeout
            });
            display_ai_footer();
        }
    }
}

/* ── REPL ───────────────────────────────────────────────────────────────── */

static void repl(const GpConfig *cfg)
{
    GitContext ctx;
    int in_repo = gitctx_collect(&ctx);

    display_banner();

    if (!in_repo)
        printf("(not inside a git repository — context unavailable)\n");

    int ai_on = cfg->api_key && ai_available();
    printf("\n");
    display_help(ai_on);
    printf("\n");

    char line[LINE_MAX_LEN];

    for (;;) {
        char *input = linedit("\033[1mgitpal>\033[0m ");
        if (!input) {
            printf("\n");
            break;
        }
        /* copy into fixed buffer, trim */
        snprintf(line, sizeof line, "%s", input);
        free(input);
        str_trim(line);
        if (line[0] == '\0') continue;

        /* add non-empty, non-slash-command lines to history */
        if (line[0] != '/') linedit_add_history(line);

        /* ── built-in commands ── */
        if (strcmp(line, "/quit") == 0 || strcmp(line, "/q") == 0) break;

        if (strcmp(line, "/help") == 0 || strcmp(line, "?") == 0) {
            display_help(ai_on);
            continue;
        }

        if (strcmp(line, "/ctx") == 0) {
            /* refresh context */
            in_repo = gitctx_collect(&ctx);
            if (in_repo) gitctx_print(&ctx);
            else         printf("Not inside a git repository.\n");
            continue;
        }

        if (strcmp(line, "/config") == 0) {
            config_print(cfg);
            continue;
        }

        if (strncmp(line, "/ai ", 4) == 0) {
            /* refresh ctx before AI query */
            gitctx_collect(&ctx);
            answer(line + 4, MODE_AI, &ctx, cfg);
            continue;
        }

        if (strncmp(line, "/local ", 7) == 0) {
            answer(line + 7, MODE_LOCAL, &ctx, cfg);
            continue;
        }

        if (strcmp(line, "/verbose") == 0 || strcmp(line, "/v") == 0) {
            g_verbose = !g_verbose;
            printf("Verbose mode %s — gitpal will %sshow the git commands it runs.\n",
                   g_verbose ? "ON" : "OFF",
                   g_verbose ? "" : "no longer ");
            continue;
        }

        /* refresh context on every query so it reflects latest repo state */
        int repo_found = gitctx_collect(&ctx);
        if (g_verbose && !repo_found)
            fprintf(stderr,
                "\033[2m[not in a git repo — no context commands ran]\033[0m\n\n");
        answer(line, MODE_AUTO, &ctx, cfg);
    }
}

/* ── usage / version ────────────────────────────────────────────────────── */

#define GITPAL_VERSION "1.0.0"

static void print_usage(void)
{
    printf("gitpal " GITPAL_VERSION " — git companion for the terminal\n\n");
    printf("Usage:\n");
    printf("  gitpal                          interactive REPL mode\n");
    printf("  gitpal [flags] <question>       one-shot mode\n\n");
    printf("Flags:\n");
    printf("  -v, --verbose   show the git commands gitpal runs internally\n");
    printf("  --ai            skip local cheat sheet, go straight to AI\n");
    printf("  --local         skip AI, use local cheat sheet only\n");
    printf("  -h, --help      show this message\n");
    printf("  --version       show version\n\n");
    printf("Examples:\n");
    printf("  gitpal \"how do I undo my last commit\"\n");
    printf("  gitpal -v \"squash last 3 commits\"\n");
    printf("  gitpal --ai \"complex rebase with upstream divergence\"\n");
    printf("  gitpal                          # enter interactive mode\n\n");
    printf("REPL slash commands: /verbose, /ctx, /config, /ai, /local, /help, /quit\n");
}

/* ── entry point ────────────────────────────────────────────────────────── */

int main(int argc, char *argv[])
{
    GpConfig cfg;
    config_load(&cfg);

    if (argc >= 2) {
        /* scan flags first */
        char query[LINE_MAX_LEN] = {0};
        QueryMode mode = MODE_AUTO;
        int bad_flag = 0;

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                print_usage();
                return 0;
            }
            if (strcmp(argv[i], "--version") == 0) {
                printf("gitpal " GITPAL_VERSION "\n");
                return 0;
            }
            if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
                g_verbose = 1;
                continue;
            }
            if (strcmp(argv[i], "--ai") == 0)    { mode = MODE_AI;    continue; }
            if (strcmp(argv[i], "--local") == 0) { mode = MODE_LOCAL; continue; }
            if (argv[i][0] == '-') {
                fprintf(stderr, "gitpal: unknown flag '%s'\n", argv[i]);
                bad_flag = 1;
                continue;
            }
            /* non-flag args form the query */
            if (query[0]) strncat(query, " ", sizeof query - strlen(query) - 1);
            strncat(query, argv[i], sizeof query - strlen(query) - 1);
        }

        if (bad_flag) {
            fprintf(stderr, "Run 'gitpal --help' for usage.\n");
            return 1;
        }

        /* flags only, no query → drop into REPL */
        if (!query[0]) {
            if (g_verbose)
                fprintf(stderr,
                    "\033[2m[verbose mode on — gitpal will show the git commands it runs]\033[0m\n\n");
            repl(&cfg);            return 0;
        }

        if (g_verbose)
            fprintf(stderr,
                "\033[2m[verbose mode — gitpal will show the git commands it runs]\033[0m\n\n");

        GitContext ctx;
        int in_repo = gitctx_collect(&ctx);
        if (g_verbose && !in_repo)
            fprintf(stderr,
                "\033[2m[not in a git repo — no context commands ran]\033[0m\n\n");
        answer(query, mode, &ctx, &cfg);
        return 0;
    }

    repl(&cfg);
    return 0;
}
