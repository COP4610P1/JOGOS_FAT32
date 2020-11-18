/**
 * There is only one sector per cluster
 *  each cluster/sector has data at the root
 * each data entry is 32 byte 
 * each cluster is 512 byte
 * 512/32 = 16 bytes 
 * root cluster is cluster 2 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "project3.h"

//variables
struct UtilityProps utilityProps;

struct BPBInfo bpbInfo;
struct DIRENTRY dirEntry;

void setBPBInfo(int file)
{
    getFileData(file);
    displayBPBInfo();
    getFirstDataSectorForCluster();
}

void getFileData(int file)
{
    unsigned char *info;

    lseek(file, 11, SEEK_SET);

    info = (unsigned char *)malloc(sizeof(unsigned char) * 512);

    read(file, info, 512);

    memcpy(&bpbInfo, info, sizeof(struct BPBInfo));
}

void displayBPBInfo()
{
    printf("\n****basic info****\n");
    printf("BPB_BytsPerSec : %d \n", bpbInfo.BPB_BytsPerSec);
    printf("BPB_SecPerClus : %d \n", (unsigned int)bpbInfo.BPB_SecPerClus);
    printf("BPB_RsvdSecCnt : %d \n", bpbInfo.BPB_RsvdSecCnt);
    printf("BPB_NumFATs : %d \n", (unsigned int)bpbInfo.BPB_NumFATs);
    printf("BPB_RootEntCnt : %d \n", bpbInfo.BPB_RootEntCnt);
    printf("BPB_TotSec32 : %d \n", bpbInfo.BPB_TotSec32);
    printf("BPB_RootClus : %d \n", bpbInfo.BPB_RootClus);
    printf("BPB_FATSz32 : %d \n", bpbInfo.BPB_FATSz32);
}

void clusterCount()
{
    printf("\n****getting cluster count****");
    //fat32 root directory sectors will always be zero
    utilityProps.rootDirSectors = 0;
    printf("\nrootDirSectors count : %d ", utilityProps.rootDirSectors);
    utilityProps.dataSec = bpbInfo.BPB_TotSec32 -
                           (bpbInfo.BPB_RsvdSecCnt + (bpbInfo.BPB_NumFATs * bpbInfo.BPB_FATSz32) +
                            utilityProps.rootDirSectors);

    printf("\ndataSec count : %d ", utilityProps.dataSec);

    utilityProps.countOfClusters = utilityProps.dataSec / (unsigned int)bpbInfo.BPB_SecPerClus;

    printf("\ncountOfClusters count : %d ", utilityProps.countOfClusters);
}

int getFatOffset(int cluster)
{
    return (bpbInfo.BPB_RsvdSecCnt *
            bpbInfo.BPB_BytsPerSec) +
           (cluster * 4);
}

// reflect slide 13 in BPB commands
int getFirstDataSectorForCluster()
{
    utilityProps.firstDataSector =
        bpbInfo.BPB_RsvdSecCnt + (bpbInfo.BPB_NumFATs * bpbInfo.BPB_FATSz32);

    return utilityProps.firstDataSector;
}

//getting the the cluster/sector offset for any cluster
unsigned int getClusterOffset(int cluster)
{
    // getting the byteoffset for the cluster/sector; slide 15
    unsigned int byteOffset = utilityProps.firstDataSector + ((cluster - 2) * (unsigned int)bpbInfo.BPB_SecPerClus);
    return byteOffset * bpbInfo.BPB_BytsPerSec;
}

void listDataEntry(int file, int clusterOffset)
{

    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERCLUSTER);

    int nextDataEntry = 0;
    int morefiles = 1;
    while (morefiles != 0)
    {

        for (int i = 0; i < ENTRYPERCLUSTER; i++)
        {
            //seeking by data entry which is 32 bytes
            lseek(file, clusterOffset + nextDataEntry, SEEK_SET);

            read(file, info, BYTESPERCLUSTER);

            memcpy(&dirEntry, info, sizeof(struct DIRENTRY));

            dirEntry.DIR_name[11] = '\0';

            // int test;
            // memcpy(&test, &dirEntry.DIR_Attr, 1);

            // printf("test %d \n", (unsigned int)dirEntry.DIR_Attr);

            if (strcmp(dirEntry.DIR_name, "") != 0)
            {
                printf("DIR_name : %s \n", dirEntry.DIR_name);
                printf("DIR_FileSize : %u \n", dirEntry.DIR_FileSize);
                printf("DIR_Attributes : %x \n", dirEntry.DIR_Attr & 0xff);
                printf("DIR_FstClusLO : %d \n", dirEntry.DIR_FstClusLO);
                printf("DIR_FstClusHI : %d \n", dirEntry.DIR_FstClusHI);
            }
            else
            {
                break;
            }

            printf("\n");
            nextDataEntry += BYTESPERCLUSTER;
        }

        // checking if the cluster has any other associated with it
        // we will have to check the fat table for the next offset
        if (nextDataEntry == bpbInfo.BPB_BytsPerSec)
        {
            //printf("\n clusterOffset %u \n", clusterOffset);
            unsigned int fatOffset = getFatOffset(435);
            printf("\nfatOffset %u \n", fatOffset);
            clusterOffset = getFatValueAtOffset(file, fatOffset);
        }
        else
        {
            morefiles = 0;
        }
    }
}

unsigned int getFatValueAtOffset(int file, unsigned int offset)
{

    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * 4);

    unsigned int fatValueOffset;

    lseek(file, offset, SEEK_SET);

    read(file, info, 4);

    memcpy(&fatValueOffset, info, sizeof(int));

    printf("\n fatValueOffset %u \n", fatValueOffset);

    return getClusterOffset(fatValueOffset);
}