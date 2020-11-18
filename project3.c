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
struct DirEntry dirEntry;

void setBPBInfo(int file)
{
    getFileData(utilityProps.file);
    getFirstDataSectorForCluster();

    utilityProps.currentCluster = bpbInfo.BPB_RootClus;
}

void getFileData(int file)
{
    unsigned char *info;

    lseek(file, 11, SEEK_SET);

    info = (unsigned char *)malloc(sizeof(unsigned char) * 512);

    read(file, info, 512);

    memcpy(&bpbInfo, info, sizeof(struct BPBInfo));
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

//get the value of the next cluster from the FAT
unsigned int getFatValueAtOffset(int file, unsigned int offset)
{

    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * 4);

    unsigned int fatValueOffset;

    lseek(file, offset, SEEK_SET);

    read(file, info, 4);

    memcpy(&fatValueOffset, info, sizeof(int));

    printf("\n fatValueOffset %u \n", fatValueOffset);

    return fatValueOffset;
}

//search for entry
struct DirEntry *searchSector(unsigned int clusterOffset, char *querytext)
{
    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERCLUSTER);

    int nextDataEntry = 0;

    for (int i = 0; i < ENTRYPERCLUSTER; i++)
    {
        //seeking by data entry which is 32 bytes
        lseek(utilityProps.file, clusterOffset + nextDataEntry, SEEK_SET);

        read(utilityProps.file, info, BYTESPERCLUSTER);

        memcpy(&dirEntry, info, sizeof(struct DirEntry));

        dirEntry.DIR_name[11] = '\0';
        // printf("dir name : %s", dirEntry.DIR_name);
        // printf("querytext: %s", querytext);

        if (strcmp(dirEntry.DIR_name, querytext) == 0)
        {
            // printf("dir name : %s", dirEntry.DIR_name);
            // printf("querytext: %s", querytext);

            printf("\nDIR_name : %s \n", dirEntry.DIR_name);
            // printf("DIR_FileSize : %u \n", dirEntry.DIR_FileSize);
            // printf("DIR_Attributes : %x \n", dirEntry.DIR_Attr & 0xff);
            printf("DIR_FstClusLO : %d \n", dirEntry.DIR_FstClusLO);
            printf("DIR_FstClusHI : %d \n", dirEntry.DIR_FstClusHI);
            i = ENTRYPERCLUSTER;
        }

        printf("\n");
        nextDataEntry += BYTESPERCLUSTER;
    }
}

/**
 * ls functions
*/
void lsCommand(struct CommandList *commandList)
{
    //utilityProps.currentCluster = 435; // using as a trigger to test each directory
    unsigned int bytesCount = -1;

    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);

    if (commandList->length < 2)
    {
        unsigned int nextCluster = utilityProps.currentCluster;

        printf("\n cluster Offset : %d \n", clusterOffset);

        bytesCount = listDataEntry(clusterOffset);

        //use to find each file of a cluster
        //runs unti the bytescount is not equal to 512
        while (bytesCount == bpbInfo.BPB_BytsPerSec || bytesCount == -1)
        {
            nextCluster = traverseCluster(nextCluster, &bytesCount);
        }
    }
    else if (commandList->length == 2)
    {
        searchSector(clusterOffset, commandList->commands[1]);
    }
    else
    {
        printf("ERROR: too many arguements");
    }
}

unsigned int traverseCluster(unsigned int cluster, unsigned int *bytesCount)
{

    unsigned int nextCluster;
    unsigned int clusterOffset;

    //if (nextCluster != utilityProps.currentCluster)
    //{
    //getting the FAT offset
    unsigned int fatOffset = getFatOffset(cluster);
    printf("\nfatOffset %u \n", fatOffset);

    //get the value of the next cluster from the FAT
    nextCluster = getFatValueAtOffset(utilityProps.file, fatOffset);
    printf("nextcluster : %u", nextCluster);
    // }

    /*
    * check to see if the FAT table says the end of cluster
    * has been reahced.
    */
    if (nextCluster >= ENDOFCLUSTER)
    {
        return -2; // -2 will stop the while loop
    }

    //getting the offset of the cluster
    clusterOffset = getClusterOffset(nextCluster);

    //reading the cluster
    (*bytesCount) = listDataEntry(clusterOffset);

    return nextCluster;
}

//listing  cluster entries
int listDataEntry(unsigned int clusterOffset)
{
    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERCLUSTER);

    int nextDataEntry = 0;

    for (int i = 0; i < ENTRYPERCLUSTER; i++)
    {
        //seeking by data entry which is 32 bytes
        lseek(utilityProps.file, clusterOffset + nextDataEntry, SEEK_SET);

        read(utilityProps.file, info, BYTESPERCLUSTER);

        memcpy(&dirEntry, info, sizeof(struct DirEntry));

        dirEntry.DIR_name[11] = '\0';

        if (strcmp(dirEntry.DIR_name, "") != 0)
        {
            printf("DIR_name : %s \n", dirEntry.DIR_name);
            // printf("DIR_FileSize : %u \n", dirEntry.DIR_FileSize);
            // printf("DIR_Attributes : %x \n", dirEntry.DIR_Attr & 0xff);
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

    return nextDataEntry;
}

/**
 * info commands
*/

void infoCommand()
{
    printf("Bytes per Sector: %u \n", bpbInfo.BPB_BytsPerSec);
    printf("Sectors per Cluster: %u \n", (unsigned int)bpbInfo.BPB_SecPerClus);
    printf("Reseverd Sector count: %u \n", bpbInfo.BPB_RsvdSecCnt);
    printf("Number of FATs: %u \n", (unsigned int)bpbInfo.BPB_NumFATs);
    // printf("BPB_RootEntCnt : %u \n", bpbInfo.BPB_RootEntCnt);
    printf("Total Sectors : %u \n", bpbInfo.BPB_TotSec32);
    printf("FATsize : %u \n", bpbInfo.BPB_FATSz32);
    printf("Root Cluster  : %u \n", bpbInfo.BPB_RootClus);
}