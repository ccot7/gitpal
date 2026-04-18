#include "display.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* detect color support */
static int use_color(void)
{
    static int cached = -1;
    if (cached < 0)
        cached = isatty(STDOUT_FILENO) && getenv("NO_COLOR") == NULL;
    return cached;
}

#define C_RESET   (use_color() ? "\033[0m"     : "")
#define C_BOLD    (use_color() ? "\033[1m"     : "")
#define C_DIM     (use_color() ? "\033[2m"     : "")
#define C_GREEN   (use_color() ? "\033[32m"    : "")
#define C_YELLOW  (use_color() ? "\033[33m"    : "")
#define C_CYAN    (use_color() ? "\033[36m"    : "")
#define C_RED     (use_color() ? "\033[31m"    : "")
#define C_MAGENTA (use_color() ? "\033[35m"    : "")

/* print a multi-line command block with syntax tinting */
static void print_command(const char *cmd)
{
    char buf[2048];
    snprintf(buf, sizeof buf, "%s", cmd);
    char *line = buf;
    while (*line) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';

        /* comment line */
        if (line[0] == '#') {
            printf("  %s%s%s\n", C_DIM, line, C_RESET);
        } else {
            /* dim the flags/options, highlight git verb */
            printf("  %s%s%s\n", C_GREEN, line, C_RESET);
        }

        if (!nl) break;
        line = nl + 1;
    }
}

void display_result(const LookupResult *r, int rank, int total)
{
    const GitEntry *e = r->entry;

    if (total > 1)
        printf("\n%s── Match %d of %d%s\n", C_BOLD, rank, total, C_RESET);
    else
        printf("\n");

    /* command block */
    printf("%s▶ Command%s\n", C_CYAN, C_RESET);
    print_command(e->command);

    /* explanation */
    printf("\n%s✦ Explanation%s\n", C_BOLD, C_RESET);
    /* indent each line */
    char buf[2048];
    snprintf(buf, sizeof buf, "%s", e->explanation);
    char *line = buf;
    while (*line) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        printf("  %s\n", line);
        if (!nl) break;
        line = nl + 1;
    }

    /* warning */
    if (e->warning) {
        printf("\n%s⚠  Warning:%s %s\n", C_YELLOW, C_RESET, e->warning);
    }
}

void display_no_results(void)
{
    printf("%sNo local match found.%s\n", C_DIM, C_RESET);
}

void display_ai_header(void)
{
    printf("\n%s╔══ AI Answer ══════════════════════════════╗%s\n",
           C_MAGENTA, C_RESET);
}

void display_ai_footer(void)
{
    printf("%s╚═══════════════════════════════════════════╝%s\n",
           C_MAGENTA, C_RESET);
}

void display_separator(void)
{
    printf("%s────────────────────────────────────────────%s\n",
           C_DIM, C_RESET);
}

void display_banner(void)
{
    printf("%sgitpal%s — git companion  "
           "%s(? for help)%s\n",
           C_BOLD, C_RESET, C_DIM, C_RESET);
}

void display_help(int ai_enabled)
{
    printf("\n%sCommands:%s\n", C_BOLD, C_RESET);
    printf("  %s/ctx%s            show current git context\n",       C_CYAN, C_RESET);
    printf("  %s/config%s         show active configuration\n",      C_CYAN, C_RESET);
    printf("  %s/verbose%s  %s/v%s   toggle verbose mode (shows git commands gitpal runs)\n",
           C_CYAN, C_RESET, C_CYAN, C_RESET);
    printf("  %s/ai <query>%s     force AI even if local match found\n", C_CYAN, C_RESET);
    printf("  %s/local <q>%s      force local lookup only\n",        C_CYAN, C_RESET);
    printf("  %s?%s  %s/help%s         this message\n",
           C_CYAN, C_RESET, C_CYAN, C_RESET);
    printf("  %s/quit%s  %sCTRL-D%s    exit\n",
           C_CYAN, C_RESET, C_CYAN, C_RESET);
    printf("\n%sJust type your question in plain English.%s\n",
           C_DIM, C_RESET);
    if (!ai_enabled)
        printf("%sAI fallback inactive (set GITPAL_API_KEY to enable).%s\n",
               C_YELLOW, C_RESET);
}
