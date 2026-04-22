# gitpal - terminal git companion

Answers git questions instantly in plain English. No googling, no man pages.

```sh
$ gitpal "undo last commit"
▶ Command
  git reset --soft HEAD~1

$ gitpal "committed to wrong branch"
▶ Command
  git cherry-pick <hash>  # on the correct branch
  git reset --hard HEAD~1  # on the wrong branch

$ gitpal "force push safely"
▶ Command
  git push --force-with-lease
```

Works offline by default - built-in reference of ~190 entries compiled into the binary. Falls back to an AI backend when local confidence is low.

Can intentionally use AI on demand, using a flag. **Your live repo context is injected automatically - super handy especially When using AI.** 

---

## Why

Git is powerful but hard to recall under pressure. gitpal lets you describe intent in plain English and shows the exact command instantly - no guesswork, no context switching.

---

## Features

- **Zero-latency local lookup** - no API, no network, no delay. Fuzzy keyword search over a comprehensive git reference compiled into the binary
- **AI fallback - only when needed** - activates automatically when local confidence is low, or on demand with `/ai`
- **Repo-aware AI context** - branch, status, recent log, and in-progress merge/rebase state are injected into every AI query automatically
- **Verbose mode** - shows the git commands gitpal runs internally to read your repo; great for learning git itself
- **Interactive REPL + one-shot** - use inline or as a persistent session with arrow-key history
- **OpenAI-compatible** - works with Claude, OpenAI, Ollama (local/offline), Groq, Mistral, or any OpenAI-compatible endpoint
- **Zero runtime deps** beyond libcurl (optional - AI disabled if absent)
- Builds clean under `-Wall -Wextra -Wpedantic` on GCC and clang, Linux and macOS

---

## Quick start

```sh
git clone https://github.com/ccot7/gitpal.git
cd gitpal
make          # requires: cc, make, (optional) libcurl-dev
make install  # installs to ~/.local/bin/gitpal
```

Make sure `~/.local/bin` is in your PATH:
```sh
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc  # or ~/.zshrc
```

Then:
```sh
gitpal "undo last commit"
gitpal -v "rebase my branch onto main"
gitpal --ai "i messed up a merge, what now"
```

---

## Installation

### Prerequisites

| Platform | Required | Optional (AI backend) |
|----------|----------|-----------------------|
| Linux (Fedora/RHEL) | `gcc make` | `libcurl-devel` |
| Linux (Debian/Ubuntu) | `gcc make` | `libcurl4-openssl-dev` |
| macOS | Xcode CLT: `xcode-select --install` | curl ships with macOS |

```sh
# Fedora/RHEL
sudo dnf install gcc make libcurl-devel

# Debian/Ubuntu
sudo apt install gcc make libcurl4-openssl-dev

# macOS - curl included, just need CLT
xcode-select --install
```

macOS with Homebrew curl:
```sh
export PKG_CONFIG_PATH="$(brew --prefix curl)/lib/pkgconfig"
make
```

### Build output

```
[gitpal] Building WITH libcurl - AI backend enabled
# or
[gitpal] libcurl not found - building WITHOUT AI backend
```

Either is fine. The only difference is whether AI fallback works.

### Install

```sh
make install          # → ~/.local/bin/gitpal
sudo make install PREFIX=/usr/local  # system-wide
make uninstall        # remove
```

---

## Usage

### One-shot mode

```sh
gitpal "how do I undo my last commit"
gitpal "i accidentally committed to main"
gitpal "squash my last 3 commits"
gitpal how do i stash my work    # quotes optional
```

Flags:
```sh
gitpal -v "undo commit"          # verbose - show git commands gitpal runs
gitpal --local "stash"           # force cheat sheet only, no AI
gitpal --ai "complex scenario"   # force AI, skip cheat sheet
gitpal --help
gitpal --version
```

### Interactive REPL

```sh
gitpal
```

