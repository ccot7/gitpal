#include "ai.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef GITPAL_NO_CURL
/* ── stub when built without curl ─────────────────────────────────────── */
int ai_available(void) { return 0; }
int ai_query(const char *ctx, const char *query, const AiConfig *cfg)
{
    (void)ctx; (void)query; (void)cfg;
    fprintf(stderr, "AI support not compiled in (rebuild with libcurl).\n");
    return -1;
}
#else
/* ── curl implementation ──────────────────────────────────────────────── */
#include <curl/curl.h>

int ai_available(void) { return 1; }

/* ── SSE streaming write callback ─────────────────────────────────────── */
typedef struct {
    char buf[256];   /* carry-over from previous chunk */
    size_t buf_len;
} SseState;

/* Extract "content" delta text from a single SSE data: line.
 * Supports both OpenAI and Anthropic streaming formats.
 * Writes extracted text to stdout immediately.            */
static void process_sse_line(const char *line)
{
    if (strncmp(line, "data: ", 6) != 0) return;
    const char *json = line + 6;
    if (strcmp(json, "[DONE]") == 0) return;

    /* OpenAI: "delta":{"content":"<text>"} */
    const char *needle = "\"content\":\"";
    const char *p = strstr(json, needle);

    /* Anthropic: "type":"content_block_delta"..."text":"<text>" */
    if (!p) {
        if (!strstr(json, "content_block_delta")) return;
        needle = "\"text\":\"";
        p = strstr(json, needle);
    }
    if (!p) return;

    p += strlen(needle);
    /* unescape and print until closing unescaped quote */
    while (*p && *p != '"') {
        if (*p == '\\' && *(p+1)) {
            p++;
            switch (*p) {
                case 'n':  putchar('\n'); break;
                case 't':  putchar('\t'); break;
                case '"':  putchar('"');  break;
                case '\\': putchar('\\'); break;
                default:   putchar(*p);  break;
            }
        } else {
            putchar(*p);
        }
        p++;
    }
    fflush(stdout);
}

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    SseState *st = (SseState *)userdata;
    size_t total = size * nmemb;
    size_t pos = 0;

    while (pos < total) {
        /* find newline in incoming data */
        size_t remaining = total - pos;
        char *nl = memchr(ptr + pos, '\n', remaining);
        size_t chunk = nl ? (size_t)(nl - (ptr + pos)) : remaining;

        /* append to carry-over buffer */
        size_t avail = sizeof(st->buf) - st->buf_len - 1;
        size_t copy  = chunk < avail ? chunk : avail;
        memcpy(st->buf + st->buf_len, ptr + pos, copy);
        st->buf_len += copy;
        st->buf[st->buf_len] = '\0';

        if (nl) {
            process_sse_line(st->buf);
            st->buf_len = 0;
            pos += chunk + 1; /* skip \n */
        } else {
            pos += chunk;
        }
    }
    return total;
}

/* ── JSON helpers (no external lib) ──────────────────────────────────── */
static void json_escape(const char *src, char *dst, size_t max)
{
    size_t di = 0;
    for (size_t i = 0; src[i] && di + 4 < max; i++) {
        unsigned char c = (unsigned char)src[i];
        if      (c == '"')  { dst[di++]='\\'; dst[di++]='"';  }
        else if (c == '\\') { dst[di++]='\\'; dst[di++]='\\'; }
        else if (c == '\n') { dst[di++]='\\'; dst[di++]='n';  }
        else if (c == '\r') { dst[di++]='\\'; dst[di++]='r';  }
        else if (c == '\t') { dst[di++]='\\'; dst[di++]='t';  }
        else                { dst[di++] = (char)c; }
    }
    dst[di] = '\0';
}

/* ── public query function ────────────────────────────────────────────── */
int ai_query(const char *git_ctx, const char *query, const AiConfig *cfg)
{
    CURL *curl = curl_easy_init();
    if (!curl) return -1;

    /* detect endpoint type from URL */
    int is_anthropic = (strstr(cfg->api_url, "anthropic.com") != NULL);

    if (g_verbose)
        fprintf(stderr, "\033[2m  [gitpal AI: POST %s model=%s]\033[0m\n",
                cfg->api_url, cfg->model);

    /* build system prompt */
    static const char SYS[] =
        "You are an expert git assistant. "
        "Rules: 1) Lead with exact git commands in a code block. "
        "2) Brief explanation after — no padding. "
        "3) Warn about dangerous operations (history rewrite, force push, hard reset). "
        "4) If context shows MERGING or REBASING state, address that first.";

    /* escape inputs */
    char esc_sys[1024], esc_ctx[4096], esc_q[1024];
    json_escape(SYS,     esc_sys, sizeof esc_sys);
    json_escape(git_ctx, esc_ctx, sizeof esc_ctx);
    json_escape(query,   esc_q,   sizeof esc_q);

    char user_msg[6144];
    snprintf(user_msg, sizeof user_msg, "%s\\n\\nQuestion: %s", esc_ctx, esc_q);

    /* build JSON body — Anthropic uses top-level "system", OpenAI uses messages array */
    char body[8192];
    if (is_anthropic) {
        snprintf(body, sizeof body,
            "{"
            "\"model\":\"%s\","
            "\"max_tokens\":1024,"
            "\"stream\":true,"
            "\"system\":\"%s\","
            "\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}]"
            "}",
            cfg->model, esc_sys, user_msg);
    } else {
        /* OpenAI-compatible: system message goes in the messages array */
        snprintf(body, sizeof body,
            "{"
            "\"model\":\"%s\","
            "\"max_tokens\":1024,"
            "\"stream\":true,"
            "\"messages\":["
              "{\"role\":\"system\",\"content\":\"%s\"},"
              "{\"role\":\"user\",\"content\":\"%s\"}"
            "]"
            "}",
            cfg->model, esc_sys, user_msg);
    }

    /* headers — send only what each endpoint expects */
    struct curl_slist *hdrs = NULL;
    char auth_hdr[300];
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json");

    if (is_anthropic) {
        snprintf(auth_hdr, sizeof auth_hdr, "x-api-key: %s", cfg->api_key);
        hdrs = curl_slist_append(hdrs, auth_hdr);
        hdrs = curl_slist_append(hdrs, "anthropic-version: 2023-06-01");
    } else {
        snprintf(auth_hdr, sizeof auth_hdr, "Authorization: Bearer %s", cfg->api_key);
        hdrs = curl_slist_append(hdrs, auth_hdr);
    }

    SseState st = {0};

    curl_easy_setopt(curl, CURLOPT_URL, cfg->api_url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, cfg->timeout);

    CURLcode res = curl_easy_perform(curl);
    printf("\n");

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        return -1;
    }
    if (http_code >= 400) {
        fprintf(stderr, "API error: HTTP %ld — check your API key and model name.\n", http_code);
        return -1;
    }
    return 0;
}
#endif /* GITPAL_NO_CURL */
