## Overview
This is a simple shell program (my_shell.c) written in C, which operates in two modes: interactive and batch.

## Features
- Interactive and Batch modes.
- Built-in commands (`quit`, `author`).
- Execution of external commands like `ls`, `echo`, `grep`, etc.
- Handling multiple commands in one line using `;` and `&&`.

## Usage

### Interactive Mode
- Start the shell in interactive mode by running `./my_shell` after compiling.
- In this mode, users can execute commands directly from the prompt.

### Batch Mode
- Start the shell with a batch file: `./my_shell batchfile`.
- The `batchfile` should be a text file containing the commands to be executed by the shell.
- If `quit` is not included in the batch file, the program will terminate automatically after executing all commands.

## Compilation
- Compile the program by executing the Makefile using the `make` command.

## Tests
- A `batchfile` is also included for testing the batch mode.

## Built-in Commands
- `quit`: Exits the shell.
- `author`: Displays the author's information.

## Future development
- Differentiate ';' and '&&'.
- Have the ability to execute built-in commands when multiple commands are given.
- Add more tests to verify the shell's output.
