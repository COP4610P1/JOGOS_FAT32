#ifndef FATFUNC
#define FATFUNC

#include "project3.h"

#define BYTESPERENTRY 32
#define ENTRYPERCLUSTER 16
#define ENTRYPERCLUSTER 16
#define ENDOFCLUSTER 268435448 // 0FFFFFF8
#define NEWCLUSTER 4294967295  //FFFFFFFF

#define ATTR_READ_ONLY 01
#define ATTR_HIDDEN 02
#define ATTR_SYSTEM 04
#define ATTR_VOLUME_ID 08
#define ATTR_DIRECTORY 16
#define ATTR_ARCHIVE 32

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

struct DirEntry
{
    unsigned char DIR_name[11];

    unsigned char DIR_Attr;

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

int getFatOffset(int cluster);

int getFirstDataSectorForCluster();

unsigned int getClusterOffset(int cluster);

unsigned int getFatValueAtOffset(unsigned int offset);
unsigned int getNextCluster(unsigned int cluster);

//search for entry
struct DirEntry *searchSector(unsigned int clusterOffset, char *querytext, unsigned int *bytesCount);

int listDataEntry(unsigned int clusterOffset);

unsigned int displayCluster(unsigned int cluster, unsigned int *bytesCount);

unsigned int traverseFAT();

unsigned int traverseCluster(unsigned int clusterOffset);

#endif