#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    const char *api_url;
    const char *api_key;   /* NULL means AI disabled */
    const char *model;
    int         ai_thresh; /* min local score before offering AI */
    long        timeout;   /* curl timeout in seconds */
} GpConfig;

void config_load(GpConfig *cfg);
void config_print(const GpConfig *cfg);

#endif /* CONFIG_H */
