# git-add

> Add file contents to the index (staging area).
> More information: <https://git-scm.com/docs/git-add>.

- Add a file to the index:
  `git add {{path/to/file}}`

- Add all files (tracked and untracked):
  `git add -A`

- Add all files in current directory:
  `git add .`

- Add only already-tracked files:
  `git add -u`

- Interactively stage parts of files (hunks):
  `git add -p`

- Interactively stage parts of a specific file:
  `git add -p {{path/to/file}}`

- Interactively add untracked files:
  `git add -i`

---
# git-blame

> Show who last changed each line of a file.
> More information: <https://git-scm.com/docs/git-blame>.

- Show blame for a file:
  `git blame {{path/to/file}}`

- Show blame for specific line range:
  `git blame -L {{10}},{{20}} {{path/to/file}}`

- Show blame ignoring whitespace changes:
  `git blame -w {{path/to/file}}`

- Show blame with abbreviated commit hashes:
  `git blame --abbrev={{4}} {{path/to/file}}`

- Show blame at a specific commit or tag:
  `git blame {{commit_or_tag}} -- {{path/to/file}}`

---
# git-bisect

> Find the commit that introduced a bug using binary search.
> More information: <https://git-scm.com/docs/git-bisect>.

- Start bisect session:
  `git bisect start`

- Mark current commit as bad (has the bug):
  `git bisect bad`

- Mark a known good commit:
  `git bisect good {{commit_hash}}`

- Mark current commit as good after testing:
  `git bisect good`

- Mark current commit as bad after testing:
  `git bisect bad`

- End bisect session and return to original branch:
  `git bisect reset`

- Run automated test script on each commit:
  `git bisect run {{./test_script.sh}}`

- View bisect log:
  `git bisect log`

---
# git-branch

> Manage branches.
> More information: <https://git-scm.com/docs/git-branch>.

- List all local branches:
  `git branch`

- List all branches (local and remote):
  `git branch -a`

- List remote branches only:
  `git branch -r`

- Create a new branch:
  `git branch {{branch_name}}`

- Delete a merged branch:
  `git branch -d {{branch_name}}`

- Force delete a branch:
  `git branch -D {{branch_name}}`

- Rename current branch:
  `git branch -m {{new_name}}`

- Rename a specific branch:
  `git branch -m {{old_name}} {{new_name}}`

- Set upstream tracking branch:
  `git branch --set-upstream-to=origin/{{branch_name}}`

- List branches with last commit info:
  `git branch -v`

- List merged branches:
  `git branch --merged`

- List unmerged branches:
  `git branch --no-merged`

---
# git-bundle

> Pack repository data into a single file for offline transfer.
> More information: <https://git-scm.com/docs/git-bundle>.

- Create a bundle of the entire repo:
  `git bundle create {{repo.bundle}} --all`

- Create a bundle of a specific branch:
  `git bundle create {{branch.bundle}} {{branch_name}}`

- Verify a bundle:
  `git bundle verify {{repo.bundle}}`

- List branches in a bundle:
  `git bundle list-heads {{repo.bundle}}`

- Clone from a bundle:
  `git clone {{repo.bundle}} {{directory}}`

---
# git-checkout

> Switch branches or restore files. Prefer git-switch and git-restore for clarity.
> More information: <https://git-scm.com/docs/git-checkout>.

- Switch to a branch:
  `git checkout {{branch_name}}`

- Create and switch to a new branch:
  `git checkout -b {{new_branch}}`

- Create branch from a specific commit:
  `git checkout -b {{new_branch}} {{commit_hash}}`

- Restore a file to its last committed state:
  `git checkout -- {{path/to/file}}`

- Switch to previous branch:
  `git checkout -`

- Check out a file from another branch:
  `git checkout {{other_branch}} -- {{path/to/file}}`

---
# git-cherry-pick

> Apply commits from one branch onto another.
> More information: <https://git-scm.com/docs/git-cherry-pick>.

- Apply a specific commit to current branch:
  `git cherry-pick {{commit_hash}}`

- Apply a range of commits:
  `git cherry-pick {{start_hash}}..{{end_hash}}`

- Apply commit without committing (stage only):
  `git cherry-pick -n {{commit_hash}}`

