#include <stdio.h>

#define BYTESPERCLUSTER 32
#define ENTRYPERCLUSTER 16

struct BPBInfo
{
    //offset 11
    unsigned short BPB_BytsPerSec;

    //offset 13
    unsigned char BPB_SecPerClus;

    //offset 14
    unsigned short BPB_RsvdSecCnt;

    // offset 16
    unsigned char BPB_NumFATs;
    // offset 17
    unsigned short BPB_RootEntCnt;

    unsigned short BPB_TotSec16;

    unsigned char BPB_Media;

    unsigned short BPB_FATSz16;

    unsigned short BPB_SecPerTrk;

    unsigned short BPB_NumHeads;

    unsigned int BPB_HiddSec;

    //offset 32
    unsigned int BPB_TotSec32;

    // offset 36
    unsigned int BPB_FATSz32;

    unsigned short BPB_ExtFlags;

    unsigned short BPB_FSVer;

    //offset 44
    unsigned int BPB_RootClus;

} __attribute__((packed));

struct DIRENTRY
{
    unsigned char DIR_name[11];
    unsigned char DIR_Attributes;
    unsigned char DIR_NTRes;
    unsigned char DIR_CrtTimeTenth;
    unsigned short DIR_CrtTime;
    unsigned short DIR_CrtDate;
    unsigned short DIR_LstAccDate;
    unsigned short DIR_FstClusHI;
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClusLO;
    unsigned int DIR_FileSize;

} __attribute__((packed));

struct UtilityProps
{

    unsigned int rootDirSectors;
    unsigned int dataSec;
    unsigned int countOfClusters;
    unsigned int firstDataSector;
};

//variables
struct UtilityProps utilityProps;

struct BPBInfo bpbInfo;
struct DIRENTRY dirEntry;

void setBPBInfo(int file);

void getFileData(int file);

void displayBPBInfo();

void printRawData(char *data, int len);

void clusterCount();

int getFatOffset(int cluster);

int getFirstDataSectorForCluster();

unsigned int getClusterOffset(int cluster);

void listDataEntry(int file, int clusterOffset);
