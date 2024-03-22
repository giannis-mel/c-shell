#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CMD_LENGTH 512
#define MAX_NUM_CMDS 20

// Function pointer type for built-in command functions
typedef void (*BuiltinCmdFunction)();

// Struct for built-in commands
typedef struct {
    const char *name;
    BuiltinCmdFunction func;
} BuiltinCmd;

// Built-in command functions
void quitShell();
void displayAuthor();

// Array of BuiltinCmd structs
BuiltinCmd builtinCommands[] = {
    {"quit", quitShell},
    {"author", displayAuthor},
};

int numOfBuiltinCommands = sizeof(builtinCommands) / sizeof(BuiltinCmd);

void executeCommand(char **args);
void handleMultipleCommands(char **args, const char *delimiter);
int getCommandType(char **args);
int isBuiltin(char *arg);
void executeBuiltinCommand(char *arg);
void processCommand(char **args);

// Built-in functions
void quitShell() {
    exit(0);
}

void displayAuthor() {
    printf("\ngiannis-mel\nmeleziadisgiannis@gmail.com\n\n");
}

void executeCommand(char **args) {
    /**
     * Executes a command by creating a new process.
     *
     * This function forks the current process. In the child process, it executes
     * the command specified in 'args' using 'execvp'. The parent process waits
     * for the child's completion. If forking or execution fails, an error is reported.
     *
     * @param args An array of strings where the first element is the command to execute
     *             and the subsequent elements are the arguments to the command.
    */
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        wait(NULL);  // Parent waits for child to complete
    } else {
        perror("Fork failed");
    }
}

void handleMultipleCommands(char **args, const char *delimiter) {
    /**
     * Executes multiple commands separated by a given delimiter.
     *
     * Iterates through 'args' and splits the commands at each occurrence of the specified 'delimiter'.
     * Each command or command group is executed separately. If the delimiter is encountered or the
     * end of 'args' is reached, the current set of commands is executed.
     *
     * @param args An array of strings, representing the command line arguments.
     * @param delimiter A string used as the delimiter to separate commands.
    */
    char *commands[MAX_NUM_CMDS];
    int i = 0, j = 0;
    while (args[i] != NULL) {
        // Include the last command before delimiter or at the end of args
        if (strcmp(args[i], delimiter) == 0 || args[i+1] == NULL) {
            commands[j] = (args[i+1] == NULL) ? args[i] : NULL;
            executeCommand(commands);
            j = 0;
        } else {
            commands[j++] = args[i];
        }
        i++;
    }
}


int getCommandType(char **args) {
    /**
     * Determines the command type based on the arguments provided.
     *
     * It checks if the command is a built-in command (returns 4), contains "&&" (returns 3),
     * contains ";" (returns 2), or is a regular command (returns 1). If no arguments are provided,
     * it returns 0 to indicate an empty command.
     *
     * @param args An array of strings, each representing a part of the command.
     * @return An integer representing the command type.
    */

    // Check for no arguments
    if (args[0] == NULL) return 0;
    // Check for built-in command
    if (args[1] == NULL && isBuiltin(args[0])) return 4;
    // Check for "&&" and ";" in the command
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "&&") == 0) return 3;
        if (strcmp(args[i], ";") == 0) return 2;
    }
    // Default to a regular command
    return 1;
}


int isBuiltin(char *arg) {
    /**
     * Checks if the given command is a built-in command.
     *
     * Iterates through the array of built-in commands and compares each with the provided 'arg'.
     * Returns 1 (true) if a match is found, indicating the command is built-in. Otherwise, returns 0 (false).
     *
     * @param arg The command to check (as a string).
     * @return An integer indicating whether the command is built-in (1 for true, 0 for false).
    */
    for (int i = 0; i < numOfBuiltinCommands; i++) {
        if (strcmp(arg, builtinCommands[i].name) == 0) return 1;
    }
    return 0;
}

void executeBuiltinCommand(char *arg) {
    /**
     * Executes a built-in command if it matches one of the predefined commands.
     *
     * Iterates through the array of built-in commands. If a match is found with the provided 'arg',
     * the corresponding function for that command is executed. If no match is found, an error message
     * is printed.
     *
     * @param arg The command to be executed (string).
    */
    for (int i = 0; i < numOfBuiltinCommands; i++) {
        if (strcmp(arg, builtinCommands[i].name) == 0) {
            (*builtinCommands[i].func)();
            return;
        }
    }
    fprintf(stderr, "Error: Something is terribly wrong in my code.\n");
}

void processCommand(char **args) {
    /**
     * Processes a command by determining its type and executing it accordingly.
     *
     * Based on the command type determined by `getCommandType`, this function decides whether
     * to execute a single command, handle multiple commands separated by ";" or "&&", or execute
     * a built-in command.
     *
     * @param args An array of strings representing the command and its arguments.
    */
    int commandType = getCommandType(args);
    switch (commandType) {
        case 0: return;
        case 1: executeCommand(args); break;
        case 2: handleMultipleCommands(args, ";"); break;
        case 3: handleMultipleCommands(args, "&&"); break;
        case 4: executeBuiltinCommand(args[0]); break;
        default: fprintf(stderr, "Error: Unrecognized command type.\n");
    }
}

void processInput(char *line, char **args) {
    /**
     * Tokenizes the input line into arguments.
     *
     * Splits the 'line' using spaces, tabs, and newlines as delimiters.
     * Populates 'args' with the resulting tokens, followed by a NULL terminator.
     *
     * @param line A string to be tokenized.
     * @param args An array to store the tokens.
    */
    char *token;
    int j = 0;

    token = strtok(line, " \n\t");
    while (token != NULL) {
        args[j++] = token;
        token = strtok(NULL, " \n\t");
    }
    args[j] = NULL;  // Ensure args is null-terminated
}

int main(int argc, char **argv) {
    /**
     * The main entry point of the shell program.
     *
     * This function initializes the shell and processes input in two modes:
     * Interactive mode: Read and execute commands from standard input (stdin).
     * Batch mode: Execute commands from a file specified as a command line argument.
    */
    char line[MAX_CMD_LENGTH], *args[MAX_NUM_CMDS];
    FILE *file;

    // Interactive mode
    if (argc == 1) {
        while (1) {
            printf("myshell> ");
            if (!fgets(line, MAX_CMD_LENGTH, stdin)) continue;
            processInput(line, args);
            processCommand(args);
        }
    // Batch mode
    } else if (argc == 2 && (file = fopen(argv[1], "r"))) {
        while (fgets(line, MAX_CMD_LENGTH, file)) {
            processInput(line, args);
            processCommand(args);
        }
        fclose(file);
    } else {
        printf("Usage: ./my_shell or ./my_shell <batchfile>\n");
    }
    return 0;
}
