# gitpal — How to Use

gitpal is a terminal tool that answers git questions in plain English.
You ask it something like *"how do I undo my last commit"* and it shows you the exact commands to run.

It works offline by default (built-in cheat sheet). If you give it an API key it can also ask an AI for harder questions.

---

## Installing it

### What you need first

**Linux (Fedora/RHEL):**
```sh
sudo dnf install gcc make libcurl-devel
```

**Linux (Debian/Ubuntu):**
```sh
sudo apt install gcc make libcurl4-openssl-dev
```

**macOS:**
```sh
xcode-select --install
```
That's it — curl is already on macOS.

### Build and install

```sh
tar xzf gitpal.tar.gz
cd gitpal
make
make install
```

You should see either:
```
[gitpal] Building WITH libcurl — AI backend enabled
```
or:
```
[gitpal] libcurl not found — building WITHOUT AI backend
```

Either is fine. The only difference is whether the AI fallback works.

After `make install`, the binary lands at `~/.local/bin/gitpal`. If your terminal says `command not found`, add this line to your `~/.bashrc` or `~/.zshrc`:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

Then open a new terminal tab and try:
```sh
gitpal "how do I undo my last commit"
```

---

## Using it — the two ways

### 1. Quick one-shot question

Pass your question as an argument:

```sh
gitpal "how do I undo my last commit"
gitpal "i accidentally committed to main instead of my branch"
gitpal "squash my last 3 commits"
```

Quotes are optional if there are no special shell characters (`&`, `*`, etc.):
```sh
gitpal how do i stash my work
```

### 2. Interactive mode (REPL)

Run it with no arguments and you get a prompt:

```sh
gitpal
```

```
gitpal — git companion  (? for help)

gitpal> i need to save my changes temporarily
gitpal> how do i get them back
gitpal> /quit
```

Type your question and press Enter. Type `/quit` or press `Ctrl-D` to exit.

Use this mode when you're in the middle of something and want to ask several questions in a row — it keeps your repo context fresh automatically.

---

## Learning git while you use gitpal — verbose mode

Verbose mode shows every git command gitpal runs **internally to read your repo state** — branch, status, recent commits, remotes. It does not affect the answer itself; the suggested git commands always show regardless.

**One-shot:**
```sh
gitpal --verbose "undo commit"
# or the short form:
gitpal -v "undo commit"
```

**In the REPL:**
```
gitpal> /verbose
Verbose mode ON — gitpal will show the git commands it runs.

gitpal> undo commit
  [gitpal runs: git branch --show-current]
  [gitpal runs: git status --short]
  [gitpal runs: git log --oneline -8]
  [gitpal runs: git remote -v]
  [gitpal runs: git rev-parse --git-dir]

▶ Command
  git reset --soft HEAD~1
...
```

Toggle it off again with `/verbose` or `/v`.

**Important:** the context commands only fire when you're inside a git repo. If you run gitpal from a directory with no `.git` parent, verbose mode tells you explicitly:

```
[verbose mode — gitpal will show the git commands it runs]
[not in a git repo — no context commands ran]

▶ Command
  git reset --soft HEAD~1
...
```

This is normal — the answer still works, it just has no repo context to read.

These are all real git plumbing commands you can run yourself any time. `git status --short` is a compact version of `git status`. `git log --oneline -8` shows your last 8 commits in a one-line format. `git branch --show-current` tells you which branch you're on. Once you've seen them a few times they'll become second nature.

---

## Reading the output

Every answer has the same structure. When there's one clear match:

```
▶ Command
  git reset --soft HEAD~1

✦ Explanation
  Undo the last commit but keep changes staged.
    Keep changes unstaged: git reset HEAD~1
    Discard changes too:   git reset --hard HEAD~1

⚠  Warning: --hard discards changes permanently.
```

When multiple matches are found (e.g. for an ambiguous query like "stash"):

```
── Match 1 of 3
▶ Command
  git stash push -m "description"
  git stash pop
  git stash list
...

── Match 2 of 3
▶ Command
  git stash pop
...
```

- **▶ Command** — the exact command(s) to run. Copy and paste these.
- **✦ Explanation** — what the command does. Indented lines are useful variants for slightly different situations.
- **⚠ Warning** — only appears when the command can't be undone, or rewrites git history. Read it before running.
- **Result 1/3** — gitpal shows up to 3 matches ranked by relevance. Result 1 is almost always what you want.

---

## The slash commands (REPL only)

| Type this | What it does |
|-----------|-------------|
| `/verbose` or `/v` | Toggle verbose mode on/off |
| `/ctx` | Show your current repo state (branch, changed files, recent commits) |
| `/config` | Show what API key and model gitpal is using |
| `/ai your question` | Skip the cheat sheet and go straight to AI |
| `/local your question` | Force cheat sheet only, never use AI |
| `?` or `/help` | Print the command list |
| `/quit` or `/q` or `Ctrl-D` | Exit |

### `/ctx` is useful when something looks wrong

If gitpal's answer doesn't seem right for your situation, run `/ctx` to see what gitpal actually sees about your repo:

```
gitpal> /ctx
  Branch : feature/login
  State  : MERGING
  Status :
    UU src/auth.c
  Recent :
    9de812a add refresh token support
    441fabc add login page
```

