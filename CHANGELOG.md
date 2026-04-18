# Changelog

All notable changes to gitpal are documented here.

---

## [1.0.0] — Initial release

### Added
- **Core binary** — single C binary, zero runtime deps beyond optional libcurl
- **Local cheat sheet** — ~47 hand-written entries covering the 80% daily git use case
- **3-tier fuzzy scorer** — phrase match (+6), substring match (+3), word overlap (+1)
- **REPL mode** — interactive session with slash commands: `/ctx`, `/config`, `/ai`, `/local`, `/help`, `/quit`
- **One-shot mode** — `gitpal "your question"` answers and exits
- **AI backend** — SSE streaming over libcurl, Anthropic + OpenAI-compatible endpoints
- **Config system** — `~/.config/gitpal/config` file + env var overrides
- **Repo context injection** — branch, status, log, remotes, MERGING/REBASING state sent to AI automatically
- **ANSI output** — colour-coded commands (green), explanations, warnings (yellow), dim headers
- **`-Wall -Wextra -Wpedantic` clean** on GCC and clang, Linux and macOS

---

## [1.1.0] — Verbose mode + AI improvements

### Added
- **`--verbose` / `-v` flag** — prints the git commands gitpal runs internally to read repo state
- **`/verbose` REPL toggle** — turn verbose on/off mid-session
- **"not in a git repo" notice** — verbose mode explicitly says when outside a repo instead of silently showing nothing
- **Context commands stripped of shell noise** — `2>/dev/null` and `| head` no longer shown in verbose output; commands display as `git branch --show-current` not `git branch --show-current 2>/dev/null`
- **`[reading repo context:]` header** — groups the 5 context commands visually

### Fixed
- AI endpoint detection — Anthropic uses `x-api-key` + `anthropic-version` header; OpenAI-compatible uses `Authorization: Bearer`. Previously both were sent to every endpoint
- Correct JSON body format per endpoint — Anthropic uses top-level `"system"`, OpenAI uses messages array
- `GITPAL_TIMEOUT` config key — configurable curl timeout (default 60s)
- `GITPAL_AI_THRESH` config key — tune how aggressively AI fallback activates

---

## [1.2.0] — linedit, config polish, DB expansion

### Added
- **`linedit.c`** — drop-in minimal line editor: arrow-key history (200 entries), left/right cursor, Ctrl-A/E/U, Del key, Ctrl-C, graceful `fgets` fallback when stdin is not a tty
- **`--help` / `-h`** — proper usage output, exits 0
- **`--version`** — prints version string, exits 0
- **Unknown flag error** — prints "gitpal: unknown flag 'X'" + help hint, exits 1
- **`-v` alone drops into REPL** — flags without a query start interactive mode
- **`/config` output** — now shows AI thresh and timeout alongside URL/model/key
- **New gitref.h entries** — git worktree, git sparse-checkout, git format-patch/am, git hooks, git lfs, shallow clone, git notes, git archive, git init, git bundle, delete remote branch, rebase --skip, GPG signed commit

### Fixed
- `Result X/3` header replaced with `Match X of N` using actual result count
- Score hidden from output (was internal implementation detail)
- Single unambiguous query returns one result with no rank header

---

## [1.3.0] — tldr-pages integration + full-text scorer

### Added
- **`tools/tldr_to_gitref.py`** — converter: parses tldr-style `.md` files into `gitref.h` C struct initialisers
- **`tools/tldr/git-pages.md`** — bundled 43-subcommand tldr reference
- **Generated block in `gitref.h`** — `BEGIN/END GENERATED FROM TLDR` markers; `--update` flag injects without touching hand-written entries
- **Full-text search** — explanation and command text now scored (+1 per matching word), guarded to only fire when keywords already matched (prevents noise from rich explanations)
- **Stop-word filter** — generic words (the, a, git, how, do, can, set, get…) excluded from word-overlap scoring
- **Partial match min length raised to 4 chars** — prevents short words (add, set, run) from false-scoring

### Fixed
- Gap pruning: results scoring < 60% of top result dropped (reduces noise tail)
- Noise floor: results below score 4 suppressed
- `re.sub` lambda fix in `update_gitref` — Python's `re.sub` interprets `\n` in replacement strings as literal newlines, destroying C escape sequences; fixed by using a lambda replacement

---

## [1.4.0] — Real tldr-pages merge + scoring stability

### Added
- **Real tldr-pages data** — generated block expanded from 43 to ~150 entries using actual tldr-pages repo (`git clone --depth 1 https://github.com/tldr-pages/tldr.git`)
- **12 new strong hand-written entries** added to beat real-tldr noise on key queries: `uncommit`, `staged changes`, `grep tracked files`, `shortlog`, `git describe`, `revert commit`, `git mv`, `git rm --cached`, `ls-files ignored`, `git gc`, `push tags`, `see staged changes`
- **Makefile header dependency tracking** — `%.o: %.c $(HEADERS)` so editing `gitref.h` now correctly triggers recompilation without needing `make clean`

### Fixed
- **`atexit(disable_raw)` registered once** — previously called on every `enable_raw()` invocation (once per REPL query); now uses a static flag
- **Keyword collision fixes** — removed `"history"` from log entry (was matching "move a file and keep history"), removed `"show commit"` from git show entry (was matching "show commit history"), removed bare `"commit"` from plain commit entry (was matching inside "uncommit")
- Verbose section in `tests.sh` rewritten to explicitly create a temp dir outside any git repo for the "outside repo" test — previously relied on current directory which could be inside a repo depending on where tests are run
- `tests.sh` grep false-flag bugs fixed: `-u` and `--verbose` were being interpreted as grep flags

### Changed
- `tldr_to_gitref.py` parser improved: handles both `-` and `*` bullet markers, looks ahead up to 3 lines for backtick commands (real tldr format has blank lines between description and command)

---

## Scoring reference (current)

| Match type | Score |
|------------|-------|
| Full keyword phrase in query | +6 |
| Full query in keyword phrase | +3 |
| Per query-word (>4 chars) in keyword | +1 |
| Per query-word in explanation (if keyword matched) | +1 |
| Noise floor cutoff | < 4 → suppressed |
| Gap pruning | < 60% of best → suppressed |
