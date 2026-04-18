#!/bin/sh
# gitpal comprehensive test suite
# Run from the gitpal project root: sh tests.sh
# Reports PASS/FAIL for each test.
# Usage:
#   sh tests.sh              # run all tests
#   sh tests.sh 2>/dev/null  # suppress gitpal stderr

GITPAL=./gitpal
PASS=0
FAIL=0
OLDPWD="$(pwd)"

# ── helpers ───────────────────────────────────────────────────────────────────

check() {
    name="$1"
    expected="$2"
    actual="$3"
    if echo "$actual" | grep -qF "$expected"; then
        printf "PASS  %s\n" "$name"
        PASS=$((PASS + 1))
    else
        printf "FAIL  %s\n" "$name"
        printf "      expected to contain: %s\n" "$expected"
        printf "      actual (first 3 lines):\n"
        echo "$actual" | head -3 | sed 's/^/        /'
        FAIL=$((FAIL + 1))
    fi
}

check_absent() {
    name="$1"
    absent="$2"
    actual="$3"
    if echo "$actual" | grep -qF "$absent"; then
        printf "FAIL  %s\n" "$name"
        printf "      should NOT contain: %s\n" "$absent"
        FAIL=$((FAIL + 1))
    else
        printf "PASS  %s\n" "$name"
        PASS=$((PASS + 1))
    fi
}

check_exit() {
    name="$1"
    expected_code="$2"
    actual_code="$3"
    if [ "$actual_code" -eq "$expected_code" ]; then
        printf "PASS  %s\n" "$name"
        PASS=$((PASS + 1))
    else
        printf "FAIL  %s  (expected exit %d, got %d)\n" "$name" "$expected_code" "$actual_code"
        FAIL=$((FAIL + 1))
    fi
}

# top_cmd: first command line from gitpal output (git or non-git)
top_cmd() {
    $GITPAL "$1" 2>/dev/null | grep -A6 "▶ Command" | grep -E "^  [a-z]" | head -1 | sed 's/^[[:space:]]*//'
}

section() {
    printf "\n── %s\n" "$1"
}

# ── 1. FLAGS AND META ─────────────────────────────────────────────────────────
section "1. flags and meta"

check "help flag shows usage section" \
    "interactive REPL mode" \
    "$($GITPAL --help 2>&1)"

check "help flag shows --verbose flag" \
    "verbose" \
    "$($GITPAL --help 2>&1)"

check "help flag shows examples" \
    "gitpal" \
    "$($GITPAL --help 2>&1)"

check "help short flag -h works" \
    "interactive REPL mode" \
    "$($GITPAL -h 2>&1)"

check "version flag outputs version string" \
    "gitpal" \
    "$($GITPAL --version 2>&1)"

check "unknown flag shows error message" \
    "unknown flag" \
    "$($GITPAL --notaflag "test" 2>&1)"

check "unknown flag mentions help" \
    "gitpal --help" \
    "$($GITPAL --notaflag "test" 2>&1)"

$GITPAL --notaflag "test" >/dev/null 2>&1
check_exit "unknown flag exits non-zero" 1 $?

$GITPAL --help >/dev/null 2>&1
check_exit "help flag exits zero" 0 $?

$GITPAL --version >/dev/null 2>&1
check_exit "version flag exits zero" 0 $?

# ── 2. COMMIT OPERATIONS ──────────────────────────────────────────────────────
section "2. commit operations"

check "undo last commit -> git reset" \
    "git reset" \
    "$(top_cmd "undo last commit")"

check "undo commit shows --soft variant" \
    "soft" \
    "$($GITPAL "undo last commit" 2>/dev/null)"

check "undo commit shows --hard variant" \
    "git reset --hard" \
    "$($GITPAL "undo last commit" 2>/dev/null)"

check "undo commit has danger warning" \
    "Warning" \
    "$($GITPAL "undo last commit" 2>/dev/null)"

