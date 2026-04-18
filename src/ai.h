#ifndef AI_H
#define AI_H

typedef struct {
    const char *api_url;
    const char *api_key;
    const char *model;
    long        timeout;
} AiConfig;

/* Returns 1 if compiled with curl support, 0 otherwise. */
int ai_available(void);

/* Stream AI response to stdout.  Returns 0 on success, -1 on error. */
int ai_query(const char *git_ctx, const char *query, const AiConfig *cfg);

#endif /* AI_H */
