# Operating Systems — Programming Assignment 02
## Re-engineering the Linux `ls` Utility

**Language:** C  
**Build Tool:** GNU Make  
**Platform:** Linux / WSL

---

## Feature 1: Project Structure and Initial Build

### Objective
The first feature established the project structure, created the Makefile, and compiled the initial C program.

### Project Structure
- `src/ls-v1.0.0.c` — Main C source file.
- `bin/` — Contains the compiled executable `ls`.
- `obj/` — Reserved for object files.
- `man/` — Reserved for manual documentation.
- `Makefile` — Automates compilation.
- `README.md` — Provides an overview of the project.
- `REPORT.md` — Records implementation details and assignment answers.

The `make` command compiles the source code, while `./bin/ls` runs the executable. Git branches, commits, and tags are used to organize development and maintain a history of feature versions.

---

## Feature 2: Long Listing Format (`v1.1.0`)

### Objective
The `-l` option displays detailed information about directory entries, including file type, permissions, link count, owner, group, file size, modification time, and filename.

### Question 1: What is the difference between `stat()` and `lstat()`? When is `lstat()` more appropriate?

**Answer:**

Both `stat()` and `lstat()` retrieve file metadata and store it in a `struct stat`. The main difference is their handling of symbolic links.

- `stat()` follows a symbolic link and returns information about its target.
- `lstat()` returns information about the symbolic link itself.

In an `ls` implementation, `lstat()` is appropriate when the program needs to identify a symbolic link as a separate file type and display its own metadata rather than the metadata of its target.

### Question 2: How do bitwise operators and predefined macros extract file type and permission information from `st_mode`?

**Answer:**

The `st_mode` field contains both file type and permission bits. Predefined macros such as `S_ISDIR()` and `S_ISLNK()` identify file types. The bitwise AND operator (`&`) checks individual permission flags.

For example, `st_mode & S_IRUSR` checks the owner's read permission, `st_mode & S_IWUSR` checks write permission, and `st_mode & S_IXUSR` checks execute permission. The program uses these results to construct a permission string such as `rwxr-xr--`. Special permission bits, including set-user-ID, set-group-ID, and the sticky bit, can also be checked.

---

## Feature 3: Column Display — Down Then Across (`v1.2.0`)

### Objective
The default display arranges filenames vertically down each column before moving to the next column. The program stores filenames in a dynamically allocated array and uses the longest filename and terminal width to calculate the layout.

### Question 1: Explain the logic of down-then-across printing. Why is a simple loop insufficient?

**Answer:**

The program first reads directory entries into an array and tracks the longest filename. It then calculates how many columns fit within the terminal width and how many rows are required.

To print down then across, the program calculates the array index for each row and column. For example, the index can be calculated using `column * num_rows + row`.

A simple loop through the array would print filenames in their stored order from left to right. That produces horizontal, or row-major, output rather than filling each column vertically. Calculating the index for each row and column is therefore necessary to achieve the required layout.

### Question 2: What is the purpose of `ioctl()`? What are the limitations of a fixed-width fallback such as 80 columns?

**Answer:**

The `ioctl()` system call can retrieve information about a terminal. The call `ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)` requests the terminal window size, including the number of columns stored in `ws.ws_col`.

The program uses this value to estimate how many filenames can fit across the screen. If terminal-width detection fails, it uses 80 columns as a fallback.

A fixed width may not match the actual terminal. If the terminal is narrower, output may wrap unexpectedly; if it is wider, the program may not use the available space efficiently. Detecting the terminal size makes the layout more adaptable.

---

## Feature 4: Horizontal Display (`-x`, `v1.3.0`)

### Objective
The `-x` option displays filenames horizontally, from left to right across each row. The default display continues to fill columns vertically, and the `-l` option continues to provide detailed file information.

### Question 1: Compare down-then-across and horizontal printing. Which requires more pre-calculation, and why?

**Answer:**

Down-then-across printing requires more pre-calculation because the program must determine the number of columns, calculate the required rows, and work out the correct array index for each row and column.

Horizontal printing can process filenames in their existing order. It prints each name across the row and moves to the next line when the available width is reached. Therefore, down-then-across printing generally requires more layout calculations than horizontal printing.

### Question 2: How does the program select between `-l`, `-x`, and the default display?

**Answer:**

The program uses `getopt()` to parse command-line options and stores the selected modes in flag variables.