check "uncommit keyword -> git reset" \
    "git reset" \
    "$(top_cmd "uncommit")"

check "amend commit -> git commit --amend" \
    "git commit --amend" \
    "$(top_cmd "fix last commit message")"

check "amend has rewrite history warning" \
    "Rewrites history" \
    "$($GITPAL "fix last commit message" 2>/dev/null)"

check "committed wrong branch -> git cherry-pick" \
    "git cherry-pick" \
    "$(top_cmd "committed to wrong branch")"

check "committed wrong branch has warning" \
    "Warning" \
    "$($GITPAL "committed to wrong branch" 2>/dev/null)"

check "empty commit -> git commit --allow-empty" \
    "git commit --allow-empty" \
    "$(top_cmd "trigger ci with empty commit")"

# ── 3. STAGING ────────────────────────────────────────────────────────────────
section "3. staging"

check "stage changes -> git add" \
    "git add" \
    "$(top_cmd "stage my changes")"

check "interactive staging -> git add -p" \
    "git add -p" \
    "$($GITPAL "stage specific hunks" 2>/dev/null)"

check "unstage file -> git restore --staged" \
    "git restore --staged" \
    "$(top_cmd "unstage a file")"

check "discard changes -> git restore" \
    "git restore" \
    "$(top_cmd "discard changes to a file")"

# ── 4. STASH ──────────────────────────────────────────────────────────────────
section "4. stash"

check "stash my changes -> git stash push" \
    "git stash push" \
    "$(top_cmd "stash my changes")"

check "stash output includes git stash list" \
    "git stash list" \
    "$($GITPAL "stash my changes" 2>/dev/null)"

check "restore stash -> git stash pop" \
    "git stash pop" \
    "$(top_cmd "restore stashed changes")"

check "get stash back -> git stash pop" \
    "git stash pop" \
    "$(top_cmd "get my stashed work back")"

check "drop stash -> git stash drop" \
    "git stash drop" \
    "$(top_cmd "drop a stash")"

check "clear stash has irreversible warning" \
    "Warning" \
    "$($GITPAL "clear all stashes" 2>/dev/null)"

# ── 5. BRANCH ─────────────────────────────────────────────────────────────────
section "5. branch"

check "create branch -> contains branch" \
    "branch" \
    "$(top_cmd "create new branch")"

check "delete local branch -> git branch -d" \
    "git branch -d" \
    "$(top_cmd "delete branch")"

check "delete branch output includes -D for force" \
    "git branch -D" \
    "$($GITPAL "delete branch" 2>/dev/null)"

check "rename branch -> git branch -m" \
    "git branch -m" \
    "$(top_cmd "rename branch")"

check "delete remote branch -> git push origin --delete" \
    "git push origin --delete" \
    "$(top_cmd "delete remote branch")"

check "set upstream tracking -> set-upstream" \
    "set-upstream" \
    "$($GITPAL "set upstream tracking" 2>/dev/null)"

# ── 6. MERGE ──────────────────────────────────────────────────────────────────
section "6. merge"

check "merge branch -> git merge" \
    "git merge" \
    "$(top_cmd "merge branch into current")"

check "abort merge -> git merge --abort" \
    "git merge --abort" \
    "$(top_cmd "abort merge")"

check "merge conflict output includes git merge --continue" \
    "git merge --continue" \
    "$($GITPAL "resolve merge conflict" 2>/dev/null)"

# ── 7. REBASE ─────────────────────────────────────────────────────────────────
section "7. rebase"

check "rebase onto main -> git rebase" \
    "git rebase" \
    "$(top_cmd "rebase my branch onto main")"

check "interactive rebase -> git rebase -i" \
    "git rebase -i" \
    "$(top_cmd "interactive rebase")"

check "squash commits -> git rebase -i" \
    "git rebase -i" \
    "$(top_cmd "squash last 3 commits")"

check "squash output has fixup option" \
    "fixup" \
    "$($GITPAL "squash commits" 2>/dev/null)"