```
gitpal - git companion  (? for help)

gitpal> i need to commit now
gitpal> how do i uncommit the last patch
gitpal> squash last 4 commits
gitpal> /ctx
gitpal> /ai rebase my branch onto origin/main with conflict strategy
gitpal> /local how do i stash
gitpal> /config
gitpal> ?
gitpal> /quit
```

Arrow keys browse history. Ctrl-A/E jump line start/end. Ctrl-U clears line.

### REPL slash commands

| Command | Effect |
|---------|--------|
| `/verbose` or `/v` | Toggle verbose mode on/off |
| `/ctx` | Show current repo state (branch, status, recent commits, remotes) |
| `/config` | Show active configuration |
| `/ai <query>` | Force AI, skip cheat sheet |
| `/local <query>` | Force cheat sheet only, no AI |
| `?` or `/help` | Print help |
| `/quit` or `/q` or Ctrl-D | Exit |

---

## Reading the output

**Single unambiguous match - no header:**
```
▶ Command
  git reset --soft HEAD~1

✦ Explanation
  Undo the last commit but keep changes staged.
    Keep changes unstaged:  git reset HEAD~1
    Discard changes too:    git reset --hard HEAD~1

⚠  Warning: --hard discards changes permanently.
```

**Multiple close matches:**
```
── Match 1 of 3
▶ Command
  git stash push -m "description"
...

── Match 2 of 3
▶ Command
  git stash pop
...
```

- **▶ Command** - exact commands to run. Copy and paste.
- **✦ Explanation** - what it does. Indented lines are variants for slightly different situations.
- **⚠ Warning** - only shown when the command is irreversible, rewrites history, or overwrites remote.
- **Match X of N** - only shown when multiple results are returned.

---

## Verbose mode - learn git while using gitpal

`-v` / `--verbose` shows the git commands gitpal runs **internally** to read your repo state before answering. The answer is always shown regardless.

```sh
gitpal -v "undo commit"
```

Inside a repo:
```
[verbose mode - gitpal will show the git commands it runs]

[reading repo context:]
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
[verbose mode - gitpal will show the git commands it runs]
[not in a git repo - no context commands ran]

▶ Command
  git reset --soft HEAD~1
```

These are real git plumbing commands you can run yourself. `git status --short` is a compact `git status`. `git log --oneline -8` shows the last 8 commits in one-line format. Toggle in REPL with `/verbose` or `/v`.

---

## `/ctx` - current repo state

```
gitpal> /ctx
  Branch : feature/login [MERGING]
  Status :
    UU src/auth.c    (merge conflict, unmerged)
    M  src/utils.c   (modified, not staged)
    ?? newfile.h     (untracked)
  Recent :
    9de812a add refresh token support
    441fabc add login page
```

Status codes explained inline: `M` = modified, `??` = untracked, `UU` = merge conflict, `A` = new file staged, `D` = deleted. State shows `[MERGING]` or `[REBASING]` when in progress - gitpal uses this context in every AI query.

---

## AI backend setup (optional)

Without an API key, gitpal uses the built-in cheat sheet - covers ~95% of everyday git use. AI is for unusual or multi-step scenarios.

### Pick a provider

