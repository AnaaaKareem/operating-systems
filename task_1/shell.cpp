#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdlib>
#include <limits.h>
using namespace std;

// Access environment variables
extern char** environ;

// Initialize current working directory
char cwd[PATH_MAX];

// Function to handle i/o-redirection operators
void redirection_handler(char** args) {
    // Initialize token counter
    int i = 0;
    // Go through all arguments in the array
    while (args[i] != nullptr) {
        // Check if the current command has an output redirection operator (>)
        if (strcmp(args[i], ">") == 0) {
            // Check if the output file is missing
            if (args[i+1] == nullptr) {
                // Print error message for missing file and exit
                printf("Missing output file\n");    
                exit(1);
            }
            // Open file for writing, create or truncate
            int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            // Duplicate the file descriptor to standard output
            dup2(fd, STDOUT_FILENO);
            // Close the original file descriptor
            close(fd);
            // Add a null pointer at the end indicating the end of the command
            args[i] = nullptr;
        }
        // Check if the current command has an input redirection operator (<)
        else if (strcmp(args[i], "<") == 0) {
            // Check if the input file is missing
            if (args[i+1] == nullptr) {
                // Print error message for missing file and exit
                printf("Missing input file\n");
                exit(1);
            }
            // Open the file for reading only
            int fd = open(args[i+1], O_RDONLY);
            // Duplicate the file descriptor to standard input
            dup2(fd, STDIN_FILENO);
            // Close the original file descriptor
            close(fd);
            // Add a null pointer at the end indicating the end of the command
            args[i] = nullptr;
        }
        // Check if the current command has an append redirection operator (>>)
        else if (strcmp(args[i], ">>") == 0) {
            // Check if the output file is missing
            if (args[i+1] == nullptr) {
                // Print error message for missing file and exit
                printf("Missing output file\n");
                exit(1);
            }
            // Open file for appending, create if needed
            int fd = open(args[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            // Redirect STDOUT to the opened file using dup2
            dup2(fd, STDOUT_FILENO);
            // Close the original file descriptor
            close(fd);
            // Add a null pointer at the end indicating the end of the command
            args[i] = nullptr;
        }
        // Increment argument counter
        i++;
    }
}

// Function execute shell commands
void shell(string line) {
    // Initialize background flag
    bool background = false;

    // Tokenize the command by spaces
    char* token = strtok((char*)line.c_str(), " ");
    // Initialize arguments array (64 arguments max)
    char* args[64];
    // Initialize argument index counter
    int i = 0;
    // Loop through all tokens (command + arguments)
    while (token) {
        // Check for background execution symbol (&)
        if (strcmp(token, "&") == 0) {
            // Set background flag to true
            background = true;
            // Stop processing further tokens
            break;
        }
        // Add token to arguments array and increment index
        args[i++] = token;
        // Get the next token from the string
        token = strtok(nullptr, " ");
    }
    // Add null at the end of the arguments array to indicate the end of the command
    args[i] = nullptr;


    // Check if no command was entered
    if (args[0] == nullptr) {
        return;
    }
    
    // Initialize command
    string cmd = args[0];

    // 1. cd - change directory
    if (cmd == "cd") {
        // Check if no directory argument provided
        if (args[1] == nullptr) {
            // Print the current working directory
            printf("%s\n", getcwd(cwd, sizeof(cwd)));
        } else {
            // Check if directory exists if not print an error message else change to it
            if (chdir(args[1]) != 0) {
                // Print error message if change directory fails
                perror("cd error");
            } else {
                // Update PWD environment variable
                setenv("PWD", getcwd(cwd, sizeof(cwd)), 1);
            }
        }

        return;
    }

    // 2. dir - list directory
    else if (cmd == "dir") {
        // Fork a new process
        pid_t pid = fork();
        // Code executed by child process
        if (pid == 0) {
            // Handle I/O redirection
            redirection_handler(args);
            // Check if no directory specified
            if (args[1] == nullptr) {
                // Prepare ls arguments array
                char* ls_args[] = {(char*)"ls", nullptr};
                // Execute ls command
                execvp("ls", ls_args);
            } else if (args[1] != nullptr) {
                // Prepare ls arguments array with path
                char* ls_args[] = {(char*)"ls", args[1], nullptr};
                // Execute ls command with path
                execvp("ls", ls_args);
            }
            // Print error if execvp fails
            perror("execvp");
            // Exit child process with error status
            exit(1);
        }
        // Code executed by parent process
        else {
            // If not running in background
            if (!background) {
                // Wait for child process to finish
                wait(NULL);
            }
        }

        return;
    }

    // 3. environ - print environment variables
    else if (cmd == "environ") {
        // Fork a new process
        pid_t pid = fork();
        // Code executed by child process
        if (pid == 0) {
            // Handle I/O redirection
            redirection_handler(args);
            // Iterate through all environment variables
            for (char** current = environ; *current != nullptr; current++) {
                // Print each environment variable in a new line
                printf("%s\n", *current);
            }
            // Exit child process successfully
            exit(0);
        }
        // Code executed by parent process
        else {
            // If not running in background
            if (!background) {
                // Wait for child process to finish
                wait(NULL);
            }
        }

        return;
    }

    // 4. set - set environment variable
    else if (cmd == "set") {
        // Check if variable name or value is missing
        if (args[1] == nullptr or args[2] == nullptr) {
            // Print usage message
            printf("Usage: set <variable> <value>\n");
        } else {
            // Set the environment variable
            setenv(args[1], args[2], 1);
        }

        return;
    }

    // 5. echo - output to console
    else if (cmd == "echo") {
        // Fork a new process
        pid_t pid = fork();
        // Code executed by child process
        if (pid == 0) {
            // Handle I/O redirection
            redirection_handler(args);
            // Initialize index to 1 to skip command name
            int i = 1;
            // Iterate through all arguments that comes after echo command
            while (args[i] != nullptr) {
                // Print argument with trailing space
                printf("%s ", args[i]);
                // Increment index
                i++;
            }
            // Print newline after the echo command finishes
            printf("\n");
            // Exit child process successfully
            exit(0);
        }
        // Code executed by parent process
        else {
             // If not running in background 
             if (!background) {
                // Wait for child process to finish
                wait(NULL);
            }
        }
        // Return from function
        return;
    }

    // 6. help - display help using more filter
    else if (cmd == "help") {

        // Fork a new process
        pid_t pid = fork();
        // Code executed by child process
        if (pid == 0) {
            // Handle I/O redirection
            redirection_handler(args);
            // Prepare arguments for more command
            char* more_args[] = {(char*)"more", (char*)"manual.txt", nullptr};
            // Execute more to display manual.txt
            execvp("more", more_args);
            // Print error if execvp fails
            perror("execvp");
            // Exit child process with error status
            exit(1);
        }
        // Code executed by parent process
        else {
            // If not running in background
            if (!background) {
                // Wait for child process
                wait(NULL);
            }
        }
        // Return from function
        return;
    }

    // 7.pause - pause shell
    else if (cmd == "pause") {
        // Prompt user to press enter
        printf("Press Enter to continue...\n");
        // Skip any characters until the user hits Enter
        while (true) {
            // Check if entered character is newline
            if (cin.get() == '\n') {
                // Break loop if enter is pressed
                break;
            }
        }
        // Return from function
        return;
    }

    // 8. quit - exit shell
    else if (cmd == "quit") {
        // Exit the shell program successfully
        exit(0);
    }

    // 9. other commands
    else {
        // Fork a new process
        pid_t pid = fork();
        // Code executed by child process
        if (pid == 0) {
            // Handle any I/O redirection
            redirection_handler(args);
            // Execute external command
            execvp(args[0], args);
            // Print error if execution fails
            perror("external command error");
            // Exit child process with error status
            exit(1);
        }
        // Code executed by parent process
        else {
            // If not running in background
            if (!background) {
                // Wait for child process to finish
                wait(NULL);
            }
        }
    }
}

// Main function entry point
int main(int argc, char* argv[]) {

    // Initialize PWD env variable
    setenv("PWD", getcwd(cwd, sizeof(cwd)), 1);

    // Initialize string variable to take in commands
    string line;
    // Check if file argument is provided for batch mode
    if (argc > 1) {
        // Open file stream for input file and pass the second argument as the file name
        ifstream file(argv[1]);

        // Check if file exists
        if (!file) {
            // Print error message
            printf("File does not exist\n");
            // Return from function
            return 1;
        }

        // Read file line by line
        while (getline(file, line)) {
            // Execute shell function for each line
            shell(line);
        }

    } else {
        // Activate interactive mode
        while (true) {
            // Print prompt with the current working directory
            printf("%s$ ", getcwd(cwd, sizeof(cwd)));
            // Read input from standard input and store it in the line variable
            if (!getline(cin, line)) {
                // Break if end of file or error
                break;
            }
            // Execute shell function and pass the command as a string
            shell(line);
        }
    }
    
    // Return zero indicating success
    return 0;
}