- Continue after resolving conflict:
  `git cherry-pick --continue`

- Abort cherry-pick:
  `git cherry-pick --abort`

---
# git-clean

> Remove untracked files and directories.
> More information: <https://git-scm.com/docs/git-clean>.

- Dry run — show what would be deleted:
  `git clean -n`

- Delete untracked files:
  `git clean -f`

- Delete untracked files and directories:
  `git clean -fd`

- Delete untracked and ignored files:
  `git clean -fdx`

- Interactively choose what to delete:
  `git clean -i`

---
# git-clone

> Clone a repository.
> More information: <https://git-scm.com/docs/git-clone>.

- Clone a repository:
  `git clone {{url}}`

- Clone into a specific directory:
  `git clone {{url}} {{directory}}`

- Clone a single branch only:
  `git clone --single-branch --branch {{branch_name}} {{url}}`

- Shallow clone (latest commit only):
  `git clone --depth 1 {{url}}`

- Clone including all submodules:
  `git clone --recurse-submodules {{url}}`

- Clone a local repository:
  `git clone {{path/to/local/repo}}`

---
# git-commit

> Record changes to the repository.
> More information: <https://git-scm.com/docs/git-commit>.

- Commit staged changes with a message:
  `git commit -m "{{message}}"`

- Commit all tracked modified files (skip git add):
  `git commit -am "{{message}}"`

- Amend the last commit:
  `git commit --amend`

- Amend without changing the message:
  `git commit --amend --no-edit`

- Create an empty commit (e.g. to trigger CI):
  `git commit --allow-empty -m "{{message}}"`

- Sign a commit with GPG:
  `git commit -S -m "{{message}}"`

---
# git-config

> Get and set repository or global options.
> More information: <https://git-scm.com/docs/git-config>.

- Set global username:
  `git config --global user.name "{{name}}"`

- Set global email:
  `git config --global user.email "{{email}}"`

- Set default editor:
  `git config --global core.editor "{{vim}}"`

- Set default branch name:
  `git config --global init.defaultBranch main`

- List all config:
  `git config --list`

- Create a command alias:
  `git config --global alias.{{st}} {{status}}`

- Set pull to rebase by default:
  `git config --global pull.rebase true`

- Enable color output:
  `git config --global color.ui auto`

- Show a specific config value:
  `git config user.email`

---
# git-describe

> Give a human-readable name to a commit based on the nearest tag.
> More information: <https://git-scm.com/docs/git-describe>.

- Describe current commit:
  `git describe`

- Describe including lightweight tags:
  `git describe --tags`

- Always output something even if no tag found:
  `git describe --always`

- Describe a specific commit:
  `git describe {{commit_hash}}`

---
# git-diff

> Show changes between commits, branches, files, and working tree.
> More information: <https://git-scm.com/docs/git-diff>.

- Show unstaged changes:
  `git diff`

- Show staged changes:
  `git diff --staged`

- Show changes in last commit:
  `git diff HEAD~1`

- Show diff between two branches:
  `git diff {{branch1}}..{{branch2}}`

- Show only filenames that changed:
  `git diff --name-only`

- Show word-level diff:
  `git diff --word-diff`

- Show diff for a specific file:
  `git diff -- {{path/to/file}}`

- Show diff ignoring whitespace:
  `git diff -w`

---
# git-fetch

> Download objects and refs from remote.
> More information: <https://git-scm.com/docs/git-fetch>.

- Fetch from default remote (origin):
  `git fetch`

- Fetch and remove stale remote-tracking branches:
  `git fetch --prune`

- Fetch all remotes:
  `git fetch --all`

- Fetch a specific branch:
  `git fetch origin {{branch_name}}`

- Fetch tags only:
  `git fetch --tags`

---
# git-format-patch

> Export commits as patch files for email-based workflows.
> More information: <https://git-scm.com/docs/git-format-patch>.

- Export last commit as patch:
  `git format-patch -1 HEAD`

- Export all commits not in main:
  `git format-patch origin/main`

- Export to a specific directory:
  `git format-patch -o {{/path/to/patches/}} origin/main`

- Apply a patch:
  `git am {{patch_file.patch}}`

---
# git-gc

