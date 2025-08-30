#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include <errno.h>

#define MAX_INPUT 256
#define MAX_PATH_LEN 512

// Function prototypes
void printPrompt();
void processCommand(char* command);
void executeBuiltinCommand(char* command);
void executeExternalCommand(char* command);
void changeDirectory(char* path);
void listDirectory();
void clearScreen();
void showHelp();

int main() {
    char input[MAX_INPUT];
    
    //Console title
    SetConsoleTitle("Hamoud Habibi's Mini Terminal");
    
    //Header MSG
    printf("___  ____           _______   _____                   _             _ \n");
    printf("|  \\/  (_)         (_)  _  \\ |_   _|                 (_)           | |\n");
    printf("| .  . |_ _ __ ___  _| | | |   | | ___ _ __ _ __ ___  _ _ __   __ _| |\n");
    printf("| |\\/| | | '_ ` _ \\| | | | |   | |/ _ \\ '__| '_ ` _ \\| | '_ \\ / _` | |\n");
    printf("| |  | | | | | | | | | |/ /    | |  __/ |  | | | | | | | | | | (_| | |\n");
    printf("\\_|  |_/_|_| |_| |_|_|___/     \\_/\\___|_|  |_| |_| |_|_|_| |_|\\__,_|_|\n");
    printf("______________________________________________________________________\n\n");
    printf("Type 'help' for available commands or 'exit' to quit.\n\n");
    
    while (1) {
        printPrompt();
        
        // --------------- Get user input --------------- //
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Remove newline character
            input[strcspn(input, "\n")] = 0;
            
            // Skip empty input
            if (strlen(input) == 0) {
                continue;
            }
            
            // Process the command
            processCommand(input);
        }
    }
    
    return 0;
}

void printPrompt() {
    char currentDir[MAX_PATH_LEN];
    if (_getcwd(currentDir, sizeof(currentDir)) != NULL) {
        printf("%s> ", currentDir);
    } else {
        printf("terminal> ");
    }
}

void processCommand(char* command) {
    if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0 || strcmp(command, "bye") == 0) {
        printf("Byee!\n");
        exit(0);
    }
    
    // Check for built in commands
    if (strncmp(command, "cd ", 3) == 0) {
        changeDirectory(command + 3);
    }
    else if (strcmp(command, "cd") == 0) {
        changeDirectory(getenv("USERPROFILE")); // Go to user home
    }
    else if (strcmp(command, "dir") == 0 || strcmp(command, "ls") == 0) {
        listDirectory();
    }
    else if (strcmp(command, "cls") == 0 || strcmp(command, "clear") == 0) {
        clearScreen();
    }
    else if (strcmp(command, "help") == 0) {
        showHelp();
    }
    else if (strcmp(command, "pwd") == 0) {
        char currentDir[MAX_PATH_LEN];
        if (_getcwd(currentDir, sizeof(currentDir)) != NULL) {
            printf("%s\n", currentDir);
        }
    }
    else {
        // Try to execute as external command
        executeExternalCommand(command);
    }
}

void changeDirectory(char* path) {
    if (path == NULL || strlen(path) == 0) {
        printf("Usage: cd <directory>\n");
        return;
    }
    
    // Remove quotes if they exist
    if (path[0] == '"' && path[strlen(path)-1] == '"') {
        path[strlen(path)-1] = '\0';
        path++;
    }
    
    if (_chdir(path) != 0) {
        printf("Cannot change directory to '%s': ", path);
        switch (errno) {
            case ENOENT:
                printf("Directory not found\n");
                break;
            case EACCES:
                printf("Access denied\n");
                break;
            default:
                printf("Unknown error\n");
                break;
        }
    }
}

void listDirectory() {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    
    hFind = FindFirstFile("*", &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error listing directory\n");
        return;
    }
    
    printf("\nDirectory listing:\n");
    printf("%-20s %-10s %-15s\n", "Name", "Type", "Size");
    printf("----------------------------------------\n");
    
    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            printf("%-20s %-10s %-15s\n", findFileData.cFileName, "<DIR>", "");
        } else {
            LARGE_INTEGER fileSize;
            fileSize.LowPart = findFileData.nFileSizeLow;
            fileSize.HighPart = findFileData.nFileSizeHigh;
            printf("%-20s %-10s %-15lld\n", findFileData.cFileName, "FILE", fileSize.QuadPart);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    
    FindClose(hFind);
    printf("\n");
}

void clearScreen() {
    system("cls");
}

void showHelp() {
    printf("\nAvailable commands:\n");
    printf("  help          - Show this help message\n");
    printf("  exit, quit    - Exit the terminal\n");
    printf("  cd <dir>      - Change directory\n");
    printf("  cd            - Go to home directory\n");
    printf("  dir, ls       - List directory contents\n");
    printf("  cls, clear    - Clear screen\n");
    printf("  pwd           - Show current directory\n");
    printf("  <command>     - Execute external command\n");
    printf("\nExamples:\n");
    printf("  cd C:\\Users\n");
    printf("  dir\n");
    printf("  notepad\n");
    printf("  ping google.com\n\n");
}

void executeExternalCommand(char* command) {
    int result = system(command);
    if (result == -1) {
        printf("Error executing command: %s\n", command);
    }
}