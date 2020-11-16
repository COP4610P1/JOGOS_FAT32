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
        //printf("File doesn't exit\n");
        fprintf(stderr, "File doesn't exit\n");
        return -1;
    }
    lseek(pf, 13, SEEK_SET);

    BPB_BytesPerSec = (unsigned char *)malloc(sizeof(unsigned char) * 1);

    // int test = read(pf, BPB_BytesPerSec, 1);
    // BPB_BytesPerSec[test] = '\0';

    //fwrite(BPB_BytesPerSec, 1, 10000, stdout);
    //malloc(bpbInfo.BPB_SecPerClus, sizeof(unsigned char) * 2);

    memcpy(&bpbInfo.BPB_SecPerClus, BPB_BytesPerSec, sizeof(bpbInfo.BPB_SecPerClus));

    //bpbInfo.BPB_BytsPerSec[3] = '\0';

    //fwrite(bpbInfo.BPB_SecPerClus, 1, sizeof(bpbInfo.BPB_SecPerClus), stdout);

    //  lseek(pf, 14, SEEK_SET);
    //  BPB_BytesPerSec = (unsigned char *)malloc(sizeof(unsigned char) * 1);

    // int test = read(pf, BPB_BytesPerSec, 2);

    // memcpy(&bpbInfo.BPB_RsvdSecCnt, BPB_BytesPerSec, sizeof(bpbInfo.BPB_RsvdSecCnt));

    setBPBInfo(pf);
    displayBPBInfo();
    clusterCount();

    // printf("$ from struct %d\n", (bpbInfo.BPB_SecPerClus[0] * 2));

    // for (int i = 0; i < 1; i++)
    // {
    //     printf("%02x ", BPB_BytesPerSec[i]);
    //     if ((i + 1) % 16 == 0)
    //         printf("\n");
    // }
    printf("\n");
    // printf("$ 0x%s, %d", BPB_BytesPerSec, test);

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