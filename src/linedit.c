#include "linedit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#define LINEDIT_MAXLEN   1024
#define LINEDIT_HIST_MAX  200

/* ── history ring buffer ─────────────────────────────────────────────── */
static char  *hist[LINEDIT_HIST_MAX];
static int    hist_len = 0;

void linedit_add_history(const char *line)
{
    if (!line || !line[0]) return;
    /* don't add duplicate of last entry */
    if (hist_len > 0 && strcmp(hist[hist_len - 1], line) == 0) return;
    if (hist_len == LINEDIT_HIST_MAX) {
        free(hist[0]);
        memmove(hist, hist + 1, sizeof(char *) * (LINEDIT_HIST_MAX - 1));
        hist_len--;
    }
    hist[hist_len++] = strdup(line);
}

/* ── raw terminal helpers ────────────────────────────────────────────── */
static struct termios orig_termios;

static void disable_raw(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static int enable_raw(void)
{
    if (!isatty(STDIN_FILENO)) return -1;
    if (tcgetattr(STDIN_FILENO, &orig_termios) < 0) return -1;
    /* register cleanup only once */
    static int registered = 0;
    if (!registered) { atexit(disable_raw); registered = 1; }
    struct termios raw = orig_termios;
    raw.c_iflag &= (tcflag_t)~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= (tcflag_t)~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= (tcflag_t)~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) return -1;
    return 0;
}

/* write helper — ignores short writes (terminal output, best-effort) */
static void twrite(const char *s, size_t n) { if (write(STDOUT_FILENO, s, n) < 0) {} }
static void tstr(const char *s)             { twrite(s, strlen(s)); }