- `-l` selects long listing, which displays detailed metadata.
- `-x` selects horizontal display.
- No option selects the default down-then-across column display.

The program checks these flags and chooses the corresponding printing logic. This keeps option handling separate from the display operations and makes the program easier to extend.

---

## Feature 5: Alphabetical Sorting (`v1.4.0`)

### Objective
This feature sorts filenames alphabetically before displaying them. It uses `qsort()` to sort the array and `strcmp()` to compare filenames. The sorted array is shared by the default, `-x`, and `-l` display modes.

### Question 1: Why must directory entries be collected before sorting? What are the drawbacks for very large directories?

**Answer:**

The `readdir()` function returns directory entries individually, and their order is not guaranteed to be alphabetical. The program must collect the filenames in an array before it can sort the complete list using `qsort()`.

This method requires memory for the array and the stored filename strings. For directories containing millions of entries, memory consumption can become substantial. Collecting, copying, and sorting so many filenames also increases processing time, and memory allocation may fail if sufficient memory is unavailable.

### Question 2: Explain the comparison function used by `qsort()`. Why does it accept `const void *` arguments?

**Answer:**

The `qsort()` function sorts an array using a comparison function supplied by the programmer. The comparison function receives pointers to two array elements, converts them to the correct string-pointer type, and compares the filenames using `strcmp()`.

The comparison function returns a negative value when the first filename comes before the second, zero when they are equal, or a positive value when the first comes after the second. `qsort()` uses these results to arrange the array.

The `const void *` parameters make the function compatible with the generic interface required by `qsort()`, which can sort arrays of different data types. Since this program stores pointers to strings, the generic arguments are converted to the appropriate pointer type before comparison. The `const` qualifier indicates that the comparison function should not modify the elements through these pointers.

---

## Feature 6: Colorized Output (`v1.5.0`)

### Objective
This feature adds color to filenames according to their file types using ANSI escape sequences. It preserves alphabetical sorting and existing display modes while disabling colors when output is redirected.

### Color Rules
- **Directories:** Blue
- **Executable files:** Green
- **Archives** (`.tar`, `.gz`, `.zip`): Red
- **Symbolic links:** Pink
- **Special files** (character devices, block devices, FIFOs, and sockets): Reverse video
- **Other files:** Default terminal color

### Report Questions

**Q1. How do ANSI escape codes work to produce color in a standard Linux terminal? Show the specific code sequence for printing text in green.**

**Answer:**

ANSI escape codes are special character sequences interpreted by compatible terminals as formatting instructions. They can change text color and style. In C, `\033[1;32m` enables bold green text, while `\033[0m` resets formatting to the terminal's default.

Example:

```c
printf("\033[1;32mThis text is green\033[0m\n");
```

Here, `\033` represents the Escape character, `32` selects green, `1` enables bold styling, and `0` resets the formatting.

**Q2. To color an executable file, you need to check its permission bits. Explain which bits in the `st_mode` field you need to check to determine if a file is executable by the owner, group, or others.**

**Answer:**

The `st_mode` field of the `stat` structure contains file type and permission information. To determine whether a file is executable, check these permission bits:

- `S_IXUSR`: Execute permission for the **owner**.
- `S_IXGRP`: Execute permission for the **group**.
- `S_IXOTH`: Execute permission for **others**.

In C, the bitwise AND operator (`&`) checks whether any of these bits are set:

```c
if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
    /* Color the filename green */
}
```

If at least one execute bit is set, the file is treated as executable for colorization.

---

## Feature 7: Recursive Listing (-R), Version 1.6.0

### 1. What is a base case in recursion, and what stops this recursive listing from continuing forever?

A base case is a condition that stops a recursive function from making further recursive calls. In this implementation, recursion continues only when the program finds a subdirectory in the current directory. If no subdirectories are found, no further recursive calls are made. The program also excludes entries beginning with a dot, including `.` and `..`, so it does not recurse through these special directory entries. Symbolic links are checked with `lstat()`, preventing the program from following symbolic links to directories.

### 2. Why must the program construct a full path before making a recursive call?

The program constructs the full path by combining the current directory and the subdirectory name, such as `parent_dir/subdir`. This ensures that the next recursive call opens the intended subdirectory, regardless of the program's current working directory. If the program called `list_directory("subdir", ...)` instead, the relative path would be resolved from the process's current working directory rather than automatically from `parent_dir`, which could cause it to open the wrong directory or fail.