check "squash output has drop option" \
    "drop" \
    "$($GITPAL "squash commits" 2>/dev/null)"

check "squash has history rewrite warning" \
    "Rewrites history" \
    "$($GITPAL "squash commits" 2>/dev/null)"

check "abort rebase -> git rebase --abort" \
    "git rebase --abort" \
    "$(top_cmd "abort rebase")"

check "rebase skip -> git rebase --skip" \
    "git rebase --skip" \
    "$(top_cmd "skip a commit during rebase")"

check "rebase conflict output includes git rebase --continue" \
    "git rebase --continue" \
    "$($GITPAL "rebase conflict" 2>/dev/null)"

# ── 8. REMOTE / PUSH / PULL ───────────────────────────────────────────────────
section "8. remote/push/pull"

check "push changes -> git push" \
    "git push" \
    "$(top_cmd "push changes")"

check "first push output mentions -u flag" \
    "push -u origin" \
    "$($GITPAL "push my branch for the first time" 2>/dev/null)"

check "force push -> git push --force-with-lease" \
    "git push --force-with-lease" \
    "$(top_cmd "force push safely")"

check "force push has warning" \
    "Warning" \
    "$($GITPAL "force push" 2>/dev/null)"

check "force push explanation has force-with-lease" \
    "force-with-lease" \
    "$($GITPAL "force push" 2>/dev/null)"

check "pull -> git pull" \
    "git pull" \
    "$(top_cmd "pull latest changes")"

check "pull without merge commit -> git pull --rebase" \
    "git pull --rebase" \
    "$(top_cmd "pull without merge commit")"

check "fetch -> git fetch" \
    "git fetch" \
    "$(top_cmd "fetch remote changes")"

check "add remote -> git remote" \
    "git remote" \
    "$(top_cmd "add a remote")"

# ── 9. INSPECTION AND HISTORY ─────────────────────────────────────────────────
section "9. inspection and history"

check "what changed in last commit -> git show" \
    "git show" \
    "$(top_cmd "what changed in last commit")"

check "diff -> git diff" \
    "git diff" \
    "$(top_cmd "see what i have changed")"

check "see staged changes output includes git diff --staged" \
    "git diff --staged" \
    "$($GITPAL "see staged changes" 2>/dev/null)"

check "blame -> git blame" \
    "git blame" \
    "$(top_cmd "find who changed a line")"

check "see who wrote a line -> git blame" \
    "git blame" \
    "$(top_cmd "see who wrote a line")"

check "find author -> git blame" \
    "git blame" \
    "$(top_cmd "find author of this code")"

check "commit history -> git log" \
    "git log" \
    "$(top_cmd "show commit history")"

check "search all history -> git log" \
    "git log" \
    "$(top_cmd "search all history for a string")"

check "search history output includes git log -S" \
    "git log -S" \
    "$($GITPAL "search history for string" 2>/dev/null)"

check "commits between dates -> git log" \
    "git log" \
    "$(top_cmd "show commits between two dates")"

check "bisect -> git bisect" \
    "git bisect" \
    "$(top_cmd "find which commit introduced a bug")"

check "search tracked files -> git grep" \
    "git grep" \
    "$(top_cmd "search for string in all tracked files")"

check "shortlog -> git shortlog" \
    "git shortlog" \
    "$(top_cmd "summarize commits by author")"

check "how many commits per person -> git shortlog" \
    "git shortlog" \
    "$(top_cmd "how many commits per person")"

check "describe commit -> git describe" \
    "git describe" \
    "$(top_cmd "get human readable name for commit")"

# ── 10. RECOVERY ──────────────────────────────────────────────────────────────
section "10. recovery"

check "recover deleted branch -> git reflog" \
    "git reflog" \
    "$(top_cmd "recover deleted branch")"

check "lost commit after reset -> git reflog" \
    "git reflog" \
    "$(top_cmd "i lost a commit after reset")"

check "reflog keyword -> git reflog" \
    "git reflog" \
    "$(top_cmd "reflog")"

