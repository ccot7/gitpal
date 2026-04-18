#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* defaults */
#define DEFAULT_API_URL   "https://api.anthropic.com/v1/messages"
#define DEFAULT_MODEL     "claude-sonnet-4-20250514"
#define DEFAULT_AI_THRESH 2
#define DEFAULT_TIMEOUT   60L

static char g_api_url[512];
static char g_api_key[256];
static char g_model[128];
static int  g_ai_thresh = DEFAULT_AI_THRESH;
static long g_timeout   = DEFAULT_TIMEOUT;

static void trim(char *s)
{
    /* leading */
    char *p = s;
    while (*p == ' ' || *p == '\t') p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    /* trailing */
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' ||
                     s[n-1] == ' '  || s[n-1] == '\t'))
        s[--n] = '\0';
}

static void load_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) return;

    char line[512];
    while (fgets(line, sizeof line, fp)) {
        trim(line);
        if (line[0] == '#' || line[0] == '\0') continue;

        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        trim(key); trim(val);

        if (strcmp(key, "GITPAL_API_URL") == 0)
            snprintf(g_api_url, sizeof g_api_url, "%s", val);
        else if (strcmp(key, "GITPAL_API_KEY") == 0)
            snprintf(g_api_key, sizeof g_api_key, "%s", val);
        else if (strcmp(key, "GITPAL_MODEL") == 0)
            snprintf(g_model, sizeof g_model, "%s", val);
        else if (strcmp(key, "GITPAL_AI_THRESH") == 0)
            g_ai_thresh = atoi(val);
        else if (strcmp(key, "GITPAL_TIMEOUT") == 0)
            g_timeout = atol(val);
    }
    fclose(fp);
}

void config_load(GpConfig *cfg)
{
    /* 1. baked-in defaults */
    snprintf(g_api_url, sizeof g_api_url, "%s", DEFAULT_API_URL);
    snprintf(g_model,   sizeof g_model,   "%s", DEFAULT_MODEL);
    g_api_key[0] = '\0';
    g_ai_thresh  = DEFAULT_AI_THRESH;

    /* 2. config file: ~/.config/gitpal/config */
    const char *home = getenv("HOME");
    if (home) {
        char path[512];
        snprintf(path, sizeof path, "%s/.config/gitpal/config", home);
        load_file(path);
    }

    /* 3. env vars override everything */
    const char *e;
    if ((e = getenv("GITPAL_API_URL")))    snprintf(g_api_url, sizeof g_api_url, "%s", e);
    if ((e = getenv("GITPAL_API_KEY")))    snprintf(g_api_key, sizeof g_api_key, "%s", e);
    if ((e = getenv("GITPAL_MODEL")))      snprintf(g_model,   sizeof g_model,   "%s", e);
    if ((e = getenv("GITPAL_AI_THRESH")))  g_ai_thresh = atoi(e);
    if ((e = getenv("GITPAL_TIMEOUT")))    g_timeout   = atol(e);

    cfg->api_url    = g_api_url;
    cfg->api_key    = g_api_key[0] ? g_api_key : NULL;
    cfg->model      = g_model;
    cfg->ai_thresh  = g_ai_thresh;
    cfg->timeout    = g_timeout;
}

void config_print(const GpConfig *cfg)
{
    printf("  API URL    : %s\n", cfg->api_url);
    printf("  Model      : %s\n", cfg->model);
    printf("  API key    : %s\n", cfg->api_key ? "(set)" : "(not set — AI disabled)");
    printf("  AI thresh  : %d  (local score below this triggers AI fallback)\n", cfg->ai_thresh);
    printf("  Timeout    : %lds\n", cfg->timeout);
}