`UU` in the status means that file has unresolved merge conflicts. If State says `MERGING` or `REBASING`, gitpal and the AI will factor that in when answering.

---

## Setting up AI (optional)

Without an API key, gitpal uses its built-in cheat sheet — that covers the vast majority of everyday git use. You only need AI for unusual or complex situations.

### Pick a provider and get a key

| Provider | Where | Notes |
|----------|-------|-------|
| **Anthropic** | [console.anthropic.com](https://console.anthropic.com) | Best reasoning quality |
| **OpenAI** | [platform.openai.com](https://platform.openai.com) | Widely used |
| **Groq** | [console.groq.com](https://console.groq.com) | Has a free tier, very fast |
| **Ollama** | [ollama.com](https://ollama.com) | Runs locally, no account, no cost, no internet after setup |

### Create the config file

```sh
mkdir -p ~/.config/gitpal
nano ~/.config/gitpal/config
```

Paste the block for your chosen provider:

**Anthropic:**
```
GITPAL_API_URL=https://api.anthropic.com/v1/messages
GITPAL_API_KEY=sk-ant-api03-...
GITPAL_MODEL=claude-sonnet-4-20250514
```

**OpenAI:**
```
GITPAL_API_URL=https://api.openai.com/v1/chat/completions
GITPAL_API_KEY=sk-proj-...
GITPAL_MODEL=gpt-4o
```

**Groq:**
```
GITPAL_API_URL=https://api.groq.com/openai/v1/chat/completions
GITPAL_API_KEY=gsk_...
GITPAL_MODEL=llama-3.3-70b-versatile
```

**Ollama — fully offline:**

Install Ollama first from [ollama.com](https://ollama.com), then pull a model (one-time download, ~4 GB):
```sh
ollama pull qwen2.5-coder:7b
```

Then your config:
```
GITPAL_API_URL=http://localhost:11434/v1/chat/completions
GITPAL_API_KEY=ollama
GITPAL_MODEL=qwen2.5-coder:7b
```

### Check it worked

```sh
gitpal
gitpal> /config
  API URL    : https://api.anthropic.com/v1/messages
  Model      : claude-sonnet-4-20250514
  API key    : (set)
  AI thresh  : 2
  Timeout    : 60s
```

If it says `(not set — AI disabled)`: check the file path is exactly `~/.config/gitpal/config` and there are no spaces around the `=` signs.

**Note:** `/config` always shows values even without a config file — those are compiled-in defaults. The only sign no file exists is `API key: (not set — AI disabled)`. This is normal and not an error.

---

## How gitpal decides when to use AI

1. Searches the built-in cheat sheet first — instant, offline, no cost.
2. If the match is confident → shows cheat sheet result, done.
3. If the match is weak or there's no match → shows what it found, then automatically asks the AI (if configured). Your current repo state is included in the question so the answer is relevant to what you're actually working on.

You can always override:
- `/ai your question` — always goes to AI
- `/local your question` — always uses cheat sheet only

---

## Things you can ask

You don't need special syntax. Write what you're trying to do, or describe what went wrong.

**Undoing mistakes:**
```
undo my last commit
i committed to the wrong branch
remove the last 3 commits
i force pushed and messed up the remote
i think i lost some commits after a reset
```

**Saving unfinished work:**
```
save my changes without committing
stash my work so i can switch branches
get my stashed changes back
i have multiple stashes how do i pick one
```

**Cleaning up history before a PR:**
```
squash my last 4 commits into one
rebase my branch onto main
edit a commit message from 3 commits ago
reorder my commits
drop a commit i don't want
```

**Working with remotes:**
```
push my branch for the first time
force push safely
pull without creating a merge commit
my push was rejected because the remote has changes
```

**Finding things:**
```
who last changed line 40 in this file
find which commit introduced a bug
search all history for a deleted function name
show a visual graph of all branches
```

**Getting unstuck:**
```
i have a merge conflict how do i resolve it
my rebase stopped halfway through
the wrong files are staged how do i unstage them
i deleted a branch by accident
```

**Setup:**
```
set my git username and email
create a shortcut for git log --oneline --graph
```

---

## If something doesn't work

**"command not found" after install**
Add `~/.local/bin` to your PATH — see the install section above.

**gitpal gives an unrelated answer**
Try being more specific. Instead of `push`, try `push my branch for the first time`. If it still misses, use `/ai` to go straight to AI.

**"AI not available (no API key or curl support)"**
Either your API key isn't configured (check with `/config`) or the binary was built without libcurl. Fix the latter with:
```sh
sudo dnf install libcurl-devel   # or apt install libcurl4-openssl-dev
make clean && make && make install
```

**"API error: HTTP 401"**
Wrong API key. Re-copy it from your provider's dashboard — these are easy to fat-finger.

**"curl error: Could not resolve host"**
No network. Use `/local` to force the offline cheat sheet, or set up Ollama for a fully local AI.

**"not inside a git repository"**
You're in a folder with no `.git` directory above it. `cd` into your project first. Run `git status` to confirm git can see your repo.

---

## Uninstall

```sh
make uninstall
rm -rf ~/.config/gitpal
```
