#include <stdio.h>
#include <string.h>
#include "project3.h"

int main()
{
    char command[100];

    while (command != "exit")
    {
        printf("$ ");
        scanf("%s",command);

        if (strcmp(command,"info") == 0)
        {
        }
        else if (strcmp(command,"ls") == 0)
        {
        }
        else if (strcmp(command,"exit") == 0)
        {
            
            _testexit();
        }
<<<<<<< Updated upstream
=======
        else if (strcmp(commandList->commands[0], "cd") == 0)
        {
            cdCommand(commandList);
        }
        else if (strcmp(commandList->commands[0], "creat") == 0)
        {
            creatCommand(commandList);
        }
        else if (strcmp(commandList->commands[0], "mkdir") == 0)
        {
            mkdirCommand(commandList);
        }
        else if (strcmp(commandList->commands[0], "rm") == 0)
        {
            struct CommandList *temp = new_commandList();
            userInput = "ls";
            temp = getCommands(userInput);
            lsCommand2(temp, commandList);
            //rmCommand();
        }
        else if (strcmp(commandList->commands[0], "exit") == 0)
        {
            stop = false;
            break;
        }
        else
        {
            printf("\nERROR : Unknown command: %s", userInput);
        }

    } while (stop != true);

    close(utilityProps.file);

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

/**
 * getting each command entered from the user
*/
struct CommandList *getCommands(char *userInput)
{

    // allocating memor for list
    struct CommandList *commandList = new_commandList();

    //buffer for traversing the user input
    char *buf = (char *)malloc(strlen(userInput) + 1);

    strcpy(buf, userInput);

    // storing each token from user input to add to the commands list
    char *item = strtok(buf, " ");

    // loops until all token is taken from buf and stored.
    while (item != NULL)
    {
        //adding token to the command list
        add_command(commandList, item);
        item = strtok(NULL, " ");
    }

    //freeing buffer pointer variable
    free(buf);

    return commandList;
}

/**
 * adding a command to the list of commands entered.
*/
void add_command(struct CommandList *commandList, char *item)
{

    int i = commandList->length;
    commandList->commands = (char **)realloc(commandList->commands, (i + 2) * sizeof(char *));
    commandList->commands[i] = (char *)malloc(strlen(item) + 1);
    commandList->commands[i + 1] = NULL;
    strcpy(commandList->commands[i], item);

    commandList->length += 1;
}

/*
 * allocating memory the list of commands
 */
struct CommandList *new_commandList(void)
{
    struct CommandList *commandList = (struct CommandList *)malloc(sizeof(struct CommandList));
    commandList->length = 0;
    commandList->commands = (char **)malloc(sizeof(char *));
    commandList->commands[0] = NULL; /* make NULL terminated */
    return commandList;
}

/**
 * get input from user
 * code from first project
*/
char *getInput(void)
{
    char *buffer = NULL;
    int bufsize = 0;

    char line[5];
    while (fgets(line, 5, stdin) != NULL)
    {
        int addby = 0;
        char *newln = strchr(line, '\n');
        if (newln != NULL)
            addby = newln - line;
        else
            addby = 5 - 1;

        buffer = (char *)realloc(buffer, bufsize + addby);
        memcpy(&buffer[bufsize], line, addby);
        bufsize += addby;

        if (newln != NULL)
            break;
>>>>>>> Stashed changes
    }

    return 0;
}