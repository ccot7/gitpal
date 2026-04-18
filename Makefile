CC      ?= cc
PREFIX  ?= $(HOME)/.local

# Sources
SRCS    := src/main.c src/lookup.c src/gitctx.c src/ai.c \
           src/config.c src/display.c src/globals.c src/linedit.c
OBJS    := $(SRCS:.c=.o)
TARGET  := gitpal

CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2 -D_POSIX_C_SOURCE=200809L

# ── Auto-detect libcurl ──────────────────────────────────────────────────
CURL_CFLAGS := $(shell pkg-config --cflags libcurl 2>/dev/null)
CURL_LIBS   := $(shell pkg-config --libs   libcurl 2>/dev/null)

ifeq ($(CURL_LIBS),)
  # pkg-config not found — try linking directly
  CURL_LIBS := -lcurl
endif

# Try a compile probe to decide whether curl is actually present
HAVE_CURL := $(shell echo '\#include <curl/curl.h>\nint main(){return 0;}' > /tmp/_gp_curl_probe.c && \
             $(CC) /tmp/_gp_curl_probe.c -o /dev/null $(CURL_CFLAGS) $(CURL_LIBS) 2>/dev/null \
             && echo yes || echo no)

ifeq ($(HAVE_CURL),yes)
  CFLAGS  += $(CURL_CFLAGS)
  LDFLAGS += $(CURL_LIBS)
  $(info [gitpal] Building WITH libcurl — AI backend enabled)
else
  CFLAGS  += -DGITPAL_NO_CURL
  $(info [gitpal] libcurl not found — building WITHOUT AI backend)
endif

# ── macOS: silence deprecated POSIX warnings ────────────────────────────
UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
  CFLAGS += -D_DARWIN_C_SOURCE
endif

# ── Targets ─────────────────────────────────────────────────────────────
.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Header dependencies — changes to any .h trigger recompile of all .o
HEADERS := $(wildcard src/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(TARGET)
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin/$(TARGET)
	@echo "Installed to $(PREFIX)/bin/$(TARGET)"
	@echo "Ensure $(PREFIX)/bin is in your PATH."

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
