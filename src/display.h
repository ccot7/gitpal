#ifndef DISPLAY_H
#define DISPLAY_H

#include "lookup.h"

void display_result(const LookupResult *r, int rank, int total);
void display_no_results(void);
void display_ai_header(void);
void display_ai_footer(void);
void display_separator(void);
void display_banner(void);
void display_help(int ai_enabled);

#endif /* DISPLAY_H */
