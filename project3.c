#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "project3.h"

void setBPBInfo(int file)
{
    getFileData(file, 11, bpbInfo.BPB_BytsPerSec, 2);
    getFileData(file, 13, bpbInfo.BPB_SecPerClus, 1);
    getFileData(file, 14, bpbInfo.BPB_RsvdSecCnt, 2);
    getFileData(file, 16, bpbInfo.BPB_NumFATs, 1);
    getFileData(file, 17, bpbInfo.BPB_RootEntCnt, 2);
    getFileData(file, 36, bpbInfo.BPB_FATSz32, 4);
    getFileData(file, 44, bpbInfo.BPB_RootClus, 4);
    getFileData(file, 32, bpbInfo.BPB_TotSec32, 4);
}

void getFileData(int file, int offset, char *destination, int bytes)
{
    unsigned char *info;

    lseek(file, offset, SEEK_SET);

    info = (unsigned char *)malloc(sizeof(unsigned char) * bytes);

    read(file, info, bytes);

    memcpy(destination, info, bytes);
}

void getFileData(int file)
{
    unsigned char *info;

    lseek(file, offset, SEEK_SET);

    info = (unsigned char *)malloc(sizeof(unsigned char) * bytes);

    read(file, info, bytes);

    memcpy(destination, info, bytes);
}

void displayBPBInfo()
{

    printf("BPB_BytsPerSec : ");
    printRawData(bpbInfo.BPB_BytsPerSec, 2);

    printf("BPB_SecPerClus : ");
    printRawData(bpbInfo.BPB_SecPerClus, 1);

    printf("BPB_RsvdSecCnt : ");
    printRawData(bpbInfo.BPB_RsvdSecCnt, 2);

    printf("BPB_NumFATs : ");
    printRawData(bpbInfo.BPB_NumFATs, 1);

    printf("BPB_FATSz32 : ");
    printRawData(bpbInfo.BPB_FATSz32, 4);

    printf("BPB_RootClus : ");
    printRawData(bpbInfo.BPB_RootClus, 4);

    printf("BPB_TotSec32 : ");
    printRawData(bpbInfo.BPB_TotSec32, 4);
}

void printRawData(char *data, int len)
{

    //int len = sizeof(data) / sizeof(*data);

    for (int i = 0; i < len; i++)
    {
        printf("%02x ", data[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}

void clusterCount()
{

    printf("cluster count");
}

void _testexit(void)
{
    int RootDirSectors = 0;
    printf("Hello world!");
}