/* ── main line editor ────────────────────────────────────────────────── */
char *linedit(const char *prompt)
{
    /* non-tty fallback */
    if (!isatty(STDIN_FILENO)) {
        if (prompt) { fputs(prompt, stdout); fflush(stdout); }
        char buf[LINEDIT_MAXLEN];
        if (!fgets(buf, sizeof buf, stdin)) return NULL;
        size_t n = strlen(buf);
        while (n > 0 && (buf[n-1] == '\n' || buf[n-1] == '\r')) buf[--n] = '\0';
        return strdup(buf);
    }

    if (enable_raw() < 0) {
        /* can't set raw mode — plain fgets */
        if (prompt) { fputs(prompt, stdout); fflush(stdout); }
        char buf[LINEDIT_MAXLEN];
        if (!fgets(buf, sizeof buf, stdin)) return NULL;
        size_t n = strlen(buf);
        while (n > 0 && (buf[n-1] == '\n' || buf[n-1] == '\r')) buf[--n] = '\0';
        return strdup(buf);
    }

    char   buf[LINEDIT_MAXLEN];
    size_t len  = 0;   /* current input length */
    size_t pos  = 0;   /* cursor position */
    int    hi   = hist_len; /* history index (hist_len = "current" / not browsing) */
    char   saved[LINEDIT_MAXLEN] = {0}; /* saves current input while browsing history */

    /* print prompt */
    tstr(prompt ? prompt : "");

    for (;;) {
        unsigned char c;
        if (read(STDIN_FILENO, &c, 1) <= 0) {
            disable_raw();
            if (len == 0) return NULL; /* EOF with empty buffer */
            buf[len] = '\0';
            tstr("\r\n");
            return strdup(buf);
        }

        if (c == '\r' || c == '\n') {
            /* confirm */
            disable_raw();
            buf[len] = '\0';
            tstr("\r\n");
            return strdup(buf);
        }

        if (c == 4) {
            /* Ctrl-D */
            disable_raw();
            if (len > 0) {
                buf[len] = '\0';
                tstr("\r\n");
                return strdup(buf);
            }
            tstr("\r\n");
            return NULL; /* EOF */
        }

        if (c == 3) {
            /* Ctrl-C — return empty string, caller will ignore it */
            disable_raw();
            tstr("\r\n");
            buf[0] = '\0';
            return strdup(buf);
        }

        if (c == 127 || c == 8) {
            /* backspace */
            if (pos > 0) {
                memmove(buf + pos - 1, buf + pos, len - pos);
                pos--; len--;
                /* redraw */
                tstr("\r\033[K");
                tstr(prompt ? prompt : "");
                twrite(buf, len);
                /* reposition cursor */
                if (pos < len) {
                    char mv[32];
                    snprintf(mv, sizeof mv, "\033[%zuD", len - pos);
                    tstr(mv);
                }
            }
            continue;
        }

        if (c == 21) {
            /* Ctrl-U — clear line */
            pos = len = 0;
            tstr("\r\033[K");
            tstr(prompt ? prompt : "");
            continue;
        }

        if (c == 1) {
            /* Ctrl-A — beginning of line */
            pos = 0;
            tstr("\r\033[K");
            tstr(prompt ? prompt : "");
            twrite(buf, len);
            if (len > 0) {
                char mv[32];
                snprintf(mv, sizeof mv, "\033[%zuD", len);
                tstr(mv);
            }
            continue;
        }

        if (c == 5) {
            /* Ctrl-E — end of line */
            if (pos < len) {
                char mv[32];
                snprintf(mv, sizeof mv, "\033[%zuC", len - pos);
                tstr(mv);
                pos = len;
            }
            continue;
        }

        if (c == 27) {
            /* escape sequence */
            unsigned char seq[3] = {0};
            if (read(STDIN_FILENO, &seq[0], 1) <= 0) continue;
            if (seq[0] != '[') continue;
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) continue;

            if (seq[1] == 'A') {
                /* up arrow — older history */
                if (hist_len == 0) continue;
                if (hi == hist_len) {
                    /* save current buffer before browsing */
                    buf[len] = '\0';
                    snprintf(saved, sizeof saved, "%s", buf);
                }
                if (hi > 0) hi--;
                const char *entry = hist[hi];
                len = strlen(entry);
                pos = len;
                memcpy(buf, entry, len);
                tstr("\r\033[K");
                tstr(prompt ? prompt : "");
                twrite(buf, len);

            } else if (seq[1] == 'B') {
                /* down arrow — newer history */
                if (hi >= hist_len) continue;
                hi++;
                const char *entry = (hi == hist_len) ? saved : hist[hi];
                len = strlen(entry);
                pos = len;
                memcpy(buf, entry, len);
                tstr("\r\033[K");
                tstr(prompt ? prompt : "");
                twrite(buf, len);

            } else if (seq[1] == 'C') {
                /* right arrow */
                if (pos < len) {
                    tstr("\033[C");
                    pos++;
                }
            } else if (seq[1] == 'D') {
                /* left arrow */
                if (pos > 0) {
                    tstr("\033[D");
                    pos--;
                }
            } else if (seq[1] == '3') {
                /* DEL key: ESC [ 3 ~ */
                unsigned char tilde;
                if (read(STDIN_FILENO, &tilde, 1) > 0 && tilde == '~') {
                    if (pos < len) {
                        memmove(buf + pos, buf + pos + 1, len - pos - 1);
                        len--;
                        tstr("\r\033[K");
                        tstr(prompt ? prompt : "");
                        twrite(buf, len);
                        if (pos < len) {
                            char mv[32];
                            snprintf(mv, sizeof mv, "\033[%zuD", len - pos);
                            tstr(mv);
                        }
                    }
                }
            }
            continue;
        }

        /* printable character — insert at cursor */
        if (c >= 32 && len < LINEDIT_MAXLEN - 1) {
            if (pos < len)
                memmove(buf + pos + 1, buf + pos, len - pos);
            buf[pos] = (char)c;
            pos++; len++;
            /* redraw from cursor */
            twrite(buf + pos - 1, len - pos + 1);
            if (pos < len) {
                char mv[32];
                snprintf(mv, sizeof mv, "\033[%zuD", len - pos);
                tstr(mv);
            }
        }
    }
}
