#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "project3.h"

struct CommandList *getCommands(char *userInput);
int getFile(char *filePath);
void add_command(struct CommandList *commandList, char *item);
struct CommandList *new_commandList(void);

struct CommandList
{
    int length;
    char **commands;
};

int main(int argc, char **argv)
{

    char command[100];

    char **commands;
    unsigned char *BPB_BytesPerSec;

    int file;
    boolean stop = false;

    if (argv[1] == NULL)
    {
        printf("Missing arguement\n");
        return -1;
    }

    file = getFile(argv[1]);

    setBPBInfo(file);

    do
    {
        struct CommandList *commandList = new_commandList();

        // printf("\n$ ");

        scanf("$  %s", command);

        //printf("********** %s", command);

        commandList = getCommands(command);

        // printf("%u", sizeof(command));
        printf("%d", commandList->length);

        if (strcmp(command, "info") == 0)
        {
        }
        else if (strcmp(command, "ls") == 0)
        {
        }
        else if (strcmp(command, "exit") == 0)
        {
            stop = false;
            break;
        }

    } while (stop != true);

    close(file);

    return 0;
}

int getFile(char *filePath)
{
    int file = open(filePath, O_RDWR);

    if (file == -1)
    {
        fprintf(stderr, "File doesn't exit\n");
        exit(0);
    }

    return file;
}

struct CommandList *getCommands(char *userInput)
{
    struct CommandList *commandList = new_commandList();
    int count = 0;

    // userInput = strtok(NULL, " ");

    while (userInput != NULL)
    {

        add_command(commandList, userInput);
        userInput = strtok(NULL, " ");

        // printf("---%s\n", userInput);
    }
    //commandList.length = count;

    return commandList;
}

void add_command(struct CommandList *commandList, char *item)
{
    printf("\n^^^^\n");
    int i = commandList->length;
    commandList->commands = (char **)realloc(commandList->commands, (i + 2) * sizeof(char *));
    commandList->commands[i] = (char *)malloc(strlen(item) + 1);
    commandList->commands[i + 1] = NULL;
    strcpy(commandList->commands[i], item);

    commandList->length += 1;
}

struct CommandList *new_commandList(void)
{
    struct CommandList *commandList = (struct CommandList *)malloc(sizeof(struct CommandList));
    commandList->length = 0;
    commandList->commands = (char **)malloc(sizeof(char *));
    commandList->commands[0] = NULL; /* make NULL terminated */
    return commandList;
}