> Cleanup unnecessary files and optimize the local repository.
> More information: <https://git-scm.com/docs/git-gc>.

- Run garbage collection:
  `git gc`

- Run aggressively (slower, more thorough):
  `git gc --aggressive`

- Run without waiting for lock:
  `git gc --auto`

---
# git-grep

> Search for a pattern in tracked files.
> More information: <https://git-scm.com/docs/git-grep>.

- Search for a string in all tracked files:
  `git grep "{{search_string}}"`

- Search case-insensitively:
  `git grep -i "{{search_string}}"`

- Search and show line numbers:
  `git grep -n "{{search_string}}"`

- Search only in specific files:
  `git grep "{{search_string}}" -- "{{*.c}}"`

- Count matches per file:
  `git grep -c "{{search_string}}"`

- Search in a specific commit or branch:
  `git grep "{{search_string}}" {{branch_or_hash}}`

---
# git-init

> Initialize a new git repository.
> More information: <https://git-scm.com/docs/git-init>.

- Initialize a repository in current directory:
  `git init`

- Initialize with a specific branch name:
  `git init -b main`

- Initialize a bare repository (for servers):
  `git init --bare`

---
# git-log

> Show commit history.
> More information: <https://git-scm.com/docs/git-log>.

- Show log in one line per commit:
  `git log --oneline`

- Show visual branch graph:
  `git log --oneline --graph --decorate --all`

- Show last N commits:
  `git log -{{10}}`

- Show commits by a specific author:
  `git log --author="{{name}}"`

- Show commits touching a file:
  `git log -- {{path/to/file}}`

- Search commit messages:
  `git log --grep="{{keyword}}"`

- Show commits between dates:
  `git log --after="{{2024-01-01}}" --before="{{2024-12-31}}"`

- Show commits that added/removed a string:
  `git log -S "{{search_string}}" --oneline`

- Show diff for each commit:
  `git log -p`

---
# git-ls-files

> Show information about files in the index and working tree.
> More information: <https://git-scm.com/docs/git-ls-files>.

- List all tracked files:
  `git ls-files`

- List untracked files:
  `git ls-files --others --exclude-standard`

- List ignored files:
  `git ls-files --ignored --exclude-standard`

- List deleted files:
  `git ls-files --deleted`

---
# git-merge

> Merge branches.
> More information: <https://git-scm.com/docs/git-merge>.

- Merge a branch into current branch:
  `git merge {{branch_name}}`

- Merge without fast-forward (always creates merge commit):
  `git merge --no-ff {{branch_name}}`

- Merge with squash (stages all changes, no commit):
  `git merge --squash {{branch_name}}`

- Abort an in-progress merge:
  `git merge --abort`

- Continue after resolving conflicts:
  `git merge --continue`

---
# git-mv

> Move or rename a file, directory, or symlink.
> More information: <https://git-scm.com/docs/git-mv>.

- Rename a file:
  `git mv {{old_name}} {{new_name}}`

- Move a file to a directory:
  `git mv {{path/to/file}} {{path/to/directory}}`

- Force move even if destination exists:
  `git mv -f {{source}} {{destination}}`

---
# git-notes

> Add or inspect object notes.
> More information: <https://git-scm.com/docs/git-notes>.

- Add a note to a commit:
  `git notes add -m "{{note}}" {{commit_hash}}`

- Show notes for a commit:
  `git notes show {{commit_hash}}`

- Edit a note:
  `git notes edit {{commit_hash}}`

- Remove a note:
  `git notes remove {{commit_hash}}`

- List all notes:
  `git log --show-notes`

---
# git-pull

> Fetch and integrate changes from a remote.
> More information: <https://git-scm.com/docs/git-pull>.

- Pull and rebase instead of merge:
  `git pull --rebase`

- Pull from a specific remote and branch:
  `git pull {{remote}} {{branch}}`

- Pull with fast-forward only:
  `git pull --ff-only`

- Pull all remotes:
  `git pull --all`

---
# git-push

> Update remote refs.
> More information: <https://git-scm.com/docs/git-push>.

- Push to default remote:
  `git push`

- Push and set upstream tracking:
  `git push -u origin {{branch_name}}`

- Force push safely (fails if remote has unseen commits):
  `git push --force-with-lease`

