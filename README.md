# 🐧 Mini `ls` — Operating Systems Assignment

> **Course:** Operating Systems  
> **Repository:** [BSDSF24M048-OS-A02](https://github.com/subhansaafir/BSDSF24M048-OS-A02)  
> **Language:** C  
> **Environment:** Linux / WSL  
> **Final version:** `v1.6.0`  
> **Project goal:** Build a small version of the Linux `ls` command and understand how a program interacts with the operating system.

---

## 📚 Table of Contents

- [1. Project Overview](#1-project-overview)
- [2. Learning Objectives](#2-learning-objectives)
- [3. Project Structure](#3-project-structure)
- [4. Tools and Basic Workflow](#4-tools-and-basic-workflow)
- [5. Version History at a Glance](#5-version-history-at-a-glance)
- [6. Feature 1 — Long Listing (`-l`), v1.1.0](#6-feature-1--long-listing--l-v110)
- [7. Feature 2 — Column Display, v1.2.0](#7-feature-2--column-display-v120)
- [8. Feature 3 — Horizontal Display (`-x`), v1.3.0](#8-feature-3--horizontal-display--x-v130)
- [9. Feature 4 — Alphabetical Sorting, v1.4.0](#9-feature-4--alphabetical-sorting-v140)
- [10. Feature 5 — Colorized Output, v1.5.0](#10-feature-5--colorized-output-v150)
- [11. Feature 6 — Recursive Listing (`-R`), v1.6.0](#11-feature-6--recursive-listing--r-v160)
- [12. Important C and Linux Concepts](#12-important-c-and-linux-concepts)
- [13. Git and GitHub Workflow](#13-git-and-github-workflow)
- [14. Build and Test the Final Program](#14-build-and-test-the-final-program)
- [15. Troubleshooting](#15-troubleshooting)
- [16. Exam Revision Notes](#16-exam-revision-notes)
- [17. Final Submission Checklist](#17-final-submission-checklist)

---

## 1. Project Overview

The Linux `ls` command displays the names of files and directories. This assignment develops a simplified `ls` program in C, adding functionality feature by feature. The work is organized using Git branches, commits, version tags, and GitHub releases.

The final program supports:

- **Long listing (`-l`)** — displays file details such as permissions, links, owner/group IDs, size, and modification time.
- **Column display** — arranges names in columns.
- **Horizontal display (`-x`)** — fills each row from left to right.
- **Alphabetical sorting** — sorts names before displaying them.
- **Colorized output** — uses terminal colors to distinguish common file types.
- **Recursive listing (`-R`)** — displays a directory and its subdirectories.

This is an educational implementation, not a complete replacement for every option and edge case supported by the system's GNU `ls`.

## 2. Learning Objectives

By implementing the features, the assignment provides practice with:

1. **System calls and library interfaces:** requesting file and directory information from the operating system.
2. **Directory handling:** opening a directory, reading entries, and closing it.
3. **File metadata:** inspecting permissions, size, timestamps, and file type.
4. **Arrays and dynamic memory:** storing names when the number of directory entries is not known in advance.
5. **Sorting:** comparing strings and arranging names alphabetically.
6. **Terminal output:** formatting text into columns and using ANSI escape sequences for colors.
7. **Recursion:** calling the directory-listing function again for each subdirectory.
8. **Version control:** maintaining feature branches, merging changes, tagging releases, and pushing work to GitHub.

## 3. Project Structure

The project uses these paths:

```text
BSDSF24M048-OS-A02/
├── src/
│   └── ls-v1.0.0.c       # C source code for the ls implementation
├── bin/
│   └── ls                # Compiled executable
├── REPORT.md             # Assignment report and feature explanations
├── Makefile              # Build instructions, if included in the repository
└── README.md              # Project guide and revision notes
```

**Important terms**

- **Source code:** the human-readable C instructions, stored in `src/`.
- **Compiler:** translates C source code into an executable program.
- **Executable/binary:** the program that can be run by the operating system, stored here in `bin/ls`.
- **Makefile:** a file containing build rules so a tool such as `make` can compile the program using a repeatable command.

> The exact files in a repository can change during development. Use `ls` or `find` to inspect the current structure.

## 4. Tools and Basic Workflow

### 4.1 Open the project in WSL

The project path used during development was:

```bash
cd "/mnt/e/5th Semester/Operating Systems/BSDSF24M048-OS-A02"
```

The quotation marks are important because the directory name contains spaces.

Check the current directory and files:

```bash
pwd
ls
```

- `pwd` prints the current working directory.
- `ls` lists the files in the current directory.

### 4.2 Compile the program

If the repository's `Makefile` provides a suitable build target, use the target documented there:

```bash
make
```

If compiling directly, the general form is:

```bash
gcc src/ls-v1.0.0.c -o bin/ls
```

- `gcc` invokes the C compiler.
- `-o bin/ls` names the resulting executable `bin/ls`.
- The `src/` directory contains the source; the `bin/` directory contains the built program.

If `bin/` does not exist, create it first:

```bash
mkdir -p bin
```

**Use the build command defined by your actual Makefile if it differs from the direct GCC example.** The source filename or build target may change as the assignment evolves.

### 4.3 Run the program

```bash
./bin/ls
./bin/ls -l
./bin/ls -x
./bin/ls -R
```

The `./` prefix tells the shell to run the executable from the current directory.

### 4.4 Inspect changes with Git

```bash
git status
git log --oneline
git branch -a
git tag
```

These commands show the working-tree state, recent commits, local/remote branches, and version tags.

## 5. Version History at a Glance

| Version | Feature | Option / behavior |
|---|---|---|
| `v1.1.0` | Long listing | `-l` |
| `v1.2.0` | Column display | Arrange names into columns |
| `v1.3.0` | Horizontal display | `-x` |
| `v1.4.0` | Alphabetical sorting | Sort names before display |
| `v1.5.0` | Colorized output | Color names according to file type |
| `v1.6.0` | Recursive listing | `-R` |

Each feature was developed on its own branch, merged into `main`, and tagged with a version number. Feature behavior accumulates across versions, so the final program combines the earlier features.

---

## 6. Feature 1 — Long Listing (`-l`), v1.1.0

### What we implemented

The `-l` option displays more information about each directory entry instead of showing only its name. The output follows the general style of Linux long-listing output.

Example:

```bash
./bin/ls -l
```

A typical line contains fields similar to:

```text
-rwxr-xr-x 1 1000 1000 4096 Oct 13 18:30 example
```

Exact values and formatting depend on the file and the implementation.

### Information displayed

- **File-type and permission characters:** indicate whether an entry is a regular file, directory, symbolic link, or another supported type, followed by permission bits.
- **Link count:** number of hard links to the file.
- **Owner and group:** the implementation may display numeric user and group IDs rather than resolving them to names.
- **Size:** file size in bytes.
- **Modification time:** when the file's contents or metadata were last modified, according to the timestamp used by the program.
- **Name:** the directory-entry name.

### How it works

The program obtains metadata for each entry and formats that metadata into a line. On Linux, interfaces such as `lstat()` and fields in `struct stat` can be used to inspect file metadata. The exact metadata call and formatting depend on the code in the submitted source.

`lstat()` is especially useful when symbolic links must be identified as links: unlike `stat()`, it reports information about the link itself rather than following the link to its target.

### Commands used in this feature

The feature-development workflow follows this pattern:

```bash
git switch main
git pull origin main
git switch -c feature-long-listing-v1.1.0

# Edit the C source, then build and test
make
./bin/ls -l

git status
git add src/ls-v1.0.0.c
git commit -m "Implement long listing for v1.1.0"
git push -u origin feature-long-listing-v1.1.0
```

After testing the feature branch, merge it into `main` and tag the release:

```bash
git switch main
git pull origin main
git merge --no-ff feature-long-listing-v1.1.0 -m "Merge feature 1: long listing"
make
git push origin main

git tag v1.1.0
git push origin v1.1.0
```

**Command notes**

- `git switch -c` creates and switches to a new branch.
- `git add` stages the selected changes for a commit.
- `git commit` saves a snapshot in the local Git history.
- `git push -u origin branch-name` publishes the branch and sets its upstream.
- `git merge --no-ff` creates a merge commit, preserving the feature branch's integration in history.
- `git tag` marks a specific commit with a version label.

> These are the feature's workflow commands in the project's versioning pattern. Commit wording or build commands may differ slightly from the exact historical commands used at the time.

---

## 7. Feature 2 — Column Display, v1.2.0

### What we implemented

This feature arranges file and directory names into columns instead of printing one name per line. Column formatting makes a directory listing more compact.

### How it works

The program needs to determine how much horizontal space is available and how many names can fit. The display logic places names into a layout that fits the terminal width. The exact number of columns depends on the width of the names and the terminal.

**Terminal width** means the number of character cells available across the terminal window. A program can query terminal dimensions, for example through terminal-control interfaces, or use another width-detection method implemented in its source.

### Commands used in this feature

```bash
git switch main
git pull origin main
git switch -c feature-column-display-v1.2.0

make
./bin/ls

git status
git add src/ls-v1.0.0.c
git commit -m "Implement column display for v1.2.0"
git push -u origin feature-column-display-v1.2.0
```

Merge and version the completed feature:

```bash
git switch main
git pull origin main
git merge --no-ff feature-column-display-v1.2.0 -m "Merge feature 2: column display"
make
git push origin main

git tag v1.2.0
git push origin v1.2.0
```

### What to remember

- Column display is a **formatting feature**: it changes where names appear, not which files exist.
- Layout depends on terminal width and name lengths.
- Test with both short and long filenames to identify alignment problems.

---

## 8. Feature 3 — Horizontal Display (`-x`), v1.3.0

### What we implemented

The `-x` option selects horizontal display. In a horizontal layout, names are filled across each row from left to right before moving to the next row.

Run it with:

```bash
./bin/ls -x
```

### Why it matters

Column-oriented layouts can fill entries in different directions. Horizontal display explicitly defines the direction in which names are placed. It is therefore a layout choice, not a file-type or metadata option.

### Commands used in this feature

```bash
git switch main
git pull origin main
git switch -c feature-horizontal-display-v1.3.0

make
./bin/ls -x

git status
git add src/ls-v1.0.0.c
git commit -m "Implement horizontal display for v1.3.0"
git push -u origin feature-horizontal-display-v1.3.0
```

Merge and version the feature:

```bash
git switch main
git pull origin main
git merge --no-ff feature-horizontal-display-v1.3.0 -m "Merge feature 3: horizontal display"
make
git push origin main

git tag v1.3.0
git push origin v1.3.0
```

### What to remember

- `-x` changes the display arrangement.
- It should coexist with the other supported options.
- Verify the output using a directory with enough entries to require multiple rows.

---

## 9. Feature 4 — Alphabetical Sorting, v1.4.0

### What we implemented

This feature sorts directory-entry names alphabetically before printing them. Without sorting, the order returned by directory-reading functions is not guaranteed to be alphabetical.

### How it works

The program collects entry names, compares them, and sorts them before formatting the output. A common C approach is to use `qsort()` with a comparison callback.

- **Comparator:** a function that compares two items and tells the sorting algorithm which should come first.
- **`strcmp()`:** compares two null-terminated strings lexicographically. Its result is negative, zero, or positive depending on the comparison.
- **`qsort()`:** a standard C library function that sorts an array using a supplied comparator.

A comparator for strings typically compares the names stored in the array. The final ordering depends on the comparator and any case-handling rules in the code.

### Commands used in this feature

```bash
git switch main
git pull origin main
git switch -c feature-alphabetical-sort-v1.4.0

make
./bin/ls

git status
git add src/ls-v1.0.0.c
git commit -m "Implement alphabetical sorting for v1.4.0"
git push -u origin feature-alphabetical-sort-v1.4.0
```

Merge and tag:

```bash
git switch main
git pull origin main
git merge --no-ff feature-alphabetical-sort-v1.4.0 -m "Merge feature 4: alphabetical sorting"
make
git push origin main

git tag v1.4.0
git push origin v1.4.0
```

### What to remember

- Directory-reading order is not the same as alphabetical order.
- Sort the names before the display routine lays them out.
- Sorting the names first allows the same order to be reused by long, column, horizontal, and recursive output.

---

## 10. Feature 5 — Colorized Output, v1.5.0

### What we implemented

This feature adds terminal colors to help visually distinguish common file types. The implementation uses ANSI escape sequences, which are special character sequences understood by many terminal emulators.

The submitted implementation's color rules include:

| Entry type / name pattern | Display style |
|---|---|
| Directory | Blue |
| Executable file | Green |
| Common compressed/archive suffixes such as `.tar`, `.gz`, `.zip` | Red |
| Symbolic link | Pink/magenta |
| Special file types | Reverse video |
| Other files | Default terminal color |

The program's actual detection order matters if more than one rule could apply to a name.

### How it works

The program identifies an entry's type or checks relevant name properties, selects a color sequence, prints the name, and then resets the terminal style.

A common ANSI pattern is:

```text
ESC[...m   # turn on a color or text style
ESC[0m     # reset styling
```

In C, `\033` is commonly used to represent the ESC character inside a string. The reset sequence is important: without it, later text may inherit the previous file's color.

### Commands used in this feature

```bash
git switch main
git pull origin main
git switch -c feature-colorized-output-v1.5.0

make
./bin/ls

git status
git add src/ls-v1.0.0.c
git commit -m "Implement colorized output for v1.5.0"
git push -u origin feature-colorized-output-v1.5.0
```

Merge and tag:

```bash
git switch main
git pull origin main
git merge --no-ff feature-colorized-output-v1.5.0 -m "Merge feature 5: colorized output"
make
git push origin main

git tag v1.5.0
git push origin v1.5.0
```

### What to remember

- ANSI color codes affect terminal presentation, not the file itself.
- Always reset the style after printing a colored name.
- File type should ideally be determined from metadata where appropriate; filename suffixes alone do not prove that a file is an archive.
- Colorized output should continue to work with sorting and display options.

---

## 11. Feature 6 — Recursive Listing (`-R`), v1.6.0

### What we implemented

The `-R` option lists the selected directory and then lists its subdirectories recursively.

For example:

```bash
./bin/ls -R
./bin/ls -R src
./bin/ls -lR src
./bin/ls -xR src
```

The exact combined-option syntax accepted depends on how the program parses arguments. The implementation uses option parsing that recognizes `-l`, `-x`, and `-R` together.

### How it works

The recursive implementation follows these steps:

1. List the current directory's entries.
2. Sort and display those entries using the existing formatting behavior.
3. For each entry that may be a subdirectory, construct its full path.
4. Inspect the entry's metadata.
5. If it is a directory, call the directory-listing function again with that path.
6. Print a directory header such as `some/path:` to show which directory is being listed.
7. Skip `.` and `..`, which refer to the current and parent directories and would otherwise cause recursion to loop back through the directory tree.

The implementation uses `lstat()` and `S_ISDIR()` when deciding whether an entry is a directory. Using `lstat()` means a symbolic link to a directory is identified as a symbolic link rather than automatically followed as a directory.

### Important technical terms

- **Recursion:** a function calling itself to solve a smaller instance of the same problem.
- **Base case:** the condition that stops further recursive calls. Here, recursion stops for an entry when it is not a directory to descend into, or when there are no further eligible subdirectories.
- **Full path:** the directory path joined with an entry name, such as `src/module/file.c`. It identifies the entry's location so metadata can be checked and a subdirectory can be opened correctly.
- **`lstat()`:** retrieves metadata about the named entry itself, including a symbolic link.
- **`S_ISDIR(mode)`:** macro used with a `struct stat` mode value to test whether the entry is a directory.

### Why full paths are needed

Suppose the program is listing `project/` and encounters a subdirectory named `src`. To list its contents, it needs to visit `project/src`, not just `src`. Constructing a full or correctly relative path ensures that metadata checks and recursive calls refer to the intended entry.

### Commands used in this feature

Create and switch to the feature branch:

```bash
git switch main
git pull origin main
git switch -c feature-recursive-listing-v1.6.0
```

Build and test the recursive behavior:

```bash
make
./bin/ls -R
./bin/ls -lR
./bin/ls -xR
./bin/ls -R src
```

A nested test directory was also used to verify that the program visited multiple levels. A reproducible test tree can be created with:

```bash
mkdir -p /tmp/ls-feature7-test/dirA/subdir
mkdir -p /tmp/ls-feature7-test/dirB
touch /tmp/ls-feature7-test/file1.txt
touch /tmp/ls-feature7-test/dirA/file2.txt
touch /tmp/ls-feature7-test/dirA/subdir/file3.txt
touch /tmp/ls-feature7-test/dirB/file4.txt

./bin/ls -R /tmp/ls-feature7-test
./bin/ls -lR /tmp/ls-feature7-test
./bin/ls -xR /tmp/ls-feature7-test
```

This test tree checks top-level entries, nested directories, and the formatting options when recursion is enabled.

Stage, commit, and publish the feature branch:

```bash
git status
git add src/ls-v1.0.0.c REPORT.md
git commit -m "Implement recursive listing for v1.6.0"
git push -u origin feature-recursive-listing-v1.6.0
```

Merge the feature into `main`, rebuild, and push:

```bash
git switch main
git pull origin main
git merge --no-ff feature-recursive-listing-v1.6.0 -m "Merge feature 7: recursive listing"
make
git push origin main
```

Create and push the final version tag:

```bash
git tag v1.6.0
git push origin v1.6.0
```

### What to remember

- Recursion is suitable for tree-shaped structures such as directory hierarchies.
- Always exclude `.` and `..` from recursive descent.
- Construct the correct path before checking or listing a child entry.
- Check that an entry is a directory before recursively listing it.
- Avoid following symbolic links unintentionally, because symlink traversal can lead to cycles.
- Recursive output needs headers or another clear separator so users can tell which directory's entries are being displayed.

---

## 12. Important C and Linux Concepts

### Directory APIs

| Function / type | Purpose |
|---|---|
| `opendir()` | Opens a directory stream for reading. |
| `readdir()` | Reads the next entry from a directory stream. |
| `closedir()` | Closes the directory stream when finished. |
| `struct dirent` | Represents a directory entry; `d_name` contains its name. |
| `struct stat` | Holds metadata about a filesystem entry. |
| `lstat()` | Reads metadata about the entry itself without following a symbolic link. |
| `S_ISDIR()` | Tests whether metadata identifies an entry as a directory. |

These are POSIX/Linux interfaces rather than functions defined by the ISO C language alone.

### Memory and arrays

The number of entries in a directory is not known ahead of time. A program can dynamically allocate an array to store names and grow it as needed.

- **Dynamic memory allocation:** requests memory while the program is running.
- **`malloc()` / `realloc()` / `free()`:** commonly used to allocate, resize, and release memory.
- **Memory management:** every allocated name and array should be handled carefully to avoid leaks and invalid memory access.

Use the actual source code to review which allocation strategy and cleanup paths this implementation uses.

### Sorting and strings

- `strcmp(a, b)` compares two C strings.
- `qsort()` sorts an array using a comparator function.
- A C string ends with a null character, `'\0'`.
- Filenames are names, not necessarily full paths; a path is needed when operating on an entry in another directory.

### Command-line arguments

The `main()` function receives command-line arguments through `argc` and `argv`:

```c
int main(int argc, char *argv[])
```

- `argc` is the number of command-line arguments.
- `argv` is an array of argument strings.
- An option parser examines arguments such as `-l`, `-x`, and `-R` to enable the requested behavior.

### File permissions

Linux permission bits describe read (`r`), write (`w`), and execute (`x`) access for the owner, group, and others. A long listing typically formats these bits into a string. For directories, execute permission also controls whether users can traverse/search the directory.

### Standard output and terminal formatting

`printf()` and related functions write formatted output. When the destination is a terminal, ANSI escape sequences can change the appearance of subsequent text. These sequences do not change file contents.

---

## 13. Git and GitHub Workflow

The project was developed feature by feature using branches and version tags.

### 13.1 Common commands

| Command | Meaning |
|---|---|
| `git status` | Shows staged, unstaged, and untracked changes. |
| `git switch main` | Switches to the `main` branch. |
| `git pull origin main` | Fetches and integrates updates from GitHub's `main` branch. |
| `git switch -c BRANCH` | Creates a branch and switches to it. |
| `git add FILE` | Stages a file's changes for the next commit. |
| `git commit -m "MESSAGE"` | Saves staged changes in local history. |
| `git push -u origin BRANCH` | Publishes a branch and sets its upstream. |
| `git merge --no-ff BRANCH` | Merges a branch while preserving a merge commit. |
| `git push origin main` | Pushes local `main` to GitHub. |
| `git tag vX.Y.Z` | Creates a version tag at the current commit. |
| `git push origin vX.Y.Z` | Pushes that tag to GitHub. |
| `git branch -a` | Lists local and remote-tracking branches. |
| `git tag` | Lists local tags. |
| `git log --oneline --decorate -12` | Shows a concise recent history. |

### 13.2 Feature branches

The repository contains these feature branches:

```text
feature-long-listing-v1.1.0
feature-column-display-v1.2.0
feature-horizontal-display-v1.3.0
feature-alphabetical-sort-v1.4.0
feature-colorized-output-v1.5.0
feature-recursive-listing-v1.6.0
```

A separate branch isolates work on one feature. Once the feature is tested, it is merged into `main`.

### 13.3 Version tags and releases

The version tags are:

```text
v1.1.0
v1.2.0
v1.3.0
v1.4.0
v1.5.0
v1.6.0
```

A **tag** marks a specific Git commit. A **GitHub Release** is a published release page that can include release notes and downloadable assets. Creating a Git tag does not automatically create a GitHub Release.

For the final version, the release title used was:

```text
Version 1.6.0: Recursive Listing (-R)
```

The compiled executable `bin/ls` was attached to the release.

### 13.4 Final repository checks

```bash
git status
git branch -a
git tag
git log --oneline --decorate -12
```

A clean final status should show that the working tree has no uncommitted changes and that `main` is up to date with `origin/main`.

---

## 14. Build and Test the Final Program

Run these commands from the project root:

```bash
make
./bin/ls
./bin/ls -l
./bin/ls -x
./bin/ls -R
./bin/ls -lR
./bin/ls -xR
```

If your Makefile uses a different target, use the target defined by the repository.

### Test checklist

- [ ] The program builds without compiler errors.
- [ ] Default listing shows the current directory's entries.
- [ ] `-l` displays the long-listing fields.
- [ ] Column output fits the terminal reasonably.
- [ ] `-x` fills names horizontally.
- [ ] Names are sorted as intended.
- [ ] Colors are reset after each colored name.
- [ ] `-R` lists nested directories.
- [ ] Recursive output clearly labels directories.
- [ ] `.` and `..` do not cause endless recursion.
- [ ] Combined options such as `-lR` work if supported by the argument parser.
- [ ] `REPORT.md` contains the required feature descriptions and report answers.

---

## 15. Troubleshooting

### `make: command not found`

The `make` utility may not be installed in the environment. Alternatively, compile using the correct GCC command for the current source and output path.

### `gcc: No such file or directory`

Check the source path and current directory:

```bash
pwd
ls src
```

### `./bin/ls: No such file or directory`

The executable may not have been built or may be in another location:

```bash
ls -l bin
make
```

### Changes do not appear on GitHub

Check which branch you are on and whether the changes were committed and pushed:

```bash
git status
git branch --show-current
git log --oneline -5
```

Then push the correct branch if needed.

### Recursive output repeats directories

Check that the code skips `.` and `..`, constructs the child path correctly, and only recurses into entries identified as directories. Be careful with symbolic links to directories.

---

## 16. Exam Revision Notes

### Must remember

1. **`opendir()`** opens a directory stream, **`readdir()`** reads entries, and **`closedir()`** closes the stream.
2. **`struct stat`** stores file metadata.
3. **`lstat()`** examines a symbolic link itself; **`stat()`** follows the link to its target.
4. **`S_ISDIR()`** checks whether a mode value represents a directory.
5. **`strcmp()`** compares strings, while **`qsort()`** sorts an array using a comparator.
6. **Recursion** requires a stopping condition; for directory traversal, do not recurse into `.` or `..`.
7. **Full paths** ensure file operations refer to the intended child entry.
8. **ANSI escape sequences** style terminal output; reset them so styles do not leak into later text.
9. **Git branches** isolate work, **commits** save snapshots, **tags** identify versions, and **GitHub Releases** publish release information and assets.

### Likely viva questions

**Q1. What does the `ls` command do?**  
It lists the names of files and directories. Options can change the displayed metadata and layout.

**Q2. Why did we implement features separately?**  
Separate feature branches make each change easier to develop, test, review, and merge without mixing unrelated work.

**Q3. What is the purpose of `-l`?**  
It displays a detailed listing, including permissions, link count, ownership information, size, timestamp, and name.

**Q4. Why should directory entries be sorted before display?**  
The order returned by directory-reading interfaces is not guaranteed to be alphabetical. Sorting creates predictable output.

**Q5. Why use `qsort()`?**  
It is a standard C library function for sorting an array using a comparison function supplied by the programmer.

**Q6. What is the difference between `stat()` and `lstat()`?**  
`stat()` follows a symbolic link and reports metadata for its target. `lstat()` reports metadata for the link itself.

**Q7. Why must `.` and `..` be skipped in recursive listing?**  
They refer to the current directory and its parent. Recursing into them can revisit already visited directories and create endless or repeated traversal.

**Q8. Why construct a full path for a subdirectory?**  
A name alone may not identify the intended entry from the program's current working directory. Joining the parent path and child name lets metadata checks and recursive calls address the correct location.

**Q9. What is a base case in recursion?**  
It is a condition that prevents further recursive calls. In directory listing, the program does not descend into entries that are not eligible subdirectories.

**Q10. What is the difference between a Git tag and a GitHub Release?**  
A tag marks a commit in Git history. A GitHub Release is a published page associated with a tag and can contain notes and downloadable files.

**Q11. What does `git merge --no-ff` do?**  
It merges a branch while creating a merge commit, even when a fast-forward merge would otherwise be possible.

**Q12. What do ANSI color codes do?**  
They send terminal styling instructions, such as changing text color. They do not modify the underlying files.

---

## 17. Final Submission Checklist

- [ ] All six features are implemented and tested.
- [ ] All feature branches have been pushed to GitHub.
- [ ] All feature branches have been merged into `main`.
- [ ] `main` is pushed and up to date with `origin/main`.
- [ ] Version tags `v1.1.0` through `v1.6.0` are present.
- [ ] GitHub Releases have the required titles and assets.
- [ ] `bin/ls` is attached to the final release.
- [ ] `REPORT.md` answers every required assignment question.
- [ ] `README.md` explains the project, features, commands, and concepts.
- [ ] `git status` shows a clean working tree.

---

## 📌 Final Note

Use this README as both project documentation and a revision guide. For exact implementation details—such as the precise metadata fields, allocation strategy, color-selection order, and argument-parsing behavior—refer to the submitted C source and `REPORT.md`. The descriptions here explain the concepts and the intended feature behavior; the actual source remains the authority for what the program does.