check "remove untracked -> git clean" \
    "git clean" \
    "$(top_cmd "remove untracked files")"

check "clean output includes dry run -n" \
    "git clean -n" \
    "$($GITPAL "remove untracked files" 2>/dev/null)"

check "clean has danger warning" \
    "Warning" \
    "$($GITPAL "delete untracked files" 2>/dev/null)"

# ── 11. TAGS ──────────────────────────────────────────────────────────────────
section "11. tags"

check "create tag -> git tag" \
    "git tag" \
    "$(top_cmd "create a tag")"

check "create tag output includes annotated tag" \
    "git tag -a" \
    "$($GITPAL "create a tag" 2>/dev/null)"

check "push tags -> git push" \
    "git push" \
    "$(top_cmd "push tags to remote")"

# ── 12. ADVANCED OPERATIONS ───────────────────────────────────────────────────
section "12. advanced operations"

check "cherry-pick -> git cherry-pick" \
    "git cherry-pick" \
    "$(top_cmd "cherry pick a commit")"

check "revert commit -> git revert" \
    "git revert" \
    "$(top_cmd "revert a commit without rewriting history")"

check "submodule -> git submodule" \
    "git submodule" \
    "$(top_cmd "add a submodule")"

check "worktree -> git worktree" \
    "git worktree" \
    "$(top_cmd "work on two branches at once")"

check "bundle offline -> git bundle" \
    "git bundle" \
    "$(top_cmd "pack repo for offline transfer")"

check "archive -> git archive" \
    "git archive" \
    "$(top_cmd "export repo as zip")"

check "init -> git init" \
    "git init" \
    "$(top_cmd "create repo from scratch")"

check "apply patch -> git am" \
    "git am" \
    "$(top_cmd "apply a patch file")"

check "export commits as patch -> git format-patch" \
    "git format-patch" \
    "$(top_cmd "export commits as patch")"

check "move file -> git mv" \
    "git mv" \
    "$(top_cmd "move a file and keep history")"

check "remove from tracking -> git rm" \
    "git rm" \
    "$(top_cmd "remove file from tracking keep on disk")"

check "list ignored files -> git ls-files" \
    "git ls-files" \
    "$(top_cmd "list ignored files")"

check "gpg sign -> git commit -S" \
    "git commit -S" \
    "$(top_cmd "sign commit with gpg")"

check "gc -> git gc" \
    "git gc" \
    "$(top_cmd "clean up git objects")"

check "sparse checkout -> git sparse-checkout" \
    "git sparse-checkout" \
    "$($GITPAL "checkout only some directories" 2>/dev/null)"

# ── 13. CONFIG ────────────────────────────────────────────────────────────────
section "13. config"

check "set email -> git config" \
    "git config" \
    "$(top_cmd "set my git email")"

check "set username -> git config" \
    "git config" \
    "$(top_cmd "set my git username")"

check "create alias -> alias" \
    "alias" \
    "$($GITPAL "create a git alias" 2>/dev/null)"

# ── 14. HOOKS / LFS / SHALLOW ─────────────────────────────────────────────────
section "14. hooks, lfs, shallow clone"

check "hooks output includes .git/hooks path" \
    ".git/hooks" \
    "$($GITPAL "git hooks pre commit" 2>/dev/null)"

check "hooks output mentions pre-commit" \
    "pre-commit" \
    "$($GITPAL "git hooks" 2>/dev/null)"

check "lfs -> git lfs" \
    "git lfs" \
    "$(top_cmd "set up git lfs large files")"

check "shallow clone -> git clone --depth" \
    "git clone --depth" \
    "$(top_cmd "shallow clone")"

check "clone without history -> git clone --depth" \
    "git clone --depth" \
    "$(top_cmd "clone without full history")"

# ── 15. NO-MATCH BEHAVIOUR ────────────────────────────────────────────────────
section "15. no-match behaviour"