- Push all tags:
  `git push --tags`

- Delete a remote branch:
  `git push origin --delete {{branch_name}}`

- Push a specific tag:
  `git push origin {{tag_name}}`

---
# git-rebase

> Reapply commits on top of another base tip.
> More information: <https://git-scm.com/docs/git-rebase>.

- Rebase current branch onto another:
  `git rebase {{base_branch}}`

- Interactive rebase for last N commits:
  `git rebase -i HEAD~{{N}}`

- Continue after resolving conflict:
  `git rebase --continue`

- Abort rebase:
  `git rebase --abort`

- Skip current commit during rebase:
  `git rebase --skip`

- Rebase onto a specific commit:
  `git rebase --onto {{new_base}} {{old_base}} {{branch}}`

---
# git-reflog

> Show history of HEAD movements. Use to recover lost commits.
> More information: <https://git-scm.com/docs/git-reflog>.

- Show reflog:
  `git reflog`

- Show reflog for a specific branch:
  `git reflog {{branch_name}}`

- Recover to a previous HEAD position:
  `git reset --hard HEAD@{{{2}}}`

- Show reflog with dates:
  `git reflog --date=iso`

- Show reflog for stash:
  `git reflog stash`

---
# git-remote

> Manage remote connections.
> More information: <https://git-scm.com/docs/git-remote>.

- List all remotes:
  `git remote -v`

- Add a remote:
  `git remote add {{name}} {{url}}`

- Remove a remote:
  `git remote remove {{name}}`

- Rename a remote:
  `git remote rename {{old_name}} {{new_name}}`

- Change remote URL:
  `git remote set-url {{name}} {{new_url}}`

- Show remote info:
  `git remote show {{origin}}`

---
# git-reset

> Reset HEAD to a previous state.
> More information: <https://git-scm.com/docs/git-reset>.

- Undo last commit, keep changes staged:
  `git reset --soft HEAD~1`

- Undo last commit, keep changes unstaged:
  `git reset HEAD~1`

- Undo last commit, discard all changes:
  `git reset --hard HEAD~1`

- Unstage a file:
  `git reset HEAD {{path/to/file}}`

- Reset to a specific commit:
  `git reset --hard {{commit_hash}}`

- Undo last N commits:
  `git reset --soft HEAD~{{N}}`

---
# git-restore

> Restore working tree files.
> More information: <https://git-scm.com/docs/git-restore>.

- Discard unstaged changes to a file:
  `git restore {{path/to/file}}`

- Unstage a file:
  `git restore --staged {{path/to/file}}`

- Restore a file from a specific commit:
  `git restore --source={{commit_hash}} {{path/to/file}}`

- Discard all unstaged changes:
  `git restore .`

---
# git-revert

> Create a new commit that undoes a previous commit.
> More information: <https://git-scm.com/docs/git-revert>.

- Revert a specific commit (creates a new commit):
  `git revert {{commit_hash}}`

- Revert without auto-committing:
  `git revert -n {{commit_hash}}`

- Revert a merge commit:
  `git revert -m 1 {{merge_commit_hash}}`

---
# git-rm

> Remove files from the index and working tree.
> More information: <https://git-scm.com/docs/git-rm>.

- Remove a file from tracking and disk:
  `git rm {{path/to/file}}`

- Remove from tracking only (keep on disk):
  `git rm --cached {{path/to/file}}`

- Remove a directory recursively:
  `git rm -r {{path/to/directory}}`

---
# git-shortlog

> Summarize git log output by author.
> More information: <https://git-scm.com/docs/git-shortlog>.

- Summarize commits by author:
  `git shortlog`

- Show commit count per author sorted:
  `git shortlog -sn`

- Summarize a specific branch:
  `git shortlog {{branch_name}}`

---
# git-show

> Show information about git objects.
> More information: <https://git-scm.com/docs/git-show>.

- Show last commit with diff:
  `git show`

- Show a specific commit:
  `git show {{commit_hash}}`

- Show only files changed in last commit:
  `git show --stat`

- Show a file at a specific commit:
  `git show {{commit_hash}}:{{path/to/file}}`

- Show a tag:
  `git show {{tag_name}}`

---
# git-stash

