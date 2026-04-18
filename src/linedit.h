/*
 * linedit.h — minimal line editor with arrow-key history for gitpal
 *
 * API matches the parts of linenoise we use:
 *   char *linedit(const char *prompt);   -- returns malloc'd line, caller frees; NULL on EOF
 *   void  linedit_add_history(const char *line);
 *
 * Falls back gracefully to plain fgets if terminal is not a tty.
 */

#ifndef LINEDIT_H
#define LINEDIT_H

char *linedit(const char *prompt);
void  linedit_add_history(const char *line);

#endif /* LINEDIT_H */
