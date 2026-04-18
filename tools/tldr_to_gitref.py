#!/usr/bin/env python3
"""
tools/tldr_to_gitref.py
-----------------------
Converts tldr-style git pages into gitref.h C entries.

Usage:
    # From a single combined file (our bundled pages):
    python3 tools/tldr_to_gitref.py tools/tldr/git-pages.md > /tmp/generated_entries.h

    # From a directory of individual tldr .md files:
    python3 tools/tldr_to_gitref.py --dir /path/to/tldr/pages/common/ > /tmp/generated_entries.h

    # Regenerate gitref.h fully (replaces the GENERATED BLOCK):
    python3 tools/tldr_to_gitref.py tools/tldr/git-pages.md --update src/gitref.h
    
    # Recommended: regenerate from real tldr-pages
    git clone --depth 1 https://github.com/tldr-pages/tldr.git /tmp/tldr
    python3 tools/tldr_to_gitref.py --dir /tmp/tldr/pages/common/ --update src/gitref.h
    make clean && make

The script only emits entries for git-* pages (skips non-git content).
Hand-written entries in gitref.h are preserved in the HAND-WRITTEN BLOCK.

Output: C struct initialisers ready to paste or auto-inject into GIT_DB[].
"""

import re
import sys
import os
import argparse
from dataclasses import dataclass, field
from typing import Optional

# ── danger keywords that trigger the warning field ──────────────────────────
DANGER_PATTERNS = [
    r"--hard", r"--force(?!-with-lease)", r"filter-branch",
    r"reset --hard", r"clean -f", r"push --force[^-]",
    r"stash clear", r"reflog expire", r"rm -rf",
]

DANGER_RE = re.compile("|".join(DANGER_PATTERNS))


@dataclass
class TldrEntry:
    subcommand: str          # e.g. "git-add"
    description: str         # first > line
    examples: list           # list of (description, command) tuples


def danger_warning(commands: list[str]) -> Optional[str]:
    """Return a warning string if any command looks dangerous."""
    for cmd in commands:
        if DANGER_RE.search(cmd):
            if "--hard" in cmd:
                return "Discards changes permanently and cannot be undone."
            if "filter-branch" in cmd:
                return "Rewrites entire history — back up your repo first."
            if "--force" in cmd and "with-lease" not in cmd:
                return "Force pushes overwrite remote history — coordinate with your team."
            if "stash clear" in cmd:
                return "Deletes all stashes permanently."
            if "clean -f" in cmd or "rm -rf" in cmd:
                return "Deletes files permanently — run with -n first to preview."
            return "This command can cause data loss — double-check before running."
    return None


def clean_cmd(cmd: str) -> str:
    """Strip tldr {{placeholder}} markers, keep the structure."""
    cmd = cmd.strip().lstrip("`").rstrip("`")
    # Replace {{placeholder}} with <placeholder>
    cmd = re.sub(r"\{\{([^}]+)\}\}", lambda m: f"<{m.group(1)}>", cmd)
    return cmd


def derive_keywords(subcommand: str, description: str, examples: list) -> list[str]:
    """
    Derive up to 7 keyword phrases from the entry.
    Only uses multi-word phrases — bare single verbs are owned by hand-written
    entries and must not collide with generated ones.
    """
    keywords = set()

    bare = subcommand.replace("git-", "")
    # Add "git <subcommand>" as a multi-word phrase (safe)
    if bare != "git" and " " not in bare:
        keywords.add(f"git {bare}")

    # Generic single-word verbs that cause false positives — skip as standalone keywords
    SKIP_FIRST_WORD = {
        "add","commit","push","pull","merge","rebase","stash","branch","tag",
        "clone","fetch","log","diff","show","reset","clean","init","remote",
        "config","apply","restore","switch","checkout","rm","mv","list","create",
        "delete","remove","run","set","get","show","view","check",
    }

    for (desc, _cmd) in examples[:8]:
        desc_clean = desc.strip().rstrip(":").lower()
        desc_clean = re.sub(r"^(a |an |the |all |only |just )", "", desc_clean)
        words = desc_clean.split()
        # Must be multi-word, meaningful length, first word not a generic verb
        if len(words) >= 2 and 8 < len(desc_clean) < 55:
            if words[0] not in SKIP_FIRST_WORD:
                keywords.add(desc_clean)

    result = sorted(keywords, key=len)[:7]
    return result


def c_escape(s: str) -> str:
    """Escape a string for use in a C string literal."""
    s = s.replace("\\", "\\\\")
    s = s.replace('"', '\\"')
    s = s.replace("\n", "\\n")
    s = s.replace("'", "\'")
    # Remove stash@{N} style refs — replace with plain description
    import re as _re
    s = _re.sub(r"stash@\{[^}]*\}", "stash@{N}", s)
    s = _re.sub(r"HEAD@\{[^}]*\}", "HEAD@{N}", s)
    return s


def escape_c_token(s: str) -> str:
    """Escape a single token (no embedded newlines) for a C string literal."""
    import re as _re
    s = s.replace("\\", "\\\\")
    s = s.replace('"', '\\"'  )
    s = s.replace("'", "\\'")
    # normalise stash/reflog refs
    s = _re.sub(r"stash@\{[^}]*\}", "stash@{N}", s)
    s = _re.sub(r"HEAD@\{[^}]*\}", "HEAD@{N}", s)
    return s