> Stash changes in working directory.
> More information: <https://git-scm.com/docs/git-stash>.

- Stash current changes:
  `git stash push -m "{{description}}"`

- Stash including untracked files:
  `git stash push -u`

- List all stashes:
  `git stash list`

- Apply most recent stash:
  `git stash pop`

- Apply a specific stash without removing it:
  `git stash apply stash@{{{2}}}`

- Drop a specific stash:
  `git stash drop stash@{{{0}}}`

- Clear all stashes:
  `git stash clear`

- Show diff of a stash:
  `git stash show -p stash@{{{0}}}`

- Stash only staged changes:
  `git stash push --staged`

---
# git-submodule

> Manage nested repositories.
> More information: <https://git-scm.com/docs/git-submodule>.

- Add a submodule:
  `git submodule add {{url}} {{path}}`

- Initialize and clone all submodules after clone:
  `git submodule update --init --recursive`

- Pull latest for all submodules:
  `git submodule foreach git pull origin main`

- Remove a submodule:
  `git submodule deinit {{path}} && git rm {{path}}`

- Show submodule status:
  `git submodule status`

---
# git-switch

> Switch branches (modern replacement for git checkout).
> More information: <https://git-scm.com/docs/git-switch>.

- Switch to an existing branch:
  `git switch {{branch_name}}`

- Create and switch to a new branch:
  `git switch -c {{new_branch}}`

- Switch to previous branch:
  `git switch -`

- Create branch from a specific commit:
  `git switch -c {{new_branch}} {{commit_hash}}`

---
# git-tag

> Create, list, and delete tags.
> More information: <https://git-scm.com/docs/git-tag>.

- List all tags:
  `git tag`

- Create a lightweight tag:
  `git tag {{tag_name}}`

- Create an annotated tag:
  `git tag -a {{tag_name}} -m "{{message}}"`

- Delete a local tag:
  `git tag -d {{tag_name}}`

- Push all tags to remote:
  `git push origin --tags`

- Push a specific tag:
  `git push origin {{tag_name}}`

- Delete a remote tag:
  `git push origin --delete {{tag_name}}`

- Tag a specific commit:
  `git tag {{tag_name}} {{commit_hash}}`

---
# git-worktree

> Manage multiple working trees from one repository.
> More information: <https://git-scm.com/docs/git-worktree>.

- Add a new working tree for a branch:
  `git worktree add {{path}} {{branch_name}}`

- List all working trees:
  `git worktree list`

- Remove a working tree:
  `git worktree remove {{path}}`

- Add working tree for a new branch:
  `git worktree add -b {{new_branch}} {{path}}`

---
# git-apply

> Apply a patch to files.
> More information: <https://git-scm.com/docs/git-apply>.

- Apply a patch file:
  `git apply {{patch_file.patch}}`

- Check if patch applies cleanly:
  `git apply --check {{patch_file.patch}}`

- Apply in reverse:
  `git apply --reverse {{patch_file.patch}}`

---
# git-filter-branch

> Rewrite history by filtering branches.
> More information: <https://git-scm.com/docs/git-filter-branch>.

- Remove a file from entire history:
  `git filter-branch --tree-filter 'rm -f {{path/to/file}}' HEAD`

- Remove a directory from entire history:
  `git filter-branch --tree-filter 'rm -rf {{path/to/dir}}' HEAD`

---
# git-sparse-checkout

> Reduce working tree to a subset of tracked files.
> More information: <https://git-scm.com/docs/git-sparse-checkout>.

- Initialize sparse checkout:
  `git sparse-checkout init --cone`

- Set directories to include:
  `git sparse-checkout set {{src/}} {{docs/}}`

- List current sparse patterns:
  `git sparse-checkout list`

- Disable sparse checkout:
  `git sparse-checkout disable`

---
# git-archive

> Create an archive of files from a tree.
> More information: <https://git-scm.com/docs/git-archive>.

- Create a tar.gz of HEAD:
  `git archive --format=tar.gz --output={{output.tar.gz}} HEAD`

- Create a zip of a specific branch:
  `git archive --format=zip {{branch_name}} > {{output.zip}}`

- Archive a subdirectory only:
  `git archive HEAD:{{path/to/dir}} > {{output.tar}}`
