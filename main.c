#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "project3.h"

void getCommands(char **commands, char command_string[]);

int main(int argc, char **argv)
{

    char command[100];
    char baseCommand[100];
    char **commands;
    unsigned char *BPB_BytesPerSec;

    int pf;

    if (argv[1] == NULL)
    {
        printf("Missing arguement\n");
        return -1;
    }

    pf = open(argv[1], O_RDWR);

    if (pf == -1)
    {
        fprintf(stderr, "File doesn't exit\n");
        return -1;
    }

    setBPBInfo(pf);

    //displayBPBInfo();

    printf("\n");

    printf("\n getFirstDataSectorForCluster : %d \n", utilityProps.firstDataSector);
    unsigned int rootClusterOffset = getClusterOffset(2);

    printf("\n rootCluster Offset : %d \n", rootClusterOffset);

    listDataEntry(pf, rootClusterOffset);

    // while (strcmp(command[0], "exit"))
    // {

    //     printf("$ ");
    //     scanf("%s", command);

    //     //getCommands(commands,);

    //     if (strcmp(command, "info") == 0)
    //     {
    //     }
    //     else if (strcmp(command, "ls") == 0)
    //     {
    //     }
    //     else if (strcmp(command, "exit") == 0)
    //     {

    //         _testexit();
    //     }
    // }

    close(pf);

    return 0;
}

void getCommands(char **commands, char command_string[])
{
}