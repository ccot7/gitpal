#ifndef GITREF_H
#define GITREF_H

#include <stddef.h>

typedef struct {
    const char *keywords[8];  /* NULL-terminated list of trigger words */
    const char *command;      /* primary command(s) to show */
    const char *explanation;  /* brief explanation */
    const char *warning;      /* optional danger warning, NULL if none */
} GitEntry;

/* ---------------------------------------------------------------------------
 * Master reference table.  Add entries freely — matched via fuzzy keyword
 * scoring in lookup.c.
 * -------------------------------------------------------------------------*/
static const GitEntry GIT_DB[] = {

    /* ── COMMIT ─────────────────────────────────────────────────────────── */
    {
        {"make a commit", "save", "record", "snapshot", "create commit", NULL},
        "git add -p && git commit -m \"<message>\"",
        "Stage changes interactively then commit with a message.",
        NULL
    },
    {
        {"amend", "fix commit", "edit commit", "last commit message", "change message", NULL},
        "git commit --amend",
        "Rewrite the most recent commit (message or staged content).\n"
        "  Amend message only:  git commit --amend -m \"new message\"\n"
        "  Add forgotten file:  git add <file> && git commit --amend --no-edit",
        "Rewrites history — avoid if already pushed."
    },
    {
        {"uncommit", "undo commit", "undo last commit", "revert commit",
         "remove last commit", "how do i undo", NULL},
        "git reset --soft HEAD~1",
        "Undo the last commit but keep changes staged.\n"
        "  Keep changes unstaged:  git reset HEAD~1\n"
        "  Discard changes too:    git reset --hard HEAD~1",
        "--hard discards changes permanently."
    },
    {
        {"wrong branch", "committed wrong branch", "commit wrong branch",
         "move commit to another branch", "commit on wrong branch", NULL},
        "git cherry-pick <hash>  # on the correct branch\ngit reset --hard HEAD~1  # on the wrong branch",
        "Cherry-pick the commit onto the right branch, then remove it from the wrong one.",
        "--hard discards the commit from wrong branch — ensure cherry-pick succeeded first."
    },
    {
        {"empty commit", "trigger ci", "force pipeline", NULL},
        "git commit --allow-empty -m \"chore: trigger CI\"",
        "Create a commit with no file changes (useful to re-trigger CI).",
        NULL
    },

    /* ── STAGING ─────────────────────────────────────────────────────────── */
    {
        {"stage", "add", "track", "index", "stage my changes",
         "stage files", "add files to commit", NULL},
        "git add <file>\ngit add -p          # interactive hunk staging\ngit add -u          # all tracked modified files",
        "Add files or hunks to the staging area.",
        NULL
    },
    {
        {"see staged changes", "staged changes", "what is staged",
         "diff staged", "show staged", NULL},
        "git diff --staged",
        "Show what has been staged (added) but not yet committed.\n"
        "  git diff           # unstaged changes\n"
        "  git diff --staged  # staged vs last commit",
        NULL
    },
    {
        {"unstage", "unadd", "remove from stage", "unstage a file",
         "undo git add", "remove staged", NULL},
        "git restore --staged <file>",
        "Remove a file from the staging area without touching the working copy.",
        NULL
    },
    {
        {"discard", "revert file", "undo changes", "restore file", NULL},
        "git restore <file>",
        "Discard working directory changes for a file (reverts to last commit).",
        "Unrecoverable — changes are lost."
    },

    /* ── STASH ───────────────────────────────────────────────────────────── */
    {
        {"stash", "stash changes", "shelve", "put aside", "save work temporarily",
         "stash my changes", "save my work temporarily", NULL},
        "git stash push -m \"description\"\ngit stash pop           # restore latest\ngit stash list          # view all stashes",
        "Temporarily shelve uncommitted changes.",
        NULL
    },
    {
        {"stash", "apply stash", "restore stash", "pop stash",
         "get stash back", "get back my stashed work", "get my stashed work back", NULL},
        "git stash pop               # apply and remove\ngit stash apply stash@{N}   # apply specific, keep in list",
        "Restore stashed changes.",
        NULL
    },
    {
        {"drop stash", "delete stash", "clear stash", "remove stash",
         "drop a stash", "wipe stash", "delete all stashes", NULL},
        "git stash drop stash@{0}\ngit stash clear   # remove ALL stashes",
        "Delete one or all stashes.",
        "git stash clear is irreversible."
    },

    /* ── BRANCH ──────────────────────────────────────────────────────────── */
    {
        {"branch", "create branch", "new branch", "checkout branch", NULL},
        "git switch -c <branch>              # create and switch\ngit switch <branch>                 # switch existing\ngit branch -a                       # list all (incl. remote)",
        "Manage branches.",
        NULL
    },
    {
        {"delete branch", "remove branch", "delete a branch",
         "delete local branch", "remove local branch", NULL},
        "git branch -d <branch>    # safe delete (merged only)\ngit branch -D <branch>    # force delete",
        "Delete a local branch.",
        "-D force-deletes even if unmerged."
    },
    {
        {"rename branch", "move branch", "rename a branch",
         "rename local branch", "change branch name", NULL},
        "git branch -m <old> <new>",
        "Rename a local branch.",
        NULL
    },
    {
        {"track remote", "set upstream", "link branch", NULL},
        "git branch --set-upstream-to=origin/<branch>",
        "Link local branch to a remote tracking branch.",
        NULL
    },

    /* ── MERGE ───────────────────────────────────────────────────────────── */
    {
        {"merge", "integrate", "combine branches", NULL},
        "git merge <branch>\ngit merge --no-ff <branch>   # always create merge commit",
        "Merge another branch into the current one.",
        NULL
    },
    {
        {"merge conflict", "resolve conflict", "conflict", NULL},
        "git status                    # see conflicted files\n# edit files, resolve <<<<< markers\ngit add <resolved-file>\ngit merge --continue",
        "After resolving conflicts in your editor, stage files and continue.",
        NULL
    },
    {
        {"abort merge", "cancel merge", "stop merge", "quit merge",
         "get out of merge", "undo merge", NULL},
        "git merge --abort",
        "Abort an in-progress merge and return to pre-merge state.",
        NULL
    },

    /* ── REBASE ──────────────────────────────────────────────────────────── */
    {
        {"replay commits", "rebase onto", "rebase branch", "rebase onto main", "rebase my branch", NULL},
        "git rebase <base-branch>\ngit rebase origin/main",
        "Reapply commits on top of another branch (linear history).",
        "Rewrites history — avoid on shared/pushed branches."
    },
    {
        {"interactive rebase", "rebase", "squash", "reorder commits",
         "squash commits", "combine commits", "combine my last", NULL},
        "git rebase -i HEAD~<N>   # replace N with number of commits, e.g. HEAD~3",
        "Interactively squash, reorder, edit, or drop commits.\n"
        "  interactive rebase: git rebase -i HEAD~<N>\n"
        "  In the editor that opens, change 'pick' to:\n"
        "    squash  — combine into previous commit (keeps message)\n"
        "    fixup   — combine into previous commit (discards message)\n"
        "    reword  — edit the commit message\n"
        "    drop    — delete the commit entirely\n"
        "    reorder — just move the lines up/down",
        "Rewrites history — avoid if already pushed."
    },
    {
        {"rebase conflict", "conflict during rebase", NULL},
        "# resolve conflict in editor, then:\ngit add <resolved-file>\ngit rebase --continue\n# or bail out:\ngit rebase --abort",
        "Resolve conflicts during a rebase one commit at a time.",
        NULL
    },
    {
        {"abort rebase", "cancel rebase", "stop rebase", "quit rebase",
         "get out of rebase", NULL},
        "git rebase --abort",
        "Stop the rebase and restore the original branch state.",
        NULL
    },

    /* ── REMOTE / PUSH / PULL ────────────────────────────────────────────── */
    {
        {"push", "upload", "publish", "send commits",
         "push for first time", "first push",
         "push my branch for the first time", NULL},
        "git push\ngit push -u origin <branch>   # first push of new branch",
        "Push commits to remote.\n"
        "  First push of a new branch:  git push -u origin <branch>",
        NULL
    },
    {
        {"force push", "overwrite remote", "force-push", "push force",
         "force push safely", "push --force", "overwrite remote branch", NULL},
        "git push --force-with-lease",
        "Force push but fail if remote has commits you haven't seen (safer than --force).",
        "Overwrites remote history — coordinate with team."
    },
    {
        {"pull", "fetch and merge", "update local",
         "pull without merge commit", "pull without creating merge commit", NULL},
        "git pull --rebase   # recommended: rebase instead of merge commit\ngit pull            # fetch + merge",
        "Fetch and integrate remote changes.\n"
        "  No merge commit:  git pull --rebase\n"
        "  Fast-forward only: git pull --ff-only",
        NULL
    },
    {
        {"fetch", "see remote changes", "download refs", NULL},
        "git fetch --prune   # fetch all + remove stale remote refs",
        "Download remote refs without merging.",
        NULL
    },
    {
        {"remote", "add remote", "list remotes", NULL},
        "git remote -v                          # list\ngit remote add origin <url>            # add\ngit remote set-url origin <url>        # change URL",
        "Manage remote connections.",
        NULL
    },

    /* ── LOG / DIFF / INSPECT ────────────────────────────────────────────── */
    {
        {"log", "commit history", "show commit history",
         "show commits between dates", "show commits between two dates",
         "search all history", "search history for string", NULL},
        "git log --oneline --graph --decorate --all",
        "Visual commit graph.\n"
        "  Filter by author:    git log --author=\"name\"\n"
        "  Filter by file:      git log -- <file>\n"
        "  Search message:      git log --grep=\"keyword\"\n"
        "  Search added/removed string: git log -S \"string\" --oneline\n"
        "  Between dates:       git log --after=\"2024-01-01\" --before=\"2024-12-31\"\n"
        "  Show diffs too:      git log -p",
        NULL
    },
    {
        {"what changed in last commit", "show last commit",
         "what did i commit", "view a commit", "inspect commit", NULL},
        "git show                    # last commit diff + message\ngit show <hash>             # specific commit\ngit show --stat             # files changed, no diff",
        "Show what changed in a commit.",
        NULL
    },
    {
        {"diff", "what changed", "see changes", "show changes",
         "what have i changed", "see what i changed",
         "see what i have changed", NULL},
        "git diff              # unstaged changes\ngit diff --staged     # staged vs last commit\ngit diff HEAD~1       # last commit vs working tree",
        "Show changes between states.",
        NULL
    },
    {
        {"blame", "who wrote", "find author", "who changed a line",
         "who last changed", "find who wrote", "find who changed", NULL},
        "git blame <file>\ngit blame -L 10,20 <file>   # lines 10-20 only",
        "Show who last modified each line of a file.",
        NULL
    },
    {
        {"bisect", "binary search", "find which commit", "find bug commit",
         "introduced a bug", "which commit broke", NULL},
        "git bisect start\ngit bisect bad              # current commit is bad\ngit bisect good <hash>      # known good commit\n# test, then: git bisect good/bad\ngit bisect reset            # done",
        "Binary search through history to find which commit introduced a bug.",
        NULL
    },
    {
        {"search code", "find string", "grep commits",
         "search for string in all tracked files", "search tracked files", NULL},
        "git grep \"pattern\"                      # search working tree\ngit log -S \"search_string\" --oneline   # commits that added/removed string",
        "Search through tracked files or history.\n"
        "  Search working tree:  git grep \"pattern\"\n"
        "  Search history:       git log -S \"string\" --oneline",
        NULL
    },
    {
        {"shortlog", "summarize commits by author", "how many commits per person",
         "commit count per author", "commits per person", NULL},
        "git shortlog -sn",
        "Summarize commits by author.\n"
        "  Sorted count:  git shortlog -sn\n"
        "  All branches:  git shortlog -sn --all",
        NULL
    },
    {
        {"describe", "get human readable name for commit", "describe current commit",
         "human readable commit name", "tag based version", NULL},
        "git describe\ngit describe --tags\ngit describe --always",
        "Give a human-readable name to a commit based on the nearest tag.",
        NULL
    },
    {
        {"revert commit", "revert a commit", "revert a commit without rewriting history",
         "undo a commit safely", "undo pushed commit", NULL},
        "git revert <hash>",
        "Create a new commit that undoes a previous commit — safe for shared branches.\n"
        "  Revert without committing: git revert -n <hash>\n"
        "  Revert a merge commit:     git revert -m 1 <merge_hash>",
        NULL
    },
    {
        {"move file", "rename file", "git mv", "move a file and keep history",
         "keep history", "rename a file in git", "rename tracked file", NULL},
        "git mv <old_name> <new_name>",
        "Move or rename a file and stage the change.\n"
        "  Move to directory: git mv <file> <directory>/",
        NULL
    },
    {
        {"remove from tracking", "untrack a file", "remove file from tracking keep on disk",
         "stop tracking file", "git rm cached", NULL},
        "git rm --cached <file>",
        "Stop tracking a file without deleting it from disk.\n"
        "  Remove from disk too: git rm <file>\n"
        "  Remove directory:     git rm -r <directory>",
        NULL
    },
    {
        {"list ignored files", "show ignored files", "ignored files",
         "see ignored", "ls-files ignored", NULL},
        "git ls-files --ignored --exclude-standard",
        "List files that git is ignoring.\n"
        "  Untracked files:  git ls-files --others --exclude-standard\n"
        "  All tracked:      git ls-files\n"
        "  Deleted:          git ls-files --deleted",
        NULL
    },
    {
        {"git gc", "clean", "garbage collection", "clean up git objects",
         "optimise repo", "git garbage collection", NULL},
        "git gc\ngit gc --aggressive",
        "Clean up unnecessary files and optimise the local repository.\n"
        "  Standard:    git gc\n"
        "  Aggressive:  git gc --aggressive  (slower, more thorough)",
        NULL
    },
    {
        {"push tags", "push all tags", "push tag to remote",
         "upload tags", "publish tags", NULL},
        "git push origin --tags",
        "Push all tags to remote.\n"
        "  Push a specific tag: git push origin <tag_name>\n"
        "  Delete remote tag:   git push origin --delete <tag_name>",
        NULL
    },

    /* ── TAGS ────────────────────────────────────────────────────────────── */
    {
        {"tag", "release", "version", "create tag",
         "create a tag", "add a tag", "tag a commit", NULL},
        "git tag v1.0.0                         # lightweight\ngit tag -a v1.0.0 -m \"Release 1.0\"     # annotated\ngit push origin --tags                 # push all tags",
        "Create and push version tags.",
        NULL
    },

    /* ── RESET / RECOVERY ────────────────────────────────────────────────── */
    {
        {"recover", "reflog", "lost commit", "undo reset",
         "recover deleted branch", "recover lost commit",
         "lost commit after reset", NULL},
        "git reflog\ngit checkout <hash>   # inspect\ngit reset --hard <hash>   # restore to that point",
        "git reflog records every HEAD movement — use it to recover 'lost' commits or deleted branches.",
        NULL
    },
    {
        {"remove untracked", "delete untracked", "clean working tree",
         "remove untracked files", "delete untracked files", NULL},
        "git clean -n    # dry run — shows what would be deleted\ngit clean -fd   # delete untracked files and directories",
        "Remove untracked files from working tree.",
        "Irreversible — untracked files are gone."
    },

    /* ── CHERRY-PICK ─────────────────────────────────────────────────────── */
    {
        {"cherry-pick", "pick commit", "copy commit", "apply commit", NULL},
        "git cherry-pick <hash>\ngit cherry-pick <hash1>..<hash2>   # range",
        "Apply a specific commit (or range) onto the current branch.",
        NULL
    },

    /* ── SUBMODULES ──────────────────────────────────────────────────────── */
    {
        {"submodule", "nested repo", "dependency repo", NULL},
        "git submodule add <url> <path>\ngit submodule update --init --recursive   # after clone\ngit submodule foreach git pull origin main",
        "Manage nested git repositories.",
        NULL
    },

    /* ── CONFIG ──────────────────────────────────────────────────────────── */
    {
        {"config", "set name", "set email", "git settings",
         "set my git email", "set my git username", "set git name", NULL},
        "git config --global user.name \"Your Name\"\ngit config --global user.email \"you@example.com\"\ngit config --list",
        "View or set git configuration.",
        NULL
    },
    {
        {"alias", "shortcut", "abbreviation", NULL},
        "git config --global alias.st status\ngit config --global alias.lg \"log --oneline --graph --all\"",
        "Create git command aliases.",
        NULL
    },

    /* ── WORKTREE ────────────────────────────────────────────────────────── */
    {
        {"worktree", "two branches at once", "work on two branches",
         "checkout without switching", "multiple working trees", NULL},
        "git worktree add ../project-hotfix hotfix/v2\n"
        "git worktree list\n"
        "git worktree remove ../project-hotfix",
        "Check out a second branch into a separate directory — no stashing, no switching.\n"
        "  Add new:   git worktree add <path> <branch>\n"
        "  List all:  git worktree list\n"
        "  Remove:    git worktree remove <path>",
        NULL
    },

    /* ── SPARSE CHECKOUT ─────────────────────────────────────────────────── */
    {
        {"sparse checkout", "partial clone", "only some files", "subset of repo", NULL},
        "git sparse-checkout init --cone\n"
        "git sparse-checkout set src/ docs/\n"
        "git sparse-checkout disable",
        "Check out only specific directories from a large repo.\n"
        "  Init:     git sparse-checkout init --cone\n"
        "  Set dirs: git sparse-checkout set <dir1> <dir2>\n"
        "  Disable:  git sparse-checkout disable",
        NULL
    },

    /* ── PATCH / FORMAT-PATCH ────────────────────────────────────────────── */
    {
        {"patch", "export commits", "send patch", "format patch", "email patch", NULL},
        "git format-patch -1 HEAD           # last commit as .patch file\n"
        "git format-patch origin/main       # all commits not in main\n"
        "git am <file.patch>                # apply a received patch",
        "Export commits as patch files (useful for email-based contribution workflows).",
        NULL
    },

    /* ── HOOKS ───────────────────────────────────────────────────────────── */
    {
        {"hook", "pre-commit", "post-commit", "git hook",
         "run on commit", "git hooks", "hooks pre commit", NULL},
        "ls .git/hooks/          # list available hooks\ncp .git/hooks/pre-commit.sample .git/hooks/pre-commit\nchmod +x .git/hooks/pre-commit",
        "Hooks are scripts in .git/hooks/ that run automatically on git events.\n"
        "  pre-commit:  runs before a commit (lint, tests)\n"
        "  commit-msg:  validates the commit message\n"
        "  post-merge:  runs after a merge (e.g. npm install)\n"
        "  pre-push:    runs before a push\n"
        "Remove a hook: rm .git/hooks/<hookname>  or  chmod -x .git/hooks/<hookname>",
        NULL
    },

    /* ── LFS ─────────────────────────────────────────────────────────────── */
    {
        {"lfs", "large files", "git lfs", "binary files", "big files", NULL},
        "git lfs install\ngit lfs track \"*.psd\"\ngit add .gitattributes\ngit lfs ls-files",
        "Git LFS stores large binary files outside the repo and replaces them with pointers.\n"
        "  Install:   git lfs install  (once per machine)\n"
        "  Track:     git lfs track \"*.bin\"\n"
        "  Migrate:   git lfs migrate import --include=\"*.psd\" --everything",
        "Requires git-lfs installed: https://git-lfs.com"
    },

    /* ── SHALLOW CLONE ───────────────────────────────────────────────────── */
    {
        {"shallow clone", "partial clone", "clone without history",
         "fast clone", "clone depth", NULL},
        "git clone --depth 1 <url>                    # latest commit only\ngit clone --depth 10 <url>                   # last 10 commits\ngit fetch --unshallow                        # convert back to full clone",
        "Clone only recent history — much faster for large repos.\n"
        "  Single branch: git clone --depth 1 --single-branch --branch main <url>",
        NULL
    },

    /* ── NOTES ───────────────────────────────────────────────────────────── */
    {
        {"notes", "git notes", "annotate commit", "attach note", NULL},
        "git notes add -m \"note text\" <hash>\ngit notes show <hash>\ngit log --show-notes",
        "Attach extra metadata to commits without rewriting history.",
        NULL
    },

    /* ── ARCHIVE ─────────────────────────────────────────────────────────── */
    {
        {"archive", "export repo", "zip repo", "tar repo", "download snapshot", NULL},
        "git archive --format=tar.gz --output=snapshot.tar.gz HEAD\ngit archive --format=zip HEAD > snapshot.zip",
        "Export a clean snapshot of the repo at a given commit — no .git directory included.",
        NULL
    },

    /* ── INIT ────────────────────────────────────────────────────────────── */
    {
        {"init", "create repo", "new repo", "create repo from scratch",
         "initialise repo", "start a repo", NULL},
        "git init\ngit init -b main   # set default branch name",
        "Create a new git repository in the current directory.",
        NULL
    },

    /* ── BUNDLE (offline transfer) ───────────────────────────────────────── */
    {
        {"bundle", "offline transfer", "pack repo for offline",
         "transfer repo without network", "send repo as file", NULL},
        "git bundle create repo.bundle --all\ngit clone repo.bundle ./destination",
        "Pack the entire repo into a single file for transfer without a network connection.\n"
        "  Create:  git bundle create repo.bundle --all\n"
        "  Verify:  git bundle verify repo.bundle\n"
        "  Clone:   git clone repo.bundle ./destination",
        NULL
    },

    /* ── DELETE REMOTE BRANCH ────────────────────────────────────────────── */
    {
        {"delete remote branch", "remove remote branch",
         "push delete branch", "delete branch on remote", NULL},
        "git push origin --delete <branch_name>",
        "Delete a branch on the remote.\n"
        "  Also clean up your local remote-tracking ref:\n"
        "    git fetch --prune",
        NULL
    },

    /* ── REBASE SKIP ─────────────────────────────────────────────────────── */
    {
        {"skip commit rebase", "rebase skip", "skip during rebase",
         "skip a commit during rebase", NULL},
        "git rebase --skip",
        "Skip the current commit during a rebase (discards it) and continue.\n"
        "  Continue instead (after resolving conflict):  git rebase --continue\n"
        "  Abort entirely:                               git rebase --abort",
        NULL
    },

    /* ── APPLY PATCH ─────────────────────────────────────────────────────── */
    {
        {"apply patch", "apply patch file", "import patch",
         "git am", "git apply", "apply a patch file", NULL},
        "git am <patch_file.patch>         # apply formatted patch (preserves author)\ngit apply <patch_file.patch>       # apply diff patch (no commit)",
        "Apply a patch file to the working tree.\n"
        "  git am:    applies .patch files from git format-patch, creates commits\n"
        "  git apply: applies raw diffs, does not create a commit\n"
        "  Check first: git apply --check <file.patch>",
        NULL
    },

    /* ── GPG SIGNED COMMIT ───────────────────────────────────────────────── */
    {
        {"sign commit", "gpg commit", "sign commit with gpg",
         "signed commit", "gpg sign", NULL},
        "git commit -S -m \"<message>\"",
        "Create a GPG-signed commit.\n"
        "  Sign all commits by default:  git config --global commit.gpgSign true\n"
        "  Verify a commit signature:    git verify-commit <hash>\n"
        "  List signing key:             git config --global user.signingKey <keyid>",
        NULL
    },


    /* ── BEGIN GENERATED FROM TLDR ──────────────────────────────────────── */
    /* git abort */
    {
        {"abort a git rebase, merge, or cherry-pick", NULL},
        "git abort",
        "  Abort a Git rebase, merge, or cherry-pick: git abort",
        NULL
    },

    /* git add */
    {
        {"stage a file for a commit", "interactively stage a file", "interactively stage parts of files", "interactively stage parts of a given file", NULL},
        "git add <path/to/file>\ngit add <[-A|--all]>\ngit add .\ngit add <[-u|--update]>",
        "  Stage a file for a commit: git add <path/to/file>\n  Add all files (tracked and untracked): git add <[-A|--all]>\n  Add all files recursively starting from the current folder: git add .\n  Only add already tracked files: git add <[-u|--update]>\n  Add an ignored file: git add <[-f|--force]> <path/to/file>\n  Interactively stage parts of files: git add <[-p|--patch]>",
        "Force pushes overwrite remote history — coordinate with your team."
    },

    /* git alias */
    {
        {"search for an existing alias", NULL},
        "git alias\ngit alias \"<name>\" \"<command>\"\ngit alias ^<name>",
        "  List all aliases: git alias\n  Create a new alias: git alias \"<name>\" \"<command>\"\n  Search for an existing alias: git alias ^<name>",
        NULL
    },

    /* git am */
    {
        {"abort the process of applying a patch file", NULL},
        "git am <path/to/file.patch>\ncurl <[-L|--location]> <https://example.com/file.patch> | git am\ngit am --abort\ngit am --reject <path/to/file.patch>",
        "  Apply and commit changes following a local patch file: git am <path/to/file.patch>\n  Apply and commit changes following a remote patch file: curl <[-L|--location]> <https://example.com/file.patch> | git am\n  Abort the process of applying a patch file: git am --abort\n  Apply as much of a patch file as possible, saving failed hunks to reject files: git am --reject <path/to/file.patch>",
        NULL
    },

    /* git annex */
    {
        {"display help", "initialize a repo with git annex", "synchronize a local repository with a remote", NULL},
        "git annex init\ngit annex add <path/to/file_or_directory>\ngit annex status <path/to/file_or_directory>\ngit annex <remote>",
        "  Initialize a repo with Git annex: git annex init\n  Add a file: git annex add <path/to/file_or_directory>\n  Show the current status of a file or directory: git annex status <path/to/file_or_directory>\n  Synchronize a local repository with a remote: git annex <remote>\n  Get a file or directory: git annex get <path/to/file_or_directory>\n  Display help: git annex help",
        NULL
    },

    /* git annotate */
    {
        {"print only rows that match a `regex`", NULL},
        "git annotate <path/to/file>\ngit annotate <[-e|--show-email]> <path/to/file>\ngit annotate -L :<regexp> <path/to/file>",
        "  Print a file with the author name and commit hash prepended to each line: git annotate <path/to/file>\n  Print a file with the author email and commit hash prepended to each line: git annotate <[-e|--show-email]> <path/to/file>\n  Print only rows that match a `regex`: git annotate -L :<regexp> <path/to/file>",
        NULL
    },

    /* git apply */
    {
        {"print messages about the patched files", "output diffstat for the input and apply the patch", NULL},
        "git apply <[-v|--verbose]> <path/to/file>\ngit apply --index <path/to/file>\ncurl <[-L|--location]> <https://example.com/file.patch> | git apply\ngit apply --stat --apply <path/to/file>",
        "  Print messages about the patched files: git apply <[-v|--verbose]> <path/to/file>\n  Apply and add the patched files to the index: git apply --index <path/to/file>\n  Apply a remote patch file: curl <[-L|--location]> <https://example.com/file.patch> | git apply\n  Output diffstat for the input and apply the patch: git apply --stat --apply <path/to/file>\n  Apply the patch in reverse: git apply <[-R|--reverse]> <path/to/file>\n  Store the patch result in the index without modifying the working tree: git apply --cache <path/to/file>",
        NULL
    },

    /* git archive */
    {
        {"use the contents of a specific directory", "output the zip archive to a specific file", "use the zip format and report progress verbosely", NULL},
        "git archive <[-v|--verbose]> HEAD\ngit archive <[-v|--verbose]> --format zip HEAD\ngit archive <[-v|--verbose]> <[-o|--output]> <path/to/file.zip> HEAD\ngit archive <[-o|--output]> <path/to/file.tar> <branch_name>",
        "  Create a `.tar` archive from the contents of the current `HEAD` and print it to `stdout`: git archive <[-v|--verbose]> HEAD\n  Use the Zip format and report progress verbosely: git archive <[-v|--verbose]> --format zip HEAD\n  Output the Zip archive to a specific file: git archive <[-v|--verbose]> <[-o|--output]> <path/to/file.zip> HEAD\n  Create a `.tar` archive from the contents of the latest commit of a specific branch: git archive <[-o|--output]> <path/to/file.tar> <branch_name>\n  Use the contents of a specific directory: git archive <[-o|--output]> <path/to/file.tar> HEAD:<path/to/directory>\n  Prepend a path to each file to archive it inside a specific directory: git archive <[-o|--output]> <path/to/file.tar> --prefix <path/to/prepend>/ HEAD",
        NULL
    },

    /* git bisect */
    {
        {"display a log of what has been done so far", NULL},
        "git bisect start <bad_commit> <good_commit>\ngit bisect <good|bad>\ngit bisect reset\ngit bisect skip",
        "  Start a bisect session on a commit range bounded by a known buggy commit, and a known clean (typically older) one: git bisect start <bad_commit> <good_commit>\n  For each commit that `git bisect` selects, mark it as \"bad\" or \"good\" after testing it for the issue: git bisect <good|bad>\n  End the bisect session and return to the previous branch: git bisect reset\n  Skip a commit during a bisect (e.g. one that fails the tests due to a different issue): git bisect skip\n  Start a bisect session considering only commits that modify a specific file or directory: git bisect start <bad_commit> <good_commit> -- <path/to/file_or_directory>\n  Automate the bisect process using a test script that `exit`s with 0 for \"good\" and non-zero for \"bad\": git bisect run <path/to/test_script> <optional_script_arguments>",
        NULL
    },

    /* git blame-someone-else */
    {
        {"change the committer and author of a commit", NULL},
        "git blame-someone-else \"<author <someone@example.com>>\" <commit>",
        "  Change the committer and author of a commit: git blame-someone-else \"<author <someone@example.com>>\" <commit>",
        NULL
    },

    /* git blame */
    {
        {"annotate a specific line range of a file", "print the author\'s email instead of their name", "annotate a file ignoring whitespaces and line moves", NULL},
        "git blame <path/to/file>\ngit blame <[-e|--show-email]> <path/to/file>\ngit blame <commit> <path/to/file>\ngit blame <commit>~ <path/to/file>",
        "  Print a file with authorship info (author name and commit hash): git blame <path/to/file>\n  Print the author\'s email instead of their name: git blame <[-e|--show-email]> <path/to/file>\n  Print a file with authorship info as of a specific commit: git blame <commit> <path/to/file>\n  Print a file with authorship info before a specific commit: git blame <commit>~ <path/to/file>\n  Print a file with authorship info starting at a given line: git blame -L <123> <path/to/file>\n  Annotate a specific line range of a file: git blame -L <start_line>,<end_line> <path/to/file>",
        NULL
    },

    /* git browse */
    {
        {"open the first upstream in the default browser", "open a specific upstream in the default browser", NULL},
        "git browse\ngit browse <upstream>",
        "  Open the first upstream in the default browser: git browse\n  Open a specific upstream in the default browser: git browse <upstream>",
        NULL
    },

    /* git bug */
    {
        {"search for bugs by text content", "filter and sort bugs using a query", NULL},
        "git bug user create\ngit bug add\ngit bug push\ngit bug pull",
        "  Create a new identity: git bug user create\n  Create a new bug: git bug add\n  Push a new bug entry to a remote: git bug push\n  Pull for updates: git bug pull\n  List existing bugs: git bug ls\n  Filter and sort bugs using a query: git bug ls \"<status>:<open> <sort>:<edit>\"",
        NULL
    },

    /* git bulk */
    {
        {"register a workspace for bulk operations", "register the current directory as a workspace", NULL},
        "git bulk --addcurrent <workspace_name>\ngit bulk --addworkspace <workspace_name> /<path/to/repository>\ngit bulk --addworkspace <workspace_name> /<path/to/parent_directory> --from <remote_repository_location>\ngit bulk --addworkspace <workspace_name> /<path/to/root_directory> --from /<path/to/file>",
        "  Register the current directory as a workspace: git bulk --addcurrent <workspace_name>\n  Register a workspace for bulk operations: git bulk --addworkspace <workspace_name> /<path/to/repository>\n  Clone a repository inside a specific directory, then register the repository as a workspace: git bulk --addworkspace <workspace_name> /<path/to/parent_directory> --from <remote_repository_location>\n  Clone repositories from a newline-separated list of remote locations, then register them as workspaces: git bulk --addworkspace <workspace_name> /<path/to/root_directory> --from /<path/to/file>\n  List all registered workspaces: git bulk --listall\n  Run a Git command on the repositories of the current workspace: git bulk <command> <command_arguments>",
        NULL
    },

    /* git changelog */
    {
        {"specify `changelog.md` as the output file", "replace contents of current changelog file entirely", NULL},
        "git changelog\ngit changelog <[-l|--list]>\ngit changelog <[-l|--list]> <[-s|--start-tag]> 2.1.0\ngit changelog <[-s|--start-tag]> 0.5.0 <[-f|--final-tag]> 1.0.0",
        "  Update existing file or create a new `History.md` file with the commit messages since the latest Git tag: git changelog\n  List commits from the current version: git changelog <[-l|--list]>\n  List a range of commits from the tag named `2.1.0` to now: git changelog <[-l|--list]> <[-s|--start-tag]> 2.1.0\n  List pretty formatted range of commits between the tag `0.5.0` and the tag `1.0.0`: git changelog <[-s|--start-tag]> 0.5.0 <[-f|--final-tag]> 1.0.0\n  List pretty formatted range of commits between the commit `0b97430` and the tag `1.0.0`: git changelog --start-commit 0b97430 <[-f|--final-tag]> 1.0.0\n  Specify `CHANGELOG.md` as the output file: git changelog CHANGELOG.md",
        NULL
    },

    /* git check-ignore */
    {
        {"use pathnames, one per line, from `stdin`", NULL},
        "git check-ignore <path/to/file_or_directory>\ngit check-ignore <path/to/file_or_directory1 path/to/file_or_directory2 ...>\ngit < <path/to/file_list> check-ignore --stdin\ngit check-ignore --no-index <path/to/file_or_directory1 path/to/file_or_directory2 ...>",
        "  Check whether a file or directory is ignored: git check-ignore <path/to/file_or_directory>\n  Check whether multiple files or directories are ignored: git check-ignore <path/to/file_or_directory1 path/to/file_or_directory2 ...>\n  Use pathnames, one per line, from `stdin`: git < <path/to/file_list> check-ignore --stdin\n  Do not check the index (used to debug why paths were tracked and not ignored): git check-ignore --no-index <path/to/file_or_directory1 path/to/file_or_directory2 ...>\n  Include details about the matching pattern for each path: git check-ignore <[-v|--verbose]> <path/to/file_or_directory1 path/to/file_or_directory2 ...>",
        NULL
    },

    /* git check-ref-format */
    {
        {"normalize a refname", "print the name of the last branch checked out", NULL},
        "git check-ref-format <refs/head/refname>\ngit check-ref-format --branch @{-1}\ngit check-ref-format --normalize <refs/head/refname>",
        "  Check the format of the specified reference name: git check-ref-format <refs/head/refname>\n  Print the name of the last branch checked out: git check-ref-format --branch @{-1}\n  Normalize a refname: git check-ref-format --normalize <refs/head/refname>",
        NULL
    },

    /* git checkout */
    {
        {"discard unstaged changes to a given file", NULL},
        "git checkout -b <branch_name>\ngit checkout -b <branch_name> <reference>\ngit checkout <branch_name>\ngit checkout -",
        "  Create and switch to a new branch: git checkout -b <branch_name>\n  Create and switch to a new branch based on a specific reference (branch, remote/branch, tag are examples of valid references): git checkout -b <branch_name> <reference>\n  Switch to an existing local branch: git checkout <branch_name>\n  Switch to the previously checked out branch: git checkout -\n  Switch to an existing remote branch: git checkout <[-t|--track]> <remote_name>/<branch_name>\n  Discard all unstaged changes in the current directory (see `git reset` for more undo-like commands): git checkout .",
        NULL
    },

    /* git cherry */
    {
        {"limit commits to those within a given limit", "specify a different upstream and topic branch", NULL},
        "git cherry <[-v|--verbose]>\ngit cherry <origin> <topic>\ngit cherry <origin> <topic> <base>",
        "  Show commits (and their messages) with equivalent commits upstream: git cherry <[-v|--verbose]>\n  Specify a different upstream and topic branch: git cherry <origin> <topic>\n  Limit commits to those within a given limit: git cherry <origin> <topic> <base>",
        NULL
    },

    /* git clean */
    {
        {"interactively delete untracked files", "immediately force deletion of all untracked files", NULL},
        "git clean <[-i|--interactive]>\ngit clean <[-n|--dry-run]>\ngit clean <[-f|--force]>\ngit clean <[-f|--force]> -d",
        "  Interactively delete untracked files: git clean <[-i|--interactive]>\n  Show which files would be deleted without actually deleting them: git clean <[-n|--dry-run]>\n  Immediately force deletion of all untracked files: git clean <[-f|--force]>\n  Delete untracked [d]irectories: git clean <[-f|--force]> -d\n  Delete only untracked files matching specific paths or glob patterns: git clean <[-f|--force]> -- <path/to/directory> \'<*.ext>\'\n  Delete untracked files except those matching the given patterns: git clean <[-f|--force]> <[-e|--exclude]> \'<*.ext>\' <[-e|--exclude]> <path/to/directory>/",
        "Force pushes overwrite remote history — coordinate with your team."
    },

    /* git coauthor */
    {
        {"insert an additional author to the last git commit", NULL},
        "git coauthor <name> <name@example.com>",
        "  Insert an additional author to the last Git commit: git coauthor <name> <name@example.com>",
        NULL
    },

    /* git cola */
    {
        {"start the gui", "start the gui in amend mode", "open the git repository at mentioned path", NULL},
        "git cola\ngit cola --amend\ngit cola --prompt\ngit cola <[-r|--repo]> <path/to/git-repository>",
        "  Start the GUI: git cola\n  Start the GUI in amend mode: git cola --amend\n  Prompt for a Git repository. Defaults to the current directory: git cola --prompt\n  Open the Git repository at mentioned path: git cola <[-r|--repo]> <path/to/git-repository>\n  Apply the path filter to the status widget: git cola <[-s|--status-filter]> <filter>",
        NULL
    },

    /* git column */
    {
        {"format `stdin` as multiple columns", NULL},
        "ls | git column --mode=<column>\nls | git column --mode=column --width=<100>\nls | git column --mode=column --padding=<30>",
        "  Format `stdin` as multiple columns: ls | git column --mode=<column>\n  Format `stdin` as multiple columns with a maximum width of `100`: ls | git column --mode=column --width=<100>\n  Format `stdin` as multiple columns with a maximum padding of `30`: ls | git column --mode=column --padding=<30>",
        NULL
    },

    /* git commits-since */
    {
        {"display commits since yesterday", "display commits since last week", "display commits since last month", "display commits since yesterday 2pm", NULL},
        "git commits-since <yesterday>\ngit commits-since <last week>\ngit commits-since <last month>\ngit commits-since <yesterday 2pm>",
        "  Display commits since yesterday: git commits-since <yesterday>\n  Display commits since last week: git commits-since <last week>\n  Display commits since last month: git commits-since <last month>\n  Display commits since yesterday 2pm: git commits-since <yesterday 2pm>",
        NULL
    },

    /* git config */
    {
        {"use an alias", NULL},
        "git config --global <user.name|user.email> \"<Your Name|email@example.com>\"\ngit config <[-l|--list]> --<local|global|system> --show-origin\ngit config --global <alias.unstage> \"reset HEAD --\"\ngit config <alias.unstage>",
        "  Globally set your name or email (this information is required to commit to a repository and will be included in all commits): git config --global <user.name|user.email> \"<Your Name|email@example.com>\"\n  List local, global, or system configuration entries and show their file location: git config <[-l|--list]> --<local|global|system> --show-origin\n  Set the global value of a given configuration entry (in this case an alias): git config --global <alias.unstage> \"reset HEAD --\"\n  Get the value of a given configuration entry: git config <alias.unstage>\n  Use an alias: git <unstage>\n  Revert a global configuration entry to its default value: git config --global --unset <alias.unstage>",
        NULL
    },

    /* git count-objects */
    {
        {"display more verbose information", "count all objects and display the total disk usage", NULL},
        "git count-objects\ngit count-objects <[-H|--human-readable]>\ngit count-objects <[-v|--verbose]>\ngit count-objects <[-H|--human-readable]> <[-v|--verbose]>",
        "  Count all objects and display the total disk usage: git count-objects\n  Display a count of all objects and their total disk usage, displaying sizes in human-readable units: git count-objects <[-H|--human-readable]>\n  Display more verbose information: git count-objects <[-v|--verbose]>\n  Display more verbose information, displaying sizes in human-readable units: git count-objects <[-H|--human-readable]> <[-v|--verbose]>",
        NULL
    },

    /* git count */
    {
        {"print the total number of commits", NULL},
        "git count\ngit count --all",
        "  Print the total number of commits: git count\n  Print the number of commits per contributor and the total number of commits: git count --all",
        NULL
    },

    /* git credential-cache */
    {
        {"store git credentials for a specific amount of time", NULL},
        "git config credential.helper \'cache --timeout=<time_in_seconds>\'",
        "  Store Git credentials for a specific amount of time: git config credential.helper \'cache --timeout=<time_in_seconds>\'",
        NULL
    },

    /* git credential-store */
    {
        {"store git credentials in a specific file", NULL},
        "git config credential.helper \'store --file=<path/to/file>\'",
        "  Store Git credentials in a specific file: git config credential.helper \'store --file=<path/to/file>\'",
        NULL
    },

    /* git describe */
    {
        {"describe a git tag", "generate a name with the tag reference path", NULL},
        "git describe\ngit describe --abbrev=<4>\ngit describe --all\ngit describe <v1.0.0>",
        "  Create a unique name for the current commit (the name contains the most recent annotated tag, the number of additional commits, and the abbreviated commit hash): git describe\n  Create a name with 4 digits for the abbreviated commit hash: git describe --abbrev=<4>\n  Generate a name with the tag reference path: git describe --all\n  Describe a Git tag: git describe <v1.0.0>\n  Create a name for the last commit of a given branch: git describe <branch_name>",
        NULL
    },

    /* git diff-files */
    {
        {"compare all changed files", "compare only specified files", "output a summary of extended header information", NULL},
        "git diff-files\ngit diff-files <path/to/file>\ngit diff-files --name-only\ngit diff-files --summary",
        "  Compare all changed files: git diff-files\n  Compare only specified files: git diff-files <path/to/file>\n  Show only the names of changed files: git diff-files --name-only\n  Output a summary of extended header information: git diff-files --summary",
        NULL
    },

    /* git diff-index */
    {
        {"compare the working directory with a specific commit", NULL},
        "git diff-index <commit>\ngit diff-index <commit> <path/to/file_or_directory>\ngit diff-index --cached <commit>\ngit diff-index --quiet <commit>",
        "  Compare the working directory with a specific commit: git diff-index <commit>\n  Compare a specific file or directory in working directory with a commit: git diff-index <commit> <path/to/file_or_directory>\n  Compare the working directory with the index (staging area) to check for staged changes: git diff-index --cached <commit>\n  Suppress output and return an exit status to check for differences: git diff-index --quiet <commit>",
        NULL
    },

    /* git diff-tree */
    {
        {"compare two tree objects", "display changes in patch format", "filter changes by a specific path", NULL},
        "git diff-tree <tree-ish1> <tree-ish2>\ngit diff-tree -r <commit1> <commit2>\ngit diff-tree <[-p|--patch]> <tree-ish1> <tree-ish2>\ngit diff-tree <tree-ish1> <tree-ish2> -- <path/to/file_or_directory>",
        "  Compare two tree objects: git diff-tree <tree-ish1> <tree-ish2>\n  Show changes between two specific commits: git diff-tree -r <commit1> <commit2>\n  Display changes in patch format: git diff-tree <[-p|--patch]> <tree-ish1> <tree-ish2>\n  Filter changes by a specific path: git diff-tree <tree-ish1> <tree-ish2> -- <path/to/file_or_directory>",
        NULL
    },

    /* git diff */
    {
        {"compare a single file between two branches or commits", NULL},
        "git diff\ngit diff HEAD\ngit diff --staged\ngit diff \'HEAD@<{3 months|weeks|days|hours|seconds ago>}\'",
        "  Show unstaged changes: git diff\n  Show all uncommitted changes (including staged ones): git diff HEAD\n  Show only staged (added, but not yet committed) changes: git diff --staged\n  Show changes from all commits since a given date/time (a date expression, e.g. \"1 week 2 days\" or an ISO date): git diff \'HEAD@<{3 months|weeks|days|hours|seconds ago>}\'\n  Show diff statistics, like files changed, histogram, and total line insertions/deletions: git diff --stat <commit>\n  Output a summary of file creations, renames, and mode changes since a given commit: git diff --summary <commit>",
        NULL
    },

    /* git difftool */
    {
        {"use the default diff tool to show staged changes", NULL},
        "git difftool --tool-help\ngit config --global diff.tool \"meld\"\ngit difftool --staged\ngit difftool <[-t|--tool]> <opendiff> <commit>",
        "  List available diff tools: git difftool --tool-help\n  Set the default diff tool to Meld: git config --global diff.tool \"meld\"\n  Use the default diff tool to show staged changes: git difftool --staged\n  Use a specific tool to show changes since a given commit: git difftool <[-t|--tool]> <opendiff> <commit>",
        NULL
    },

    /* git extras */
    {
        {"display help", "display version", "install or upgrade `git-extras` commands", NULL},
        "git extras update\ngit extras <[-h|--help]>\ngit extras <[-v|--version]>",
        "  Install or upgrade `git-extras` commands: git extras update\n  Display help: git extras <[-h|--help]>\n  Display version: git extras <[-v|--version]>",
        NULL
    },

    /* git fame */
    {
        {"ignore whitespace changes", "detect intra-file line moves and copies", "detect inter-file line moves and copies", "display contributions per file extension", "display contributions in the specified format", "calculate contributions made after the specified date", "calculate contributions for the current git repository", NULL},
        "git fame\ngit fame --excl \"<regex>\"\ngit fame --since \"<3 weeks ago|2021-05-13>\"\ngit fame --format <pipe|yaml|json|csv|tsv>",
        "  Calculate contributions for the current Git repository: git fame\n  Exclude files/directories that match the specified `regex`: git fame --excl \"<regex>\"\n  Calculate contributions made after the specified date: git fame --since \"<3 weeks ago|2021-05-13>\"\n  Display contributions in the specified format: git fame --format <pipe|yaml|json|csv|tsv>\n  Display contributions per file extension: git fame <[-t|--bytype]>\n  Ignore whitespace changes: git fame <[-w|--ignore-whitespace]>",
        NULL
    },

    /* git fast-export */
    {
        {"export a specific branch only", "export the entire repository to a file", "export only a specific subdirectory\'s history", "export the entire git repository history to `stdout`", NULL},
        "git fast-export --all\ngit fast-export --all > <path/to/file>\ngit fast-export <main>\ngit fast-export --progress <n> --all > <path/to/file>",
        "  Export the entire Git repository history to `stdout`: git fast-export --all\n  Export the entire repository to a file: git fast-export --all > <path/to/file>\n  Export a specific branch only: git fast-export <main>\n  Export with `progress` statements every `n` objects (for showing progress during `git fast-import`): git fast-export --progress <n> --all > <path/to/file>\n  Export only a specific subdirectory\'s history: git fast-export --all -- <path/to/directory> > <path/to/file>",
        NULL
    },

    /* git fetch */
    {
        {"deepen current shallow branch by 2 commits", "also fetch tags from the remote upstream repository", NULL},
        "git fetch\ngit fetch <remote_name>\ngit fetch --all\ngit fetch <[-t|--tags]>",
        "  Fetch the latest changes from the default remote upstream repository (if set): git fetch\n  Fetch new branches from a specific remote upstream repository: git fetch <remote_name>\n  Fetch the latest changes from all remote upstream repositories: git fetch --all\n  Also fetch tags from the remote upstream repository: git fetch <[-t|--tags]>\n  Delete local references to remote branches that have been deleted upstream: git fetch <[-p|--prune]>\n  Deepen current shallow branch by 2 commits: git fetch --deepen 2",
        NULL
    },

    /* git filter-branch */
    {
        {"update author email", NULL},
        "git filter-branch --tree-filter \'rm <[-f|--force]> <file>\' HEAD\ngit filter-branch --env-filter \'GIT_AUTHOR_EMAIL=<new_email>\' HEAD\ngit filter-branch --tree-filter \'rm <[-rf|--recursive --force]> <folder>\' HEAD",
        "  Remove a file from all commits: git filter-branch --tree-filter \'rm <[-f|--force]> <file>\' HEAD\n  Update author email: git filter-branch --env-filter \'GIT_AUTHOR_EMAIL=<new_email>\' HEAD\n  Delete a folder from history: git filter-branch --tree-filter \'rm <[-rf|--recursive --force]> <folder>\' HEAD",
        "Rewrites entire history — back up your repo first."
    },

    /* git filter-repo */
    {
        {"replace a sensitive string in all files", "extract a single folder, keeping history", "move everything from sub-folder one level up", NULL},
        "git filter-repo --replace-text <(echo \'<find>==><replacement>\')\ngit filter-repo --path <path/to/folder>\ngit filter-repo --path <path/to/folder> --invert-paths\ngit filter-repo --path-rename <path/to/folder>/:",
        "  Replace a sensitive string in all files: git filter-repo --replace-text <(echo \'<find>==><replacement>\')\n  Extract a single folder, keeping history: git filter-repo --path <path/to/folder>\n  Remove a single folder, keeping history: git filter-repo --path <path/to/folder> --invert-paths\n  Move everything from sub-folder one level up: git filter-repo --path-rename <path/to/folder>/:",
        NULL
    },

    /* git flow */
    {
        {"publish a feature to the remote server", "initialize it inside an existing git repository", NULL},
        "git flow init\ngit flow feature start <feature>\ngit flow feature finish <feature>\ngit flow feature publish <feature>",
        "  Initialize it inside an existing Git repository: git flow init\n  Start developing on a feature branch based on `develop`: git flow feature start <feature>\n  Finish development on a feature branch, merging it into the `develop` branch and deleting it: git flow feature finish <feature>\n  Publish a feature to the remote server: git flow feature publish <feature>\n  Get a feature published by another user: git flow feature pull origin <feature>",
        NULL
    },

    /* git for-each-ref */
    {
        {"sort refs by committer date (oldest first)", "limit output to a specified number of refs", "sort refs by committer date (most recent first)", NULL},
        "git for-each-ref\ngit for-each-ref refs/heads/\ngit for-each-ref refs/tags/\ngit for-each-ref --merged HEAD refs/heads/",
        "  List all refs (branches and tags): git for-each-ref\n  List only branches: git for-each-ref refs/heads/\n  List only tags: git for-each-ref refs/tags/\n  Show branches merged into `HEAD`: git for-each-ref --merged HEAD refs/heads/\n  List short names of all refs: git for-each-ref --format \"%(refname:short)\"\n  Sort refs by committer date (most recent first): git for-each-ref --sort -committerdate",
        NULL
    },

    /* git fork */
    {
        {"fork and clone a github repository by its url", "fork and clone a github repository by its slug", NULL},
        "git fork <https://github.com/tldr-pages/tldr>\ngit fork <tldr-pages/tldr>",
        "  Fork and clone a GitHub repository by its URL: git fork <https://github.com/tldr-pages/tldr>\n  Fork and clone a GitHub repository by its slug: git fork <tldr-pages/tldr>",
        NULL
    },

    /* git format-patch */
    {
        {"write a `.patch` file for the `n` latest commits", NULL},
        "git format-patch <origin>\ngit format-patch <revision_1>..<revision_2>\ngit format-patch -<n>",
        "  Create an auto-named `.patch` file for all the unpushed commits: git format-patch <origin>\n  Write a `.patch` file for all the commits between 2 revisions to `stdout`: git format-patch <revision_1>..<revision_2>\n  Write a `.patch` file for the `n` latest commits: git format-patch -<n>",
        NULL
    },

    /* git gc */
    {
        {"display help", "suppress all output", "optimise the repository", "aggressively optimise, takes more time", "do not prune loose objects (prunes by default)", NULL},
        "git gc\ngit gc --aggressive\ngit gc --no-prune\ngit gc --quiet",
        "  Optimise the repository: git gc\n  Aggressively optimise, takes more time: git gc --aggressive\n  Do not prune loose objects (prunes by default): git gc --no-prune\n  Suppress all output: git gc --quiet\n  Display help: git gc --help",
        NULL
    },

    /* git grep */
    {
        {"search for a string, including submodules", "search for a string at a specific point in history", "search for a string in files in the current `head`", NULL},
        "git grep \"<search_string>\"\ngit grep \"<search_string>\" -- \"<*.ext>\"\ngit grep --recurse-submodules \"<search_string>\"\ngit grep \"<search_string>\" <HEAD~2>",
        "  Search for a string in files in the current `HEAD`: git grep \"<search_string>\"\n  Search for a string in files matching a glob pattern in the current `HEAD`: git grep \"<search_string>\" -- \"<*.ext>\"\n  Search for a string, including submodules: git grep --recurse-submodules \"<search_string>\"\n  Search for a string at a specific point in history: git grep \"<search_string>\" <HEAD~2>\n  Search for a string across all branches and all of history: git grep \"<search_string>\" $(git rev-list --all)",
        NULL
    },

    /* git gui */
    {
        {"launch the gui", "open `git gui citool` in a read-only mode", "open `git gui blame` in a specific revision", "open `git gui citool` in the \"amend last commit\" mode", NULL},
        "git gui\ngit gui blame <path/to/file>\ngit gui blame <revision> <path/to/file>\ngit gui blame --line=<line> <path/to/file>",
        "  Launch the GUI: git gui\n  Show a specific file with author name and commit hash on each line: git gui blame <path/to/file>\n  Open `git gui blame` in a specific revision: git gui blame <revision> <path/to/file>\n  Open `git gui blame` and scroll the view to center on a specific line: git gui blame --line=<line> <path/to/file>\n  Open a window to make one commit and return to the shell when it is complete: git gui citool\n  Open `git gui citool` in the \"Amend Last Commit\" mode: git gui citool --amend",
        NULL
    },

    /* git guilt */
    {
        {"find blame delta over the last three weeks", "calculate the change in blame between two revisions", "ignore whitespace only changes when attributing blame", NULL},
        "git guilt\ngit guilt <first_revision> <last_revision>\ngit guilt <[-e|--email]>\ngit guilt <[-w|--ignore-whitespace]>",
        "  Show total blame count: git guilt\n  Calculate the change in blame between two revisions: git guilt <first_revision> <last_revision>\n  Show author emails instead of names: git guilt <[-e|--email]>\n  Ignore whitespace only changes when attributing blame: git guilt <[-w|--ignore-whitespace]>\n  Find blame delta over the last three weeks: git guilt \'git log --until \"3 weeks ago\" --format \"%H\" <[-n|--max-count]> 1\'\n  Find blame delta over the last three weeks (Git 1.8.5+): git guilt @{3.weeks.ago}",
        NULL
    },

    /* git hash-object */
    {
        {"compute the object id from `stdin`", "compute the object id without storing it", "compute the object id specifying the object type", "compute the object id and store it in the git database", NULL},
        "git hash-object <path/to/file>\ngit hash-object -w <path/to/file>\ngit hash-object -t <blob|commit|tag|tree> <path/to/file>\ncat <path/to/file> | git hash-object --stdin",
        "  Compute the object ID without storing it: git hash-object <path/to/file>\n  Compute the object ID and store it in the Git database: git hash-object -w <path/to/file>\n  Compute the object ID specifying the object type: git hash-object -t <blob|commit|tag|tree> <path/to/file>\n  Compute the object ID from `stdin`: cat <path/to/file> | git hash-object --stdin",
        NULL
    },

    /* git help */
    {
        {"display help about a specific git subcommand", "display a list of all available git subcommands", NULL},
        "git help <subcommand>\ngit help <[-w|--web]> <subcommand>\ngit help <[-a|--all]>\ngit help <[-g|--guides]>",
        "  Display help about a specific Git subcommand: git help <subcommand>\n  Display help about a specific Git subcommand in a web browser: git help <[-w|--web]> <subcommand>\n  Display a list of all available Git subcommands: git help <[-a|--all]>\n  List the available guides: git help <[-g|--guides]>\n  List all possible configuration variables: git help <[-c|--config]>",
        NULL
    },

    /* git ignore-io */
    {
        {"generate a `.gitignore` template", NULL},
        "git ignore-io list\ngit ignore-io <item_a,item_b,...>",
        "  List available templates: git ignore-io list\n  Generate a `.gitignore` template: git ignore-io <item_a,item_b,...>",
        NULL
    },

    /* git init */
    {
        {"initialize a new local repository", NULL},
        "git init\ngit init <[-b|--initial-branch]> <branch_name>\ngit init --object-format sha256\ngit init --bare",
        "  Initialize a new local repository: git init\n  Initialize a repository with the specified name for the initial branch: git init <[-b|--initial-branch]> <branch_name>\n  Initialize a repository using SHA256 for object hashes (requires Git version 2.29+): git init --object-format sha256\n  Initialize a barebones repository, suitable for use as a remote over SSH: git init --bare",
        NULL
    },

    /* git instaweb */
    {
        {"listen only on localhost", "listen on a specific port", "use a specified http daemon", "also auto-launch a web browser", "stop the currently running gitweb server", "restart the currently running gitweb server", "launch a gitweb server for the current git repository", NULL},
        "git instaweb --start\ngit instaweb --start <[-l|--local]>\ngit instaweb --start <[-p|--port]> <1234>\ngit instaweb --start <[-d|--httpd]> <lighttpd|apache2|mongoose|plackup|webrick>",
        "  Launch a GitWeb server for the current Git repository: git instaweb --start\n  Listen only on localhost: git instaweb --start <[-l|--local]>\n  Listen on a specific port: git instaweb --start <[-p|--port]> <1234>\n  Use a specified HTTP daemon: git instaweb --start <[-d|--httpd]> <lighttpd|apache2|mongoose|plackup|webrick>\n  Also auto-launch a web browser: git instaweb --start <[-b|--browser]>\n  Stop the currently running GitWeb server: git instaweb --stop",
        NULL
    },

    /* git-lfs-transfer */
    {
        {"git lfs-transfer", "upload large files tracked by git lfs to a repository", NULL},
        "git-lfs-transfer <path/to/repo.git> upload\ngit-lfs-transfer <path/to/repo.git> download",
        "  Upload large files tracked by Git LFS to a repository: git-lfs-transfer <path/to/repo.git> upload\n  Download large files tracked by Git LFS from a repository: git-lfs-transfer <path/to/repo.git> download",
        NULL
    },

    /* git lfs */
    {
        {"initialize git lfs", "track files that match a glob", "replace pointer files with actual git lfs objects", NULL},
        "git lfs install\ngit lfs track \'<*.bin>\'\ngit config <[-f|--file]> .lfsconfig lfs.url <lfs_endpoint_url>\ngit lfs track",
        "  Initialize Git LFS: git lfs install\n  Track files that match a glob: git lfs track \'<*.bin>\'\n  Change the Git LFS endpoint URL (useful if the LFS server is separate from the Git server): git config <[-f|--file]> .lfsconfig lfs.url <lfs_endpoint_url>\n  List tracked patterns: git lfs track\n  List tracked files that have been committed: git lfs ls-files\n  Push all Git LFS objects to the remote server (useful if errors are encountered): git lfs push --all <remote_name> <branch_name>",
        NULL
    },

    /* git lock */
    {
        {"disable the ability to commit changes of a local file", NULL},
        "git lock <path/to/file>",
        "  Disable the ability to commit changes of a local file: git lock <path/to/file>",
        NULL
    },

    /* git magic */
    {
        {"[e]dit the commit [m]essage before committing", NULL},
        "git magic\ngit magic -a\ngit magic -m \"<custom_commit_message>\"\ngit magic -em \"<custom_commit_message>\"",
        "  Commit changes with a generated message: git magic\n  [a]dd untracked files and commit changes with a generated message: git magic -a\n  Commit changes with a custom [m]essage: git magic -m \"<custom_commit_message>\"\n  [e]dit the commit [m]essage before committing: git magic -em \"<custom_commit_message>\"\n  Commit changes and [p]ush to remote: git magic -p\n  Commit changes with a [f]orce [p]ush to remote: git magic -fp",
        NULL
    },

    /* git mailinfo */
    {
        {"extract but remove leading and trailing whitespace", NULL},
        "git mailinfo <message|patch>\ngit mailinfo -k <message|patch>\ngit mailinfo --scissors <message|patch>",
        "  Extract the patch and author data from an email message: git mailinfo <message|patch>\n  Extract but remove leading and trailing whitespace: git mailinfo -k <message|patch>\n  Remove everything from the body before a scissors line (e.g. \"-->* --\") and retrieve the message or patch: git mailinfo --scissors <message|patch>",
        NULL
    },

    /* git merge-base */
    {
        {"print the best common ancestor of two commits", "print all best common ancestors of two commits", NULL},
        "git merge-base <commit_1> <commit_2>\ngit merge-base <[-a|--all]> <commit_1> <commit_2>\ngit merge-base --is-ancestor <ancestor_commit> <commit>",
        "  Print the best common ancestor of two commits: git merge-base <commit_1> <commit_2>\n  Print all best common ancestors of two commits: git merge-base <[-a|--all]> <commit_1> <commit_2>\n  Check if a commit is an ancestor of a specific commit: git merge-base --is-ancestor <ancestor_commit> <commit>",
        NULL
    },

    /* git merge-index */
    {
        {"inspect merge inputs for a file using `cat`", "quietly merge all files with a custom program", NULL},
        "git merge-index git-merge-one-file -a\ngit merge-index git-merge-one-file -- <path/to/file>\ngit merge-index -o git-merge-one-file -- <path/to/file1 path/to/file2 ...>\ngit merge-index -q <merge-program> -a",
        "  Merge all files needing resolution using the standard helper: git merge-index git-merge-one-file -a\n  Merge a specific file: git merge-index git-merge-one-file -- <path/to/file>\n  Merge multiple files, continuing on failures: git merge-index -o git-merge-one-file -- <path/to/file1 path/to/file2 ...>\n  Quietly merge all files with a custom program: git merge-index -q <merge-program> -a\n  Inspect merge inputs for a file using `cat`: git merge-index cat -- <path>",
        NULL
    },

    /* git merge-one-file */
    {
        {"handle a binary file merge", "use as a helper in merge-index for a file", "resolve a simple merge conflict for a file", NULL},
        "git merge-one-file <path/to/file>\ngit merge-index git-merge-one-file <path/to/file>\ngit merge-one-file -p <path/to/file>\ngit read-tree -m <branch1> <branch2> && git merge-index git-merge-one-file <path/to/file>",
        "  Resolve a simple merge conflict for a file: git merge-one-file <path/to/file>\n  Use as a helper in merge-index for a file: git merge-index git-merge-one-file <path/to/file>\n  Handle a binary file merge: git merge-one-file -p <path/to/file>\n  Apply after read-tree in a scripted merge: git read-tree -m <branch1> <branch2> && git merge-index git-merge-one-file <path/to/file>",
        NULL
    },

    /* git merge-tree */
    {
        {"perform a merge and write the resulting tree", NULL},
        "git merge-tree <branch1> <branch2>\ngit merge-tree --write-tree <branch1> <branch2>",
        "  Show the result of a merge between two branches: git merge-tree <branch1> <branch2>\n  Perform a merge and write the resulting tree: git merge-tree --write-tree <branch1> <branch2>",
        NULL
    },

    /* git merge */
    {
        {"edit the merge message", "abort a merge in case of conflicts", NULL},
        "git merge <branch_name>\ngit merge <[-e|--edit]> <branch_name>\ngit merge --no-ff <branch_name>\ngit merge --squash <branch_name>",
        "  Merge a branch into your current branch: git merge <branch_name>\n  Edit the merge message: git merge <[-e|--edit]> <branch_name>\n  Merge a branch and create a merge commit: git merge --no-ff <branch_name>\n  Copy the state of a branch into the working tree and stage it (Note: Use `git commit` to create the actual commit): git merge --squash <branch_name>\n  Abort a merge in case of conflicts: git merge --abort\n  Merge using a specific strategy: git merge <[-s|--strategy]> <strategy> <[-X|--strategy-option]> <strategy_option> <branch_name>",
        NULL
    },

    /* git mergetool */
    {
        {"launch the merge tool identified by a name", "launch the default merge tool to resolve conflicts", "don\'t prompt before each invocation of the merge tool", NULL},
        "git mergetool\ngit mergetool --tool-help\ngit mergetool <[-t|--tool]> <tool_name>\ngit mergetool <[-y|--no-prompt]>",
        "  Launch the default merge tool to resolve conflicts: git mergetool\n  List valid merge tools: git mergetool --tool-help\n  Launch the merge tool identified by a name: git mergetool <[-t|--tool]> <tool_name>\n  Don\'t prompt before each invocation of the merge tool: git mergetool <[-y|--no-prompt]>\n  Explicitly use the GUI merge tool (see the `merge.guitool` configuration variable): git mergetool <[-g|--gui]>\n  Explicitly use the regular merge tool (see the `merge.tool` configuration variable): git mergetool --no-gui",
        NULL
    },

    /* git mktree */
    {
        {"allow missing objects", "allow the creation of multiple tree objects", NULL},
        "git mktree\ngit mktree --missing\ngit mktree -z\ngit mktree --batch",
        "  Build a tree object and verify that each tree entry\'s hash identifies an existing object: git mktree\n  Allow missing objects: git mktree --missing\n  Read the NUL ([z]ero character) terminated output of the tree object (`git ls-tree -z`): git mktree -z\n  Allow the creation of multiple tree objects: git mktree --batch\n  Sort and build a tree from `stdin` (non-recursive `git ls-tree` output format is required): git < <path/to/tree.txt> mktree",
        NULL
    },

    /* git name-rev */
    {
        {"read commit ids from `stdin`", "restrict names to branch refs", "enumerate all matching ref names", "use only tags to name the commit", NULL},
        "git name-rev HEAD\ngit name-rev --name-only HEAD\ngit name-rev --all\ngit name-rev --tags HEAD",
        "  Show the name for `HEAD`: git name-rev HEAD\n  Show only the name: git name-rev --name-only HEAD\n  Enumerate all matching ref names: git name-rev --all\n  Use only tags to name the commit: git name-rev --tags HEAD\n  Exit with a non-zero status code instead of printing `undefined` for unknown commits: git name-rev --no-undefined <commit-ish>\n  Show names for multiple commits: git name-rev HEAD~1 HEAD~2 main",
        NULL
    },

    /* git notes */
    {
        {"copy a note from one object to another", "edit an existing note (defaults to `head`)", NULL},
        "git notes list\ngit notes list [<object>]\ngit notes show [<object>]\ngit notes append <object>",
        "  List all notes and the objects they are attached to: git notes list\n  List all notes attached to a given object (defaults to `HEAD`): git notes list [<object>]\n  Show the notes attached to a given object (defaults to `HEAD`): git notes show [<object>]\n  Append a note to a specified object (opens the default text editor): git notes append <object>\n  Append a note to a specified object, specifying the message: git notes append --message=\"<message_text>\"\n  Edit an existing note (defaults to `HEAD`): git notes edit [<object>]",
        NULL
    },

    /* git obliterate */
    {
        {"erase the existence of specific files", NULL},
        "git obliterate <file_1 file_2 ...>\ngit obliterate <file_1 file_2 ...> -- <commit_hash_1>..<commit_hash_2>",
        "  Erase the existence of specific files: git obliterate <file_1 file_2 ...>\n  Erase the existence of specific files between 2 commits: git obliterate <file_1 file_2 ...> -- <commit_hash_1>..<commit_hash_2>",
        NULL
    },

    /* git p4 */
    {
        {"submit git changes back to perforce", NULL},
        "git p4 clone <path/to/p4_depot>\ngit p4 sync <path/to/p4_depot>\ngit p4 rebase\ngit p4 submit",
        "  Clone a Perforce depot into a new Git repository: git p4 clone <path/to/p4_depot>\n  Sync changes from Perforce into the current Git repository: git p4 sync <path/to/p4_depot>\n  Rebase local commits on top of the latest Perforce changes: git p4 rebase\n  Submit Git changes back to Perforce: git p4 submit\n  Clone the full Perforce history instead of only the latest changelist: git p4 clone <path/to/p4_depot>@all",
        NULL
    },

    /* git prune */
    {
        {"prune unreachable objects while showing progress", NULL},
        "git prune <[-n|--dry-run]>\ngit prune <[-v|--verbose]>\ngit prune --progress",
        "  Report what would be removed by Git prune without removing it: git prune <[-n|--dry-run]>\n  Prune unreachable objects and display what has been pruned to `stdout`: git prune <[-v|--verbose]>\n  Prune unreachable objects while showing progress: git prune --progress",
        NULL
    },

    /* git psykorebase */
    {
        {"specify the branch to rebase", "continue after conflicts have been handled", NULL},
        "git psykorebase <upstream_branch>\ngit psykorebase --continue\ngit psykorebase <upstream_branch> <target_branch>",
        "  Rebase the current branch on top of another using a merge commit and only one conflict handling: git psykorebase <upstream_branch>\n  Continue after conflicts have been handled: git psykorebase --continue\n  Specify the branch to rebase: git psykorebase <upstream_branch> <target_branch>",
        NULL
    },

    /* git push */
    {
        {"publish tags that aren\'t yet in the remote repository", NULL},
        "git push\ngit push <remote_name> <local_branch>\ngit push <[-u|--set-upstream]> <remote_name> <local_branch>\ngit push <remote_name> <local_branch>:<remote_branch>",
        "  Send local changes in the current branch to its default remote counterpart: git push\n  Send changes from a specific local branch to its remote counterpart: git push <remote_name> <local_branch>\n  Send changes from a specific local branch to its remote counterpart, and set the remote one as the default push/pull target of the local one: git push <[-u|--set-upstream]> <remote_name> <local_branch>\n  Send changes from a specific local branch to a specific remote branch: git push <remote_name> <local_branch>:<remote_branch>\n  Send changes on all local branches to their counterparts in a given remote repository: git push --all <remote_name>\n  Delete a branch in a remote repository: git push <remote_name> <[-d|--delete]> <remote_branch>",
        NULL
    },

    /* git rebase-patch */
    {
        {"find the commit the patch applies to and do a rebase", NULL},
        "git rebase-patch <patch_file>",
        "  Find the commit the patch applies to and do a rebase: git rebase-patch <patch_file>",
        NULL
    },

    /* git remote */
    {
        {"rename a remote", NULL},
        "git remote <[-v|--verbose]>\ngit remote show <remote_name>\ngit remote add <remote_name> <remote_url>\ngit remote set-url <remote_name> <new_url>",
        "  List existing remotes with their names and URLs: git remote <[-v|--verbose]>\n  Show information about a remote: git remote show <remote_name>\n  Add a remote: git remote add <remote_name> <remote_url>\n  Change the URL of a remote (use `--add` to keep the existing URL): git remote set-url <remote_name> <new_url>\n  Show the URL of a remote: git remote get-url <remote_name>\n  Remove a remote: git remote remove <remote_name>",
        NULL
    },

    /* git rename-branch */
    {
        {"rename a specific branch", "rename the branch you are currently on", NULL},
        "git rename-branch <new_branch_name>\ngit rename-branch <old_branch_name> <new_branch_name>",
        "  Rename the branch you are currently on: git rename-branch <new_branch_name>\n  Rename a specific branch: git rename-branch <old_branch_name> <new_branch_name>",
        NULL
    },

    /* git rename-remote */
    {
        {"change the upstream remote to origin", NULL},
        "git rename-remote <upstream> <origin>",
        "  Change the upstream remote to origin: git rename-remote <upstream> <origin>",
        NULL
    },

    /* git rename-tag */
    {
        {"rename an existing git tag locally and remotely", NULL},
        "git rename-tag <old_tag_name> <new_tag_name>",
        "  Rename an existing Git tag locally and remotely: git rename-tag <old_tag_name> <new_tag_name>",
        NULL
    },

    /* git repack */
    {
        {"repack all objects into a single pack", "limit the repack to local objects only", "pack unpacked objects in the current directory", NULL},
        "git repack\ngit repack -d\ngit repack -a\ngit repack -l",
        "  Pack unpacked objects in the current directory: git repack\n  Remove redundant objects after packing: git repack -d\n  Repack all objects into a single pack: git repack -a\n  Limit the repack to local objects only: git repack -l",
        NULL
    },

    /* git repl */
    {
        {"start an interactive git shell", "exit the interactive git shell (or press `<ctrl d>`)", NULL},
        "git repl\n<git_subcommand> <command_arguments>\n!<command> <command_arguments>\nexit",
        "  Start an interactive Git shell: git repl\n  Run a Git command while in the interactive Git shell: <git_subcommand> <command_arguments>\n  Run an external (non-Git) command while in the interactive Git shell: !<command> <command_arguments>\n  Exit the interactive Git shell (or press `<Ctrl d>`): exit",
        NULL
    },

    /* git replace */
    {
        {"edit an object\'s content interactively", NULL},
        "git replace <object> <replacement>\ngit replace <[-d|--delete]> <object>\ngit replace --edit <object>",
        "  Replace any commit with a different one, leaving other commits unchanged: git replace <object> <replacement>\n  Delete existing replace refs for the given objects: git replace <[-d|--delete]> <object>\n  Edit an object\'s content interactively: git replace --edit <object>",
        NULL
    },

    /* git rerere */
    {
        {"enable rerere globally", "forget a file\'s recorded resolution", NULL},
        "git config --global rerere.enabled true\ngit rerere forget <path/to/file>\ngit rerere status",
        "  Enable rerere globally: git config --global rerere.enabled true\n  Forget a file\'s recorded resolution: git rerere forget <path/to/file>\n  Check the status of recorded resolutions: git rerere status",
        NULL
    },

    /* git reset */
    {
        {"unstage everything", "unstage specific file(s)", "interactively unstage portions of a file", NULL},
        "git reset\ngit reset <path/to/file1 path/to/file2 ...>\ngit reset <[-p|--patch]> <path/to/file>\ngit reset HEAD~",
        "  Unstage everything: git reset\n  Unstage specific file(s): git reset <path/to/file1 path/to/file2 ...>\n  Interactively unstage portions of a file: git reset <[-p|--patch]> <path/to/file>\n  Undo the last commit, keeping its changes (and any further uncommitted changes) in the filesystem: git reset HEAD~\n  Undo the last two commits, adding their changes to the index, i.e. staged for commit: git reset --soft HEAD~2\n  Discard any uncommitted changes, staged or not (for only unstaged changes, use `git checkout`): git reset --hard",
        "Discards changes permanently and cannot be undone."
    },

    /* git restore */
    {
        {"unstage a file", "unstage all files", "discard all unstaged changes to tracked files", "interactively select sections of files to restore", "discard all changes to files, both staged and unstaged", NULL},
        "git restore <path/to/file>\ngit restore <[-s|--source]> <commit> <path/to/file>\ngit restore :/\ngit restore <[-S|--staged]> <path/to/file>",
        "  Restore an unstaged file to the staged version: git restore <path/to/file>\n  Restore an unstaged file to the version of a specific commit: git restore <[-s|--source]> <commit> <path/to/file>\n  Discard all unstaged changes to tracked files: git restore :/\n  Unstage a file: git restore <[-S|--staged]> <path/to/file>\n  Unstage all files: git restore <[-S|--staged]> :/\n  Discard all changes to files, both staged and unstaged: git restore <[-W|--worktree]> <[-S|--staged]> :/",
        NULL
    },

    /* git rev-list */
    {
        {"print the number of commits since a specific tag", NULL},
        "git rev-list <HEAD>\ngit rev-list <[-n|--max-count]> 1 HEAD -- <path/to/file>\ngit rev-list --since \"<2019-12-01 00:00:00>\" <branch_name>\ngit rev-list --merges <commit>",
        "  List all commits on the current branch: git rev-list <HEAD>\n  Print the latest commit that changed (add/edit/remove) a specific file on the current branch: git rev-list <[-n|--max-count]> 1 HEAD -- <path/to/file>\n  List commits more recent than a specific date, on a specific branch: git rev-list --since \"<2019-12-01 00:00:00>\" <branch_name>\n  List all merge commits on a specific commit: git rev-list --merges <commit>\n  Print the number of commits since a specific tag: git rev-list <tag_name>..HEAD --count",
        NULL
    },

    /* git revert */
    {
        {"revert multiple commits", "revert a specific commit", "revert the 5th last commit", "revert the most recent commit", "cancel a git revert after a merge conflict", "don\'t create new commits, just change the working tree", NULL},
        "git revert HEAD\ngit revert HEAD~4\ngit revert <0c01a9>\ngit revert <branch_name~5>..<branch_name~2>",
        "  Revert the most recent commit: git revert HEAD\n  Revert the 5th last commit: git revert HEAD~4\n  Revert a specific commit: git revert <0c01a9>\n  Revert multiple commits: git revert <branch_name~5>..<branch_name~2>\n  Don\'t create new commits, just change the working tree: git revert <[-n|--no-commit]> <0c01a9>..<9a1743>\n  Cancel a Git revert after a merge conflict: git revert --abort",
        NULL
    },

    /* git rm */
    {
        {"reduce the repository to only the `.git` directory", NULL},
        "git rm <path/to/file>\ngit rm -r <path/to/directory>\ngit rm --cached <path/to/file>\ngit rm -r .",
        "  Remove file from repository index and filesystem: git rm <path/to/file>\n  Remove directory [r]ecursively: git rm -r <path/to/directory>\n  Remove file from repository index but keep it untouched locally: git rm --cached <path/to/file>\n  Reduce the repository to only the `.git` directory: git rm -r .",
        NULL
    },

    /* git root */
    {
        {"print the absolute path of the current git repository", NULL},
        "git root\ngit root <[-r|--relative]>",
        "  Print the absolute path of the current Git repository: git root\n  Print the current working directory relative to the root of the current Git repository: git root <[-r|--relative]>",
        NULL
    },

    /* git rscp */
    {
        {"copy specific files from a remote", "copy a specific directory from a remote", NULL},
        "git rscp <remote_name> <path/to/file1 path/to/file2 ...>\ngit rscp <remote_name> <path/to/directory>",
        "  Copy specific files from a remote: git rscp <remote_name> <path/to/file1 path/to/file2 ...>\n  Copy a specific directory from a remote: git rscp <remote_name> <path/to/directory>",
        NULL
    },

    /* git scp */
    {
        {"copy specific files to a remote", "copy a specific directory to a remote", "copy unstaged files to a specific remote", "copy staged and unstaged files to a remote", NULL},
        "git scp <remote_name>\ngit scp <remote_name> HEAD\ngit scp <remote_name> HEAD~1\ngit scp <remote_name> <path/to/file1 path/to/file2 ...>",
        "  Copy unstaged files to a specific remote: git scp <remote_name>\n  Copy staged and unstaged files to a remote: git scp <remote_name> HEAD\n  Copy files that has been changed in the last commit and any staged or unstaged files to a remote: git scp <remote_name> HEAD~1\n  Copy specific files to a remote: git scp <remote_name> <path/to/file1 path/to/file2 ...>\n  Copy a specific directory to a remote: git scp <remote_name> <path/to/directory>",
        NULL
    },

    /* git secret */
    {
        {"encrypt secrets", "decrypt secret files", "grant access by email", "register a secret file", "revoke access by email", "grant access to the current git user\'s email", "initialize `git-secret` in a local repository", NULL},
        "git secret init\ngit secret tell -m\ngit secret tell <email>\ngit secret killperson <email>",
        "  Initialize `git-secret` in a local repository: git secret init\n  Grant access to the current Git user\'s email: git secret tell -m\n  Grant access by email: git secret tell <email>\n  Revoke access by email: git secret killperson <email>\n  List emails with access to secrets: git secret whoknows\n  Register a secret file: git secret add <path/to/file>",
        NULL
    },

    /* git sed */
    {
        {"replace the specified text, using `regex`", "replace the specified text in the current repository", NULL},
        "git sed \'<find_text>\' \'<replace_text>\'\ngit sed -c \'<find_text>\' \'<replace_text>\'\ngit sed -f g \'<find_text>\' \'<replace_text>\'\ngit sed \'<find_text>\' \'<replace_text>\' -- <path/to/directory>",
        "  Replace the specified text in the current repository: git sed \'<find_text>\' \'<replace_text>\'\n  Replace the specified text and then commit the resulting changes with a standard commit message: git sed -c \'<find_text>\' \'<replace_text>\'\n  Replace the specified text, using `regex`: git sed -f g \'<find_text>\' \'<replace_text>\'\n  Replace a specific text in all files under a given directory: git sed \'<find_text>\' \'<replace_text>\' -- <path/to/directory>",
        NULL
    },

    /* git send-email */
    {
        {"send a given commit", "send multiple (e.g. 10) commits in the current branch", NULL},
        "git send-email -1\ngit send-email -1 <commit>\ngit send-email <-10>\ngit send-email -<number_of_commits> --compose",
        "  Send the last commit in the current branch interactively: git send-email -1\n  Send a given commit: git send-email -1 <commit>\n  Send multiple (e.g. 10) commits in the current branch: git send-email <-10>\n  Send an introductory email message for the patch series: git send-email -<number_of_commits> --compose\n  Review and edit the email message for each patch you\'re about to send: git send-email -<number_of_commits> --annotate",
        NULL
    },

    /* git show-branch */
    {
        {"compare all remote tracking branches", "compare a given branch with the current branch", "compare both local and remote tracking branches", "display the commit name instead of the relative name", NULL},
        "git show-branch <branch_name|ref|commit>\ngit show-branch <branch_name1|ref1|commit1 branch_name2|ref2|commit2 ...>\ngit show-branch <[-r|--remotes]>\ngit show-branch <[-a|--all]>",
        "  Show a summary of the latest commit on a branch: git show-branch <branch_name|ref|commit>\n  Compare commits in the history of multiple commits or branches: git show-branch <branch_name1|ref1|commit1 branch_name2|ref2|commit2 ...>\n  Compare all remote tracking branches: git show-branch <[-r|--remotes]>\n  Compare both local and remote tracking branches: git show-branch <[-a|--all]>\n  List the latest commits in all branches: git show-branch <[-a|--all]> --list\n  Compare a given branch with the current branch: git show-branch --current <commit|branch_name|ref>",
        NULL
    },

    /* git show-ref */
    {
        {"verify that a given reference exists", NULL},
        "git show-ref\ngit show-ref --branches\ngit show-ref --tags\ngit show-ref --verify <path/to/ref>",
        "  Show all refs in the repository: git show-ref\n  Show only heads references: git show-ref --branches\n  Show only tags references: git show-ref --tags\n  Verify that a given reference exists: git show-ref --verify <path/to/ref>",
        NULL
    },

    /* git-sizer */
    {
        {"git sizer", "report all statistics", "see additional options", NULL},
        "git-sizer\ngit-sizer -v\ngit-sizer -h",
        "  Report only statistics that have a level of concern greater than 0: git-sizer\n  Report all statistics: git-sizer -v\n  See additional options: git-sizer -h",
        NULL
    },

    /* git sparse-checkout */
    {
        {"enable sparse checkout", "specify which directories (or files) to include", "disable sparse-checkout and restore full repository", NULL},
        "git sparse-checkout init\ngit sparse-checkout disable\ngit sparse-checkout set <path/to/directory>\ngit sparse-checkout add <path/to/directory>",
        "  Enable sparse checkout: git sparse-checkout init\n  Disable sparse-checkout and restore full repository: git sparse-checkout disable\n  Specify which directories (or files) to include: git sparse-checkout set <path/to/directory>\n  Add more paths later: git sparse-checkout add <path/to/directory>",
        NULL
    },

    /* git standup */
    {
        {"display help", NULL},
        "git standup -a <name|email> -d <10>\ngit standup -a <name|email> -d <10> -g\ngit standup -a all -d <10>\ngit standup -h",
        "  Show a given author\'s commits from the last 10 days: git standup -a <name|email> -d <10>\n  Show a given author\'s commits from the last 10 days and whether they are GPG signed: git standup -a <name|email> -d <10> -g\n  Show all the commits from all contributors for the last 10 days: git standup -a all -d <10>\n  Display help: git standup -h",
        NULL
    },

    /* git stash */
    {
        {"drop all stashes", NULL},
        "git stash push <[-m|--message]> <stash_message>\ngit stash <[-u|--include-untracked]>\ngit stash <[-p|--patch]>\ngit stash list",
        "  Stash current changes with a message, except new (untracked) files: git stash push <[-m|--message]> <stash_message>\n  Stash current changes, including new untracked files: git stash <[-u|--include-untracked]>\n  Interactively select parts of changed files for stashing: git stash <[-p|--patch]>\n  List all stashes (shows stash name, related branch and message): git stash list\n  Show the changes as a patch between the stash (default is `stash@{N}`) and the commit back when stash entry was first created: git stash show <[-p|--patch]> <stash@{N}\n  Apply a stash (default is the latest, named `stash@{N}`): git stash apply <optional_stash_name_or_commit>",
        "Deletes all stashes permanently."
    },

    /* git status */
    {
        {"give output in short format", "don\'t show untracked files in the output", NULL},
        "git status\ngit status <[-s|--short]>\ngit status <[-vv|--verbose --verbose]>\ngit status <[-b|--branch]>",
        "  Show changed files which are not yet added for commit: git status\n  Give output in short format: git status <[-s|--short]>\n  Show verbose information on changes in both the staging area and working directory: git status <[-vv|--verbose --verbose]>\n  Show the branch and tracking info: git status <[-b|--branch]>\n  Show output in short format along with branch info: git status <[-sb|--short --branch]>\n  Show the number of entries currently stashed away: git status --show-stash",
        NULL
    },

    /* git stripspace */
    {
        {"trim whitespace from a file", "trim whitespace and git comments from a file", "convert all lines in a file into git comments", NULL},
        "cat <path/to/file> | git stripspace\ncat <path/to/file> | git stripspace <[-s|--strip-comments]>\ngit < <path/to/file> stripspace <[-c|--comment-lines]>",
        "  Trim whitespace from a file: cat <path/to/file> | git stripspace\n  Trim whitespace and Git comments from a file: cat <path/to/file> | git stripspace <[-s|--strip-comments]>\n  Convert all lines in a file into Git comments: git < <path/to/file> stripspace <[-c|--comment-lines]>",
        NULL
    },

    /* git submodule */
    {
        {"change the url of a submodule", "update submodules to their latest commits", NULL},
        "git submodule\ngit submodule update --init --recursive\ngit submodule add <repository_url>\ngit submodule add <repository_url> <path/to/directory>",
        "  View existing submodules and the checked-out commit for each one: git submodule\n  Install a repository\'s submodules (listed in `.gitmodules`): git submodule update --init --recursive\n  Add a Git repository as a submodule of the current one: git submodule add <repository_url>\n  Add a Git repository as a submodule of the current one, at a specific directory: git submodule add <repository_url> <path/to/directory>\n  Update submodules to their latest commits: git submodule update --remote\n  Change the URL of a submodule: git submodule set-url <path/to/submodule> <new_url>",
        NULL
    },

    /* git subtree */
    {
        {"update subtree repository to its latest commit", NULL},
        "git subtree add <[-P|--prefix]> <path/to/directory> --squash <repository_url> <branch_name>\ngit subtree pull <[-P|--prefix]> <path/to/directory> <repository_url> <branch_name>\ngit subtree merge <[-P|--prefix]> <path/to/directory> --squash <repository_url> <branch_name>\ngit subtree push <[-P|--prefix]> <path/to/directory> <repository_url> <branch_name>",
        "  Add a Git repository as a subtree and squash the commits together: git subtree add <[-P|--prefix]> <path/to/directory> --squash <repository_url> <branch_name>\n  Update subtree repository to its latest commit: git subtree pull <[-P|--prefix]> <path/to/directory> <repository_url> <branch_name>\n  Merge recent changes up to the latest subtree commit into the subtree: git subtree merge <[-P|--prefix]> <path/to/directory> --squash <repository_url> <branch_name>\n  Push commits to a subtree repository: git subtree push <[-P|--prefix]> <path/to/directory> <repository_url> <branch_name>\n  Extract a new project history from the history of a subtree: git subtree split <[-P|--prefix]> <path/to/directory> <repository_url> <[-b|--branch]> <branch_name>",
        NULL
    },

    /* git summary */
    {
        {"display data about a git repository", "display data about a git repository since a commit-ish", NULL},
        "git summary\ngit summary <commit|branch_name|tag_name>\ngit summary --dedup-by-email\ngit summary --line",
        "  Display data about a Git repository: git summary\n  Display data about a Git repository since a commit-ish: git summary <commit|branch_name|tag_name>\n  Display data about a Git repository, merging committers using different emails into 1 statistic for each author: git summary --dedup-by-email\n  Display data about a Git repository, showing the number of lines modified by each contributor: git summary --line",
        NULL
    },

    /* git svn */
    {
        {"update local clone from the remote svn repository", NULL},
        "git svn clone <https://example.com/subversion_repo> <local_directory>\ngit svn clone <[-r|--revision]> <1234>:HEAD <https://svn.example.net/subversion/repo> <local_directory>\ngit svn rebase\ngit svn fetch",
        "  Clone an SVN repository: git svn clone <https://example.com/subversion_repo> <local_directory>\n  Clone an SVN repository starting at a given revision number: git svn clone <[-r|--revision]> <1234>:HEAD <https://svn.example.net/subversion/repo> <local_directory>\n  Update local clone from the remote SVN repository: git svn rebase\n  Fetch updates from the remote SVN repository without changing the Git `HEAD`: git svn fetch\n  Commit back to the SVN repository: git svn commit",
        NULL
    },

    /* git symbolic-ref */
    {
        {"read a reference by name", "store a reference by a name", NULL},
        "git symbolic-ref refs/<name> <ref>\ngit symbolic-ref -m \"<message>\" refs/<name> refs/heads/<branch_name>\ngit symbolic-ref refs/<name>\ngit symbolic-ref <[-d|--delete]> refs/<name>",
        "  Store a reference by a name: git symbolic-ref refs/<name> <ref>\n  Store a reference by name, including a message with a reason for the update: git symbolic-ref -m \"<message>\" refs/<name> refs/heads/<branch_name>\n  Read a reference by name: git symbolic-ref refs/<name>\n  Delete a reference by name: git symbolic-ref <[-d|--delete]> refs/<name>\n  For scripting, hide errors with `--quiet` and use `--short` to simplify (\"refs/heads/X\" prints as \"X\"): git symbolic-ref <[-q|--quiet]> --short refs/<name>",
        NULL
    },

    /* git sync */
    {
        {"sync without cleaning untracked files", "sync the current local branch with its remote branch", NULL},
        "git sync\ngit sync origin main\ngit sync <[-s|--soft]> <remote_name> <branch_name>",
        "  Sync the current local branch with its remote branch: git sync\n  Sync the current local branch with the remote main branch: git sync origin main\n  Sync without cleaning untracked files: git sync <[-s|--soft]> <remote_name> <branch_name>",
        NULL
    },

    /* git update-ref */
    {
        {"update ref with a message", NULL},
        "git update-ref -d <HEAD>\ngit update-ref -m <message> <HEAD> <4e95e05>",
        "  Delete a ref, useful for soft resetting the first commit: git update-ref -d <HEAD>\n  Update ref with a message: git update-ref -m <message> <HEAD> <4e95e05>",
        NULL
    },

    /* git var */
    {
        {"[l]ist all git logical variables", "print the value of a git logical variable", NULL},
        "git var <GIT_AUTHOR_IDENT|GIT_COMMITTER_IDENT|GIT_EDITOR|GIT_PAGER>\ngit var -l",
        "  Print the value of a Git logical variable: git var <GIT_AUTHOR_IDENT|GIT_COMMITTER_IDENT|GIT_EDITOR|GIT_PAGER>\n  [l]ist all Git logical variables: git var -l",
        NULL
    },

    /* git verify-pack */
    {
        {"verify a packed git archive file", NULL},
        "git verify-pack <path/to/pack-file>\ngit verify-pack <[-v|--verbose]> <path/to/pack-file>\ngit verify-pack <[-s|--stat-only]> <path/to/pack-file>",
        "  Verify a packed Git archive file: git verify-pack <path/to/pack-file>\n  Verify a packed Git archive file and show verbose details: git verify-pack <[-v|--verbose]> <path/to/pack-file>\n  Verify a packed Git archive file and only display the statistics: git verify-pack <[-s|--stat-only]> <path/to/pack-file>",
        NULL
    },

    /* git whatchanged */
    {
        {"display logs and changes for recent commits", NULL},
        "git whatchanged\ngit whatchanged --since \"<2 hours ago>\"\ngit whatchanged <path/to/file_or_directory>",
        "  Display logs and changes for recent commits: git whatchanged\n  Display logs and changes for recent commits within the specified time frame: git whatchanged --since \"<2 hours ago>\"\n  Display logs and changes for recent commits for specific files or directories: git whatchanged <path/to/file_or_directory>",
        NULL
    },

    /* git worktree */
    {
        {"move an existing worktree to a new location", NULL},
        "git worktree add <branch>\ngit worktree add <path/to/directory> <branch>\ngit worktree add <path/to/directory> -b <new_branch>\ngit worktree list",
        "  Add a new worktree directory for a branch with the same name (created if missing): git worktree add <branch>\n  Create a new directory with the specified branch checked out into it: git worktree add <path/to/directory> <branch>\n  Create a new directory with a new branch checked out into it: git worktree add <path/to/directory> -b <new_branch>\n  List all the working directories (including the primary one): git worktree list\n  Move an existing worktree to a new location: git worktree move <path/to/worktree> <new/path>\n  Remove a worktree directory and its metadata (only if it has no uncommitted changes): git worktree remove <path/to/worktree>",
        NULL
    },

    /* git */
    {
        {"stage all changes for a commit", NULL},
        "git init\ngit clone <https://example.com/repo.git>\ngit status\ngit add <[-A|--all]>",
        "  Create an empty Git repository: git init\n  Clone a remote Git repository from the internet: git clone <https://example.com/repo.git>\n  View the status of the local repository: git status\n  Stage all changes for a commit: git add <[-A|--all]>\n  Commit changes to version history: git commit <[-m|--message]> <message_text>\n  Push local commits to a remote repository: git push",
        "Discards changes permanently and cannot be undone."
    },

    /* gita */
    {
        {"git gita", "display information of all repositories", "register one or more repositories with gita", "display a summary of a[ll] registered repositories", NULL},
        "gita ll\ngita st\ngita add <path/to/repo1 path/to/repo2 ...>\ngita pull",
        "  Display a summary of a[ll] registered repositories: gita ll\n  Show the [st]atus of all registered repositories: gita st\n  Register one or more repositories with gita: gita add <path/to/repo1 path/to/repo2 ...>\n  Pull updates for all registered repositories: gita pull\n  Run a Git command on a specific repository: gita super <repo_name> <git_command>\n  Create a group of repositories and set it as the active context: gita group add <[-n|--name]> <group_name> <repo1 repo2 ...> && gita context <group_name>",
        NULL
    },

    /* gitea */
    {
        {"git gitea", "display help", "display version", "display help for a specific subcommand", NULL},
        "gitea web\ngitea migrate\ngitea admin <user list>\ngitea <admin> --help",
        "  Run the Gitea web server using the default configuration: gitea web\n  Create the necessary database schema and tables: gitea migrate\n  Run administrative subcommands for user management or authentication management: gitea admin <user list>\n  Display help for a specific subcommand: gitea <admin> --help\n  Display help: gitea help\n  Display version: gitea --version",
        NULL
    },

    /* github-label-sync */
    {
        {"git github-label-sync", "keep labels that aren\'t in `labels.json`", "synchronize labels using a local `labels.json` file", "synchronize labels using a specific labels json file", NULL},
        "github-label-sync --access-token <token> <repository_name>\ngithub-label-sync --access-token <token> --labels <url|path/to/json_file> <repository_name>\ngithub-label-sync --access-token <token> --dry-run <repository_name>\ngithub-label-sync --access-token <token> --allow-added-labels <repository_name>",
        "  Synchronize labels using a local `labels.json` file: github-label-sync --access-token <token> <repository_name>\n  Synchronize labels using a specific labels JSON file: github-label-sync --access-token <token> --labels <url|path/to/json_file> <repository_name>\n  Perform a dry run instead of actually synchronizing labels: github-label-sync --access-token <token> --dry-run <repository_name>\n  Keep labels that aren\'t in `labels.json`: github-label-sync --access-token <token> --allow-added-labels <repository_name>\n  Synchronize using the `$GITHUB_ACCESS_TOKEN` environment variable: github-label-sync <repository_name>",
        NULL
    },

    /* gitk */
    {
        {"git gitk", NULL},
        "gitk\ngitk <path/to/file_or_directory>\ngitk --since=\"1 week ago\"\ngitk --until=\"1/1/2015\"",
        "  Show the repository browser for the current Git repository: gitk\n  Show repository browser for a specific file or directory: gitk <path/to/file_or_directory>\n  Show commits made since 1 week ago: gitk --since=\"1 week ago\"\n  Show commits older than 1/1/2015: gitk --until=\"1/1/2015\"\n  Show at most 100 changes in all branches: gitk --max-count=100 --all",
        NULL
    },

    /* gitlab-backup */
    {
        {"git gitlab-backup", NULL},
        "sudo gitlab-backup create\nsudo gitlab-backup create STRATEGY=<copy>\nsudo gitlab-backup restore BACKUP=<backup_id>\nsudo gitlab-backup restore BACKUP=<backup_id> SKIP=<db,uploads,...>",
        "  Create a full backup (default strategy): sudo gitlab-backup create\n  Create a full backup using the copy strategy: sudo gitlab-backup create STRATEGY=<copy>\n  Restore a backup by specifying its ID: sudo gitlab-backup restore BACKUP=<backup_id>\n  Restore a backup while skipping specific components: sudo gitlab-backup restore BACKUP=<backup_id> SKIP=<db,uploads,...>",
        NULL
    },

    /* gitlab-ctl */
    {
        {"git gitlab-ctl", "restart every service", "restart a specific service", "reconfigure the application", "display the status of every service", "display the logs of a specific service", "display the status of a specific service", NULL},
        "sudo gitlab-ctl status\nsudo gitlab-ctl status <nginx>\nsudo gitlab-ctl restart\nsudo gitlab-ctl restart <nginx>",
        "  Display the status of every service: sudo gitlab-ctl status\n  Display the status of a specific service: sudo gitlab-ctl status <nginx>\n  Restart every service: sudo gitlab-ctl restart\n  Restart a specific service: sudo gitlab-ctl restart <nginx>\n  Display the logs of every service and keep reading until `<Ctrl c>` is pressed: sudo gitlab-ctl tail\n  Display the logs of a specific service: sudo gitlab-ctl tail <nginx>",
        NULL
    },

    /* gitlab-runner */
    {
        {"register a runner", "git gitlab-runner", "unregister a runner", "restart the runner service", "register a runner with a docker executor", "display the status of the runner service", NULL},
        "sudo gitlab-runner register --url <https://gitlab.example.com> --registration-token <token> --name <name>\nsudo gitlab-runner register --url <https://gitlab.example.com> --registration-token <token> --name <name> --executor <docker>\nsudo gitlab-runner unregister --name <name>\nsudo gitlab-runner status",
        "  Register a runner: sudo gitlab-runner register --url <https://gitlab.example.com> --registration-token <token> --name <name>\n  Register a runner with a Docker executor: sudo gitlab-runner register --url <https://gitlab.example.com> --registration-token <token> --name <name> --executor <docker>\n  Unregister a runner: sudo gitlab-runner unregister --name <name>\n  Display the status of the runner service: sudo gitlab-runner status\n  Restart the runner service: sudo gitlab-runner restart\n  Check if the registered runners can connect to GitLab: sudo gitlab-runner verify",
        NULL
    },

    /* gitlab */
    {
        {"git gitlab", "start a specific ci job", NULL},
        "gitlab create_project <project_name>\ngitlab commit <project_name> <commit_hash>\ngitlab pipeline_jobs <project_name> <pipeline_id>\ngitlab job_play <project_name> <job_id>",
        "  Create a new project: gitlab create_project <project_name>\n  Get info about a specific commit: gitlab commit <project_name> <commit_hash>\n  Get info about jobs in a CI pipeline: gitlab pipeline_jobs <project_name> <pipeline_id>\n  Start a specific CI job: gitlab job_play <project_name> <job_id>",
        NULL
    },

    /* gitleaks */
    {
        {"git gitleaks", "scan a local directory", "use a custom rules file", "scan a remote repository", "output scan results to a json file", "start scanning from a specific commit", "scan uncommitted changes before a commit", NULL},
        "gitleaks detect --repo-url <https://github.com/username/repository.git>\ngitleaks detect <[-s|--source]> <path/to/repository>\ngitleaks detect <[-s|--source]> <path/to/repository> --report <path/to/report.json>\ngitleaks detect <[-s|--source]> <path/to/repository> --config-path <path/to/config.toml>",
        "  Scan a remote repository: gitleaks detect --repo-url <https://github.com/username/repository.git>\n  Scan a local directory: gitleaks detect <[-s|--source]> <path/to/repository>\n  Output scan results to a JSON file: gitleaks detect <[-s|--source]> <path/to/repository> --report <path/to/report.json>\n  Use a custom rules file: gitleaks detect <[-s|--source]> <path/to/repository> --config-path <path/to/config.toml>\n  Start scanning from a specific commit: gitleaks detect <[-s|--source]> <path/to/repository> --log-opts <--since=commit_id>\n  Scan uncommitted changes before a commit: gitleaks protect --staged",
        NULL
    },

    /* gitlint */
    {
        {"git gitlint", "start a specific ci job", "range of commits to lint", "path to a file containing a commit-msg", "read staged commit meta-info from the local repository", NULL},
        "gitlint\ngitlint --commits <single_refspec_argument>\ngitlint --extra-path <path/to/directory>\ngitlint --target <path/to/target_directory>",
        "  Check the last commit message: gitlint\n  The range of commits to lint: gitlint --commits <single_refspec_argument>\n  Path to a directory or Python module with extra user-defined rules: gitlint --extra-path <path/to/directory>\n  Start a specific CI job: gitlint --target <path/to/target_directory>\n  Path to a file containing a commit-msg: gitlint --msg-filename <path/to/file>\n  Read staged commit meta-info from the local repository: gitlint --staged",
        NULL
    },

    /* gitmoji */
    {
        {"git gitmoji", "start the commit wizard", "configure global preferences", "search emoji list for a list of keywords", "update cached list of emojis from main repository", NULL},
        "gitmoji --commit\ngitmoji --init\ngitmoji --remove\ngitmoji --list",
        "  Start the commit wizard: gitmoji --commit\n  Initialize the Git hook (so `gitmoji` will be run every time `git commit` is run): gitmoji --init\n  Remove the Git hook: gitmoji --remove\n  List all available emojis and their descriptions: gitmoji --list\n  Search emoji list for a list of keywords: gitmoji --search <keyword1> <keyword2>\n  Update cached list of emojis from main repository: gitmoji --update",
        NULL
    },

    /* gitsome */
    {
        {"git gitsome", "setup github integration with the current account", NULL},
        "gitsome\ngh configure\ngh notifications\ngh starred \"<python 3>\"",
        "  Enter the gitsome shell (optional), to enable autocompletion and interactive help for Git (and gh) commands: gitsome\n  Setup GitHub integration with the current account: gh configure\n  List notifications for the current account (as would be seen in <https://github.com/notifications>): gh notifications\n  List the current account\'s starred repos, filtered by a given search string: gh starred \"<python 3>\"\n  View the recent activity feed of a given GitHub repository: gh feed <tldr-pages/tldr>\n  View the recent activity feed for a given GitHub user, using the default pager (e.g. `less`): gh feed <torvalds> <[-p|--pager]>",
        NULL
    },

    /* gitstats */
    {
        {"git gitstats", "display help", "generate statistics for a local repository", NULL},
        "gitstats <path/to/git_repo/.git> <path/to/output_folder>\n<Invoke-Item|open|xdg-open> <path/to/output_folder/index.html>\ngitstats <[-h|--help]>",
        "  Generate statistics for a local repository: gitstats <path/to/git_repo/.git> <path/to/output_folder>\n  View generated statistics in a web browser on Windows (PowerShell)/macOS/Linux: <Invoke-Item|open|xdg-open> <path/to/output_folder/index.html>\n  Display help: gitstats <[-h|--help]>",
        NULL
    },

    /* gitui */
    {
        {"git gitui", "generate a bug report", "use a specific git directory", "use a specific working directory", "store logging output into a specific file", NULL},
        "gitui\ngitui <[-t|--theme]> <theme2.ron>\ngitui --logfile <path/to/file>\ngitui <[-f|--file]> <path/to/file>",
        "  Start `gitui` for the repository in the current directory: gitui\n  Specify the filename of the color theme loaded from the config directory (defaults to `theme.ron`): gitui <[-t|--theme]> <theme2.ron>\n  Store logging output into a specific file: gitui --logfile <path/to/file>\n  Inspect a specific file inside the repository in the current directory: gitui <[-f|--file]> <path/to/file>\n  Use notify-based filesystem watcher instead of tick-based update: gitui --watcher\n  Generate a bug report: gitui --bugreport",
        NULL
    },

    /* gitwatch */
    {
        {"git gitwatch", NULL},
        "gitwatch <path/to/file_or_directory>\ngitwatch -r <remote_name> <path/to/file_or_directory>\ngitwatch -r <remote_name> -b <branch_name> <path/to/file_or_directory>",
        "  Automatically commit any changes made to a file or directory: gitwatch <path/to/file_or_directory>\n  Automatically commit changes and push them to a remote repository: gitwatch -r <remote_name> <path/to/file_or_directory>\n  Automatically commit changes and push them to a specific branch of a remote repository: gitwatch -r <remote_name> -b <branch_name> <path/to/file_or_directory>",
        NULL
    },

    /* ── END GENERATED FROM TLDR ────────────────────────────────────────── */

    /* sentinel */
    {{NULL}, NULL, NULL, NULL}
};

#define GIT_DB_SIZE (sizeof(GIT_DB) / sizeof(GIT_DB[0]) - 1)

#endif /* GITREF_H */