| Provider | Where | Notes |
|----------|-------|-------|
| **Anthropic** | [console.anthropic.com](https://console.anthropic.com) | Best reasoning |
| **OpenAI** | [platform.openai.com](https://platform.openai.com) | Widely used |
| **Groq** | [console.groq.com](https://console.groq.com) | Free tier, very fast |
| **Ollama** | [ollama.com](https://ollama.com) | Runs locally, no account, no cost, no internet after setup |
| **Mistral** | [console.mistral.ai](https://console.mistral.ai) | EU-based |

### Create config file

```sh
mkdir -p ~/.config/gitpal
nano ~/.config/gitpal/config
```

**Anthropic (default):**
```sh
GITPAL_API_URL=https://api.anthropic.com/v1/messages
GITPAL_API_KEY=sk-ant-api03-...
GITPAL_MODEL=claude-sonnet-4-20250514
```

**OpenAI:**
```sh
GITPAL_API_URL=https://api.openai.com/v1/chat/completions
GITPAL_API_KEY=sk-proj-...
GITPAL_MODEL=gpt-4o
```

**Groq (free tier available):**
```sh
GITPAL_API_URL=https://api.groq.com/openai/v1/chat/completions
GITPAL_API_KEY=gsk_...
GITPAL_MODEL=llama-3.3-70b-versatile
```

**Ollama - fully offline:**
```sh
# First: install from https://ollama.com then:
ollama pull qwen2.5-coder:7b   # one-time ~4 GB download

GITPAL_API_URL=http://localhost:11434/v1/chat/completions
GITPAL_API_KEY=ollama
GITPAL_MODEL=qwen2.5-coder:7b
```

### Environment variables

All config keys can be set as env vars - they override the config file:
```sh
GITPAL_API_KEY=sk-ant-... gitpal --ai "squash my last 3 commits"
GITPAL_MODEL=gpt-4o gitpal
```

| Variable | Default | Description |
|----------|---------|-------------|
| `GITPAL_API_URL` | `https://api.anthropic.com/v1/messages` | API endpoint |
| `GITPAL_API_KEY` | _(none - AI disabled)_ | API key |
| `GITPAL_MODEL` | `claude-sonnet-4-20250514` | Model name |
| `GITPAL_AI_THRESH` | `2` | Min local score before auto-fallback to AI |
| `GITPAL_TIMEOUT` | `60` | curl timeout in seconds |
| `NO_COLOR` | _(unset)_ | Set to disable ANSI colours |

### Verify config

```sh
gitpal
gitpal> /config
  API URL    : https://api.anthropic.com/v1/messages
  Model      : claude-sonnet-4-20250514
  API key    : (set)
  AI thresh  : 2
  Timeout    : 60s
```

`/config` always shows values even without a config file - those are compiled-in defaults. The only sign no file exists is `API key: (not set - AI disabled)`. That is normal and not an error.

---

## AI fallback logic

1. Run fuzzy local lookup against the built-in cheat sheet (instant, offline)
2. If best score ≥ threshold → show local result, done
3. If score < threshold or no match → show local result (if any) **and** automatically query AI with full repo context injected
4. `/ai` always goes straight to AI; `/local` never uses AI

---

## Things you can ask

Write what you're trying to do, or describe what went wrong.

**Undoing mistakes:**
```
undo my last commit
i committed to the wrong branch
remove the last 3 commits
i force pushed and messed up the remote
i lost some commits after a reset
```

**Saving unfinished work:**
```
stash my changes
get my stashed changes back
i have multiple stashes how do i pick one
```

**Cleaning up history before a PR:**
```
squash my last 4 commits into one
rebase my branch onto main
edit a commit message from 3 commits ago
reorder my commits
```

**Working with remotes:**
```
push my branch for the first time
force push safely
pull without creating a merge commit
delete a remote branch
my push was rejected because the remote has changes
```

**Finding things:**
```
who last changed line 40 in this file
find which commit introduced a bug
search all history for a deleted function
show commits between two dates
```

**Getting unstuck:**
```
i have a merge conflict how do i fix it
my rebase stopped halfway through
skip a commit during rebase
recover a deleted branch
```

---

## How the cheat sheet works

The entire reference database is `src/gitref.h` - a static C array compiled directly into the binary. No external files at runtime. `src/lookup.c` scores queries using a 3-tier keyword matcher:

1. Full keyword phrase found in query → **+6**
2. Full query found inside a keyword phrase → **+3**
3. Per query-word (>4 chars, non-stop-word) in keyword → **+1**
4. Per query-word in explanation text (only if keywords already matched) → **+1**

Results below score 4 are suppressed. Results scoring less than 60% of the top result are also dropped (gap pruning). Single unambiguous queries return one result with no rank header.

### Regenerating from real tldr-pages

The bundled `tools/tldr/git-pages.md` covers 43 subcommands. For the full ~150-entry set from the community-maintained [tldr-pages](https://github.com/tldr-pages/tldr) project:

```sh
git clone --depth 1 https://github.com/tldr-pages/tldr.git /tmp/tldr
python3 tools/tldr_to_gitref.py --dir /tmp/tldr/pages/common/ --update src/gitref.h
make clean && make
```

This replaces only the `GENERATED BLOCK` in `gitref.h` - hand-written entries above it are untouched.

---

## Extending the reference database

Add entries to `src/gitref.h` - no other files need changing. Rebuild with `make`.

```c
{
    {"your keyword", "another phrase", "natural language variant", NULL},
    "git command --to --run\ngit alternative --command",
    "Explanation of what it does.\n"
    "  Variant:  git command --other-flag",
    "Optional danger warning, or NULL if safe."
},
```

**Keyword tuning tips:**
- Multi-word phrases score more reliably than single words - use `"undo last commit"` not just `"undo"`
- Include natural-language variants: `"i committed to wrong branch"`, `"uncommit"`
- Include symptom phrasing: `"accidentally pushed"`, `"lost commit after reset"`
- Up to 7 keywords + NULL per entry (8-slot array limit)
- Danger warning field: shown in yellow with ⚠ - use for anything irreversible or history-rewriting

---

## Project layout

```
gitpal/
  src/
    main.c      - REPL, one-shot, arg parsing, flag handling
    lookup.c    - 3-tier fuzzy scorer + full-text search + gap pruning
    gitref.h    - reference database (~190 entries, compiled into binary)
    gitctx.c    - git repo context collector via popen
    ai.c        - libcurl SSE streaming, Anthropic + OpenAI-compatible
    config.c    - config file (~/.config/gitpal/config) + env var loader
    display.c   - ANSI output, result formatting, colour detection
    linedit.c   - minimal line editor (arrow history, no external deps)
    globals.c   - process-wide flags (g_verbose)
  tools/
    tldr_to_gitref.py   - converter: tldr-pages → gitref.h C entries
    tldr/git-pages.md   - bundled tldr git reference (43 subcommands)
  tests.sh      - 147-test suite: sh tests.sh
  Makefile
  README.md
  .gitignore
```

---

## Troubleshooting

**`command not found` after `make install`**
Add `~/.local/bin` to PATH - see Installation above.

**`AI not available (no API key or curl support)`**
Either your API key isn't set (check `/config`) or the binary was built without libcurl:
```sh
sudo dnf install libcurl-devel   # Fedora
sudo apt install libcurl4-openssl-dev   # Ubuntu
make clean && make && make install
```

**`API error: HTTP 401`**
Wrong API key. Re-copy from your provider's dashboard.

**`API error: HTTP 400`**
Wrong model name. Check `GITPAL_MODEL` matches the provider's model string exactly.

**`curl error: Could not resolve host`**
No network. Use `/local` to force offline cheat sheet, or set up Ollama.

**gitpal gives an unrelated answer**
Try more specific phrasing. Instead of `push`, try `push my branch for the first time`. If it still misses, use `/ai`.

**`/config` shows defaults but I set a config file**
Check the file path is exactly `~/.config/gitpal/config` with no spaces around `=` signs and no quotes around values.

**`not inside a git repository`**
You're in a folder with no `.git` parent. `cd` into your project first.

---

## Test suite

```sh
sh tests.sh              # run all 147 tests
sh tests.sh 2>/dev/null  # suppress gitpal stderr output
```

Sections: flags/meta, commit operations, staging, stash, branch, merge, rebase, remote/push/pull, inspection/history, recovery, tags, advanced, config, hooks/lfs/shallow, no-match, result count, verbose mode (inside repo, outside repo, generic), flag combinations, warnings, natural language variants, edge cases.

---

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

---

## License

MIT - see [LICENSE](LICENSE).

---

## 👤 Author
**Cadmus of Tyre**  
GitHub: [@ccot7](https://github.com/ccot7)

---
> *The best git command is the one you don't have to remember.*
