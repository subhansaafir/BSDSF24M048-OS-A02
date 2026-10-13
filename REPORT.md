
## Feature 2: Long Listing Format

### 1. Difference between stat() and lstat()

stat() retrieves metadata about the file referred to by a path and follows
symbolic links to their targets. lstat() retrieves metadata about the symbolic
link itself instead of following it. For an ls implementation, lstat() is
useful when the program must identify and display symbolic links correctly.

### 2. Using st_mode and bitwise operators

The st_mode field in struct stat stores both file type and permission bits.
Bitwise operators and predefined macros extract the required information.
For example, (st_mode & S_IRUSR) checks whether the owner has read permission,
and S_ISDIR(st_mode) checks whether the file is a directory. Similar macros
check other file types, while S_IWUSR and S_IXUSR represent owner write and
execute permissions. Special permission bits include S_ISUID, S_ISGID,
and S_ISVTX.

## Feature 3: Column Display

The program stores filenames in a dynamic array and arranges them vertically down each column, then across columns. It calculates the number of columns using the longest filename and terminal width.

The program uses ioctl() with TIOCGWINSZ to detect terminal width. If detection fails, it uses 80 columns as the default.