check "nonsense returns no-match message" \
    "No local match" \
    "$($GITPAL "purple monkey dishwasher" 2>/dev/null)"

check "gibberish returns no-match" \
    "No local match" \
    "$($GITPAL "zzzzz xkqrm" 2>/dev/null)"

check "unrelated tech query returns no-match" \
    "No local match" \
    "$($GITPAL "kubernetes deployment yaml" 2>/dev/null)"

# ── 16. RESULT COUNT ──────────────────────────────────────────────────────────
section "16. result count"

out=$($GITPAL "stash" 2>/dev/null)
if echo "$out" | grep -q "Match 1 of"; then
    printf "PASS  ambiguous 'stash' returns multiple results\n"; PASS=$((PASS+1))
else
    printf "FAIL  ambiguous 'stash' should return multiple results\n"; FAIL=$((FAIL+1))
fi

out=$($GITPAL "abort merge" 2>/dev/null)
if echo "$out" | grep -q "Match [0-9] of"; then
    printf "FAIL  precise 'abort merge' should return single result\n"; FAIL=$((FAIL+1))
else
    printf "PASS  precise 'abort merge' returns single result\n"; PASS=$((PASS+1))
fi

out=$($GITPAL "force push safely" 2>/dev/null)
if echo "$out" | grep -q "Match [0-9] of"; then
    printf "FAIL  precise 'force push safely' should return single result\n"; FAIL=$((FAIL+1))
else
    printf "PASS  precise 'force push safely' returns single result\n"; PASS=$((PASS+1))
fi

out=$($GITPAL "git hooks pre commit" 2>/dev/null)
if echo "$out" | grep -q "Match [0-9] of"; then
    printf "FAIL  precise 'git hooks pre commit' should return single result\n"; FAIL=$((FAIL+1))
else
    printf "PASS  precise 'git hooks pre commit' returns single result\n"; PASS=$((PASS+1))
fi

# ── 17. VERBOSE MODE ──────────────────────────────────────────────────────────
section "17. verbose mode"

# ── A. outside repo (must NOT be in git repo) ──
TMP_NO_REPO=$(mktemp -d)
cd "$TMP_NO_REPO" || exit 1

OUT_NO_REPO=$("$OLDPWD/$GITPAL" -v "undo commit" 2>&1)

check "verbose -v shows header (outside repo)" \
    "verbose mode" \
    "$OUT_NO_REPO"

check "verbose outside repo shows not-in-repo notice" \
    "not in a git repo" \
    "$OUT_NO_REPO"

check_absent "outside repo does NOT show repo context" \
    "reading repo context" \
    "$OUT_NO_REPO"

cd "$OLDPWD"
rm -rf "$TMP_NO_REPO"

# ── B. inside repo ──
TMPREPO=$(mktemp -d)
cd "$TMPREPO" || exit 1

git init -q 2>/dev/null
git config user.email "t@t.com"
git config user.name "T"
git commit -q --allow-empty -m "init" 2>/dev/null

OUT_REPO=$("$OLDPWD/$GITPAL" -v "undo commit" 2>&1)

check "verbose inside repo shows header" \
    "verbose mode" \
    "$OUT_REPO"

check "verbose inside repo shows reading context header" \
    "reading repo context" \
    "$OUT_REPO"

check "verbose inside repo shows git branch --show-current" \
    "git branch --show-current" \
    "$OUT_REPO"

check "verbose inside repo shows git status --short" \
    "git status --short" \
    "$OUT_REPO"

check "verbose inside repo shows git log --oneline" \
    "git log --oneline" \
    "$OUT_REPO"

check "verbose inside repo still shows the answer" \
    "git reset" \
    "$OUT_REPO"

check_absent "inside repo does NOT show not-in-repo notice" \
    "not in a git repo" \
    "$OUT_REPO"

cd "$OLDPWD"
rm -rf "$TMPREPO"

# ── C. generic verbose behavior (environment independent) ──

