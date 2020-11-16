#include <stdio.h>

struct BPBInfo
{
    //offset 11
    unsigned char BPB_BytsPerSec[2];

    //offset 13
    unsigned char BPB_SecPerClus[1];

    //offset 14
    unsigned char BPB_RsvdSecCnt[2];

    // offset 16
    unsigned char BPB_NumFATs[1];

    // offset 17
    unsigned char BPB_RootEntCnt[2];

    // offset 36
    unsigned char BPB_FATSz32[4];

    //offset 44
    unsigned char BPB_RootClus[4];

    //offset 32
    unsigned char BPB_TotSec32[4];
};

struct DIRENTRY
{
    unsigned char DIR_name[11];
    unsigned char DIR_Attributes;

} __attribute__((packed));

struct BPBInfo bpbInfo;

void setBPBInfo(int file);

void getFileData(int file, int offset, char *destination, int bytes);

void displayBPBInfo();

void printRawData(char *data, int len);

void clusterCount();

void _testexit(void);