def entry_to_c(entry: TldrEntry) -> str:
    """Render a TldrEntry as a C struct initialiser."""
    if not entry.examples:
        return ""

    commands = [clean_cmd(cmd) for (_, cmd) in entry.examples]
    keywords = derive_keywords(entry.subcommand, entry.description, entry.examples)

    if not keywords:
        return ""

    # Command string: escape each line individually, join with literal \n
    cmd_str = "\\n".join(escape_c_token(c) for c in commands[:4])

    # Explanation: one line per example
    expl_parts = []
    for (desc, cmd) in entry.examples[:6]:
        part = f"  {desc.strip().rstrip(':')}: {clean_cmd(cmd)}"
        expl_parts.append(escape_c_token(part))
    expl_str = "\\n".join(expl_parts)

    warning = danger_warning(commands)

    kw_parts = ", ".join(f'"{escape_c_token(k)}"' for k in keywords[:7])
    kw_str   = "{" + kw_parts + ", NULL}"

    out  = f"    /* {entry.subcommand} */\n"
    out += f"    {{\n"
    out += f"        {kw_str},\n"
    out += f"        \"{cmd_str}\",\n"
    out += f"        \"{expl_str}\",\n"
    if warning:
        out += f"        \"{escape_c_token(warning)}\"\n"
    else:
        out += f"        NULL\n"
    out += f"    }},"
    return out


def parse_combined_file(path: str) -> list[TldrEntry]:
    """Parse our combined git-pages.md file (sections separated by ---)."""
    entries = []
    with open(path) as f:
        content = f.read()

    sections = re.split(r"\n---\n", content)
    for section in sections:
        entry = parse_tldr_section(section.strip())
        if entry:
            entries.append(entry)
    return entries


def parse_tldr_file(path: str) -> Optional["TldrEntry"]:
    """Parse a single tldr .md file."""
    with open(path) as f:
        return parse_tldr_section(f.read())


def parse_tldr_section(text: str) -> Optional[TldrEntry]:
    """Parse a single tldr page section into a TldrEntry."""
    lines = text.strip().splitlines()
    if not lines:
        return None

    # First line: # git-add
    header = lines[0].strip()
    if not header.startswith("#"):
        return None
    subcommand = header.lstrip("# ").strip()
    if not subcommand.startswith("git"):
        return None

    # Description: first > line
    description = ""
    for line in lines[1:]:
        line = line.strip()
        if line.startswith(">") and not line.startswith("> More"):
            description = line.lstrip("> ").strip()
            break

        # Examples: pairs of "- description:" and "  `command`"
    examples = []
    i = 0
    n = len(lines)

    while i < n:
        line = lines[i].strip()

        if re.match(r"[-*]\s+", line):
            desc = re.sub(r"^[-*]\s+", "", line).rstrip(":")

            for j in range(1, 4):
                if i + j < n:
                    cmd_line = lines[i + j].strip()

                    m = cmd_line.find("`")
                    if m != -1:
                        m2 = cmd_line.find("`", m + 1)
                        if m2 != -1:
                            examples.append((desc, cmd_line[m+1:m2]))
                            break

        i += 1

    if not examples:
     return None

    return TldrEntry(subcommand=subcommand, description=description, examples=examples)


def parse_tldr_dir(dirpath: str) -> list[TldrEntry]:
    """Parse all git-*.md files in a directory."""
    entries = []

    files = [
        f for f in os.listdir(dirpath)
        if f.startswith("git") and f.endswith(".md")
    ]

    print("Git files:", len(files))  # debug

    for fname in sorted(files):
        path = os.path.join(dirpath, fname)
        entry = parse_tldr_file(path)
        if entry:
            entries.append(entry)

    return entries


GENERATED_BEGIN = "    /* ── BEGIN GENERATED FROM TLDR ──────────────────────────────────────── */"
GENERATED_END   = "    /* ── END GENERATED FROM TLDR ────────────────────────────────────────── */"


def update_gitref(entries: list[TldrEntry], gitref_path: str):
    """Write generated entries into gitref.h GENERATED BLOCK.
    
    Writes each entry line-by-line to avoid any Python string escape
    interpretation during f-string assembly.
    """
    import io, re

    with open(gitref_path) as f:
        original = f.read()

    # Build block by writing each entry to a buffer line by line
    buf = io.StringIO()
    buf.write(GENERATED_BEGIN + "\n")
    for e in entries:
        c = entry_to_c(e)
        if c:
            buf.write(c)
            buf.write("\n\n")
    buf.write(GENERATED_END)
    block = buf.getvalue()

    if GENERATED_BEGIN in original:
        pattern = re.escape(GENERATED_BEGIN) + r".*?" + re.escape(GENERATED_END)
        updated = re.sub(pattern, lambda m: block, original, flags=re.DOTALL)
    else:
        sentinel = "    /* sentinel */"
        updated = original.replace(sentinel, block + "\n\n    /* sentinel */")

    with open(gitref_path, "w") as f:
        f.write(updated)

    print(f"Updated {gitref_path}: {len(entries)} entries injected.", file=sys.stderr)


def main():
    ap = argparse.ArgumentParser(description="Convert tldr git pages to gitref.h entries")
    ap.add_argument("source", help="Combined .md file or directory of tldr pages")
    ap.add_argument("--dir", action="store_true", help="Source is a directory of .md files")
    ap.add_argument("--update", metavar="GITREF_H", help="Inject into gitref.h instead of stdout")
    args = ap.parse_args()

    if args.dir or os.path.isdir(args.source):
        entries = parse_tldr_dir(args.source)
    else:
        entries = parse_combined_file(args.source)

    if not entries:
        print("No entries parsed.", file=sys.stderr)
        sys.exit(1)

    print(f"Parsed {len(entries)} entries.", file=sys.stderr)

    if args.update:
        update_gitref(entries, args.update)
    else:
        for entry in entries:
            c = entry_to_c(entry)
            if c:
                print(c)
                print()


if __name__ == "__main__":
    main()