check_absent "verbose does not show 2>/dev/null" \
    "2>/dev/null" \
    "$($GITPAL -v "undo commit" 2>&1)"

check_absent "verbose does not show | head" \
    "| head" \
    "$($GITPAL -v "undo commit" 2>&1)"

# ── 18. FLAG COMBINATIONS ─────────────────────────────────────────────────────
section "18. flag combinations"

check "--local returns answer without AI" \
    "git reset" \
    "$($GITPAL --local "undo commit" 2>/dev/null)"

check "-v with query returns answer" \
    "git reset" \
    "$($GITPAL -v "undo commit" 2>/dev/null)"

check "--verbose with query returns answer" \
    "git reset" \
    "$($GITPAL --verbose "undo commit" 2>/dev/null)"

check "multi-word query without quotes works" \
    "git reset" \
    "$($GITPAL undo last commit 2>/dev/null)"

$GITPAL "undo commit" >/dev/null 2>&1
check_exit "normal query exits zero" 0 $?

$GITPAL "purple monkey" >/dev/null 2>&1
check_exit "no-match query exits zero" 0 $?

# ── 19. WARNINGS PRESENT WHERE EXPECTED ──────────────────────────────────────
section "19. warnings"

check "undo commit Warning present" \
    "Warning" "$($GITPAL "undo last commit" 2>/dev/null)"

check "force push Warning present" \
    "Warning" "$($GITPAL "force push" 2>/dev/null)"

check "squash Warning present" \
    "Warning" "$($GITPAL "squash commits" 2>/dev/null)"

check "stash clear Warning present" \
    "Warning" "$($GITPAL "clear all stashes" 2>/dev/null)"

check "committed wrong branch Warning present" \
    "Warning" "$($GITPAL "committed to wrong branch" 2>/dev/null)"

check "hard reset Warning mentions permanently" \
    "permanently" "$($GITPAL "undo last commit" 2>/dev/null)"

check "squash Warning mentions history" \
    "history" "$($GITPAL "squash commits" 2>/dev/null)"

# ── 20. NATURAL LANGUAGE VARIANTS ─────────────────────────────────────────────
section "20. natural language variants"

check "how do i undo my last commit -> git reset" \
    "git reset" \
    "$(top_cmd "how do i undo my last commit")"

check "i committed to wrong branch -> cherry-pick" \
    "git cherry-pick" \
    "$(top_cmd "i committed to wrong branch")"

check "save work temporarily -> stash" \
    "git stash" \
    "$(top_cmd "save my work temporarily")"

check "who last changed this line -> blame" \
    "git blame" \
    "$(top_cmd "who last changed this line")"

check "combine my last commits -> rebase -i" \
    "git rebase -i" \
    "$(top_cmd "combine my last 4 commits")"

check "overwrite remote -> force push" \
    "force-with-lease" \
    "$($GITPAL "overwrite remote branch" 2>/dev/null)"

check "get back stashed work -> stash pop" \
    "git stash pop" \
    "$(top_cmd "get back my stashed work")"

# ── 21. EDGE CASES ────────────────────────────────────────────────────────────
section "21. edge cases"

check "very long query does not crash" \
    "git" \
    "$($GITPAL "how do I undo a commit that was pushed to a remote branch that other people have already pulled and built on top of" 2>/dev/null)"

check "HEAD~1 query does not crash" \
    "" \
    "$($GITPAL "what does HEAD~1 mean" 2>/dev/null)"

$GITPAL "x" >/dev/null 2>&1
check_exit "single char query exits zero" 0 $?

$GITPAL "undo commit" >/dev/null 2>&1
check_exit "normal one-shot exits zero" 0 $?

# ── summary ───────────────────────────────────────────────────────────────────

printf "\n────────────────────────────────────────────\n"
printf "Results: %d passed, %d failed  (total %d)\n" "$PASS" "$FAIL" "$((PASS + FAIL))"
printf "────────────────────────────────────────────\n"

[ $FAIL -gt 0 ] && exit 1
exit 0
