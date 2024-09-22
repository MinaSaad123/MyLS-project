# MyLS Project Overview

## Project Name
**MyLS**
# MyLS Project

## Overview
**MyLS** is a custom implementation of the Linux `ls` command that lists directory contents. This project extends the basic functionality of `ls` by supporting a range of options including `-latucifd1`. These options allow the user to control the display of files in various formats, order, and additional details.

## Supported Options

### `-l`
Displays the contents in long format, providing detailed information such as file permissions, number of links, owner, group, file size, and modification time.

### `-a`
Shows all files, including hidden files (those that start with a `.`).

### `-t`
Sorts files by modification time, with the most recently modified files appearing first.

### `-u`
Displays the last access time instead of the modification time.

### `-c`
Sorts and displays files by their last status change time instead of modification time.

### `-i`
Displays the inode number for each file.

### `-f`
Disables sorting; lists files in directory order.

### `-d`
Lists only directories, not their contents.

### `-1`
Forces the output to display one file per line.

## Features
- Full support for handling hidden files and directories.
- Options to sort by modification, access, and change times.
- Efficient directory traversal using system calls.
- Custom formatting for long-format output with details like inodes and permissions.
- Support for single-column file display with the `-1` option.

## Features
- Full support for handling hidden files and directories.
- Options to sort by modification, access, and change times.
- Efficient directory traversal using system calls.
- Custom formatting for long-format output with details like inodes and permissions.
- Support for single-column file display with the `-1` option.

## Usage
To use **MyLS**, simply invoke it from the terminal followed by any of the supported options:

```bash
./myls -latu
```

This command will:
- Show files in long format (`-l`),
- Display hidden files (`-a`),
- Sort by last access time (`-u`),
- And order them by the most recent first (`-t`).

## Project Demo
For a detailed video explanation and demo of the **MyLS** project, watch the following YouTube video:

[![MyLS Project Demo](https://img.youtube.com/vi/DbQ2LE1ykxo/0.jpg)](https://youtu.be/DbQ2LE1ykxo?si=95Gl7z3YxIoJRpgV)
