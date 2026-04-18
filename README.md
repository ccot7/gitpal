# gitpal

A terminal git companion for senior engineers.  
Answers git questions instantly from a built-in reference database.  
Falls back to an AI backend (Claude / OpenAI / Ollama / any OpenAI-compatible API) for complex or contextual queries — with your actual repo state injected automatically.

## Features

- **Zero-latency local lookup** — fuzzy keyword search over a curated ~96-entry git reference
- **AI fallback** — activates automatically when local confidence is low, or on demand
- **Repo-aware AI context** — branch, status, recent log, and in-progress merge/rebase state are injected into every AI query
- **One-shot and REPL modes** — use inline or interactively
- **OpenAI-compatible API** — works with Claude, OpenAI, Ollama (local), Groq, Mistral, etc.
- **Zero runtime deps** beyond libcurl (optional — AI disabled if absent)
- Builds clean under `-Wall -Wextra -Wpedantic` on GCC and clang, Linux and macOS

## Build

```sh
# requires: cc, make, (optional) libcurl-dev
make
make install        # installs to ~/.local/bin/gitpal
```

macOS with Homebrew curl:
```sh
export PKG_CONFIG_PATH="$(brew --prefix curl)/lib/pkgconfig"
make
```

## Configuration

`~/.config/gitpal/config` (or env vars — env overrides file):

```sh
# Which API endpoint to use (default: Anthropic Claude)
GITPAL_API_URL=https://api.anthropic.com/v1/messages
GITPAL_API_KEY=sk-ant-...
GITPAL_MODEL=claude-sonnet-4-20250514

# OpenAI
# GITPAL_API_URL=https://api.openai.com/v1/chat/completions
# GITPAL_API_KEY=sk-...
# GITPAL_MODEL=gpt-4o

# Ollama (local, no key needed)
# GITPAL_API_URL=http://localhost:11434/v1/chat/completions
# GITPAL_API_KEY=ollama
# GITPAL_MODEL=qwen2.5-coder:7b
```

Without an API key, gitpal runs in local-only mode (no AI).

## Usage

**Interactive REPL:**
```
$ gitpal
gitpal> i need to commit now
gitpal> how do i uncommit the last patch
gitpal> squash last 4 commits
gitpal> /ctx                   — show current repo state
gitpal> /ai rebase my branch onto origin/main with conflict strategy
gitpal> /local how do i stash
gitpal> /config
gitpal> ?                      — help
```

**One-shot:**
```sh
gitpal "how do i undo a commit"
gitpal --ai "i pushed to wrong branch, what now"
gitpal --local "how do i stash"
```

## Slash commands (REPL)

| Command | Effect |
|---------|--------|
| `/ai <query>` | Force AI, skip local lookup |
| `/local <query>` | Force local lookup, skip AI |
| `/ctx` | Show current repo context |
| `/config` | Show active configuration |
| `?` / `/help` | Help |
| `/quit` | Exit |

## AI fallback logic

1. Run fuzzy local lookup (scored keyword match)
2. If best score ≥ threshold → show local result only
3. If best score < threshold → show local result (if any) **and** automatically query AI with repo context
4. `/ai` always goes straight to AI; `/local` never uses AI

## Project layout

```
gitpal/
  src/
    main.c      — REPL, one-shot, arg parsing
    lookup.c    — fuzzy keyword scorer + full-text search
    gitref.h    — reference database (~96 entries, compiled into binary)
    gitctx.c    — git repo context collector (popen)
    ai.c        — libcurl SSE streaming, OpenAI-compatible
    config.c    — config file + env var loader
    display.c   — ANSI output, result formatting
    linedit.c   — minimal line editor (arrow key history, no deps)
    globals.c   — process-wide flags (g_verbose)
  tools/
    tldr_to_gitref.py   — converter: tldr-pages → gitref.h entries
    tldr/git-pages.md   — bundled tldr git reference (43 subcommands)
  tests.sh      — 145-test suite (sh tests.sh)
  Makefile
  README.md
  how_to_use.md
```

## How the cheatsheet works

The entire reference database is `src/gitref.h` — a static C array compiled directly into the binary. There is no external file at runtime. `src/lookup.c` scores queries against it using a 3-tier keyword matcher:

1. Full keyword phrase found in query → **+5**
2. Full query found inside a keyword phrase → **+3**
3. Per query-word found in keyword phrase → **+1**
4. Per query-word found in explanation (tiebreaker) → **+0.5**

Results scoring below 3 are dropped. Results scoring less than 60% of the top result are also dropped (gap pruning). A single unambiguous query returns one result with no rank header; multiple close matches show `Match X of N`.

## Verbose mode

`-v` / `--verbose` prints the git commands gitpal runs internally to read your repo state. The answer itself is always shown regardless.

```sh
gitpal -v "undo commit"
```

Inside a repo:
```
[verbose mode — gitpal will show the git commands it runs]

  [gitpal runs: git branch --show-current]
  [gitpal runs: git status --short]
  [gitpal runs: git log --oneline -8]
  [gitpal runs: git remote -v]
  [gitpal runs: git rev-parse --git-dir]

▶ Command
  git reset --soft HEAD~1
```

Outside a repo:
```
[verbose mode — gitpal will show the git commands it runs]
[not in a git repo — no context commands ran]

▶ Command
  git reset --soft HEAD~1
```

## Config defaults

`/config` always shows values even without a config file — those are compiled-in defaults. The only sign no file exists is `API key: (not set)`. To add a key:

```sh
mkdir -p ~/.config/gitpal
echo "GITPAL_API_KEY=sk-ant-..." >> ~/.config/gitpal/config
```

## Extending the reference database

Add entries to `src/gitref.h` — no other files need changing:

```c
{
    {"your", "keywords", "here", NULL},
    "git command --to --run",
    "Explanation of what it does.",
    "Optional danger warning, or NULL."
},
```

Rebuild with `make`.
