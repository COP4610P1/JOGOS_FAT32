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
#include "fat_functions.h"

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
unsigned int getFatValueAtOffset(unsigned int offset)
{

    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * 4);

    unsigned int fatValueOffset;

    lseek(utilityProps.file, offset, SEEK_SET);

    read(utilityProps.file, info, 4);

    memcpy(&fatValueOffset, info, sizeof(int));

    //printf("\n fatValueOffset %u \n", fatValueOffset);

    return fatValueOffset;
}

//get the next cluster
unsigned int getNextCluster(unsigned int cluster)
{
    unsigned int nextCluster;
    unsigned int clusterOffset;

    unsigned int fatOffset = getFatOffset(cluster);
    nextCluster = getFatValueAtOffset(fatOffset);

    /*
    * check to see if the FAT table says the end of cluster
    * has been reahced.
    */
    if (nextCluster >= ENDOFCLUSTER)
    {
        return -2; // -2 will stop the while loop
    }

    return nextCluster;
}

//search for entry
struct DirEntry *searchSector(unsigned int clusterOffset, char *querytext, unsigned int *bytesCount)
{
    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERENTRY);

    int nextDataEntry = 0;

    struct DirEntry *resultEntry = (struct DirEntry *)malloc(sizeof(struct DirEntry));

    for (int i = 0; i < ENTRYPERCLUSTER; i++)
    {
        //seeking by data entry which is 32 bytes
        lseek(utilityProps.file, clusterOffset + nextDataEntry, SEEK_SET);

        read(utilityProps.file, info, BYTESPERENTRY);

        memcpy(&dirEntry, info, sizeof(struct DirEntry));

        dirEntry.DIR_name[11] = '\0';
        trimString(dirEntry.DIR_name);

        //  printf("dir name size %d", (int)sizeof(dirEntry.DIR_name));

        if (strcmp(dirEntry.DIR_name, querytext) == 0)
        {
            memcpy(resultEntry, &dirEntry, sizeof(struct DirEntry));

            // printf("\nDIR_name : %s \n", resultEntry->DIR_name);
            // printf("DIR_FileSize : %u \n", resultEntry->DIR_FileSize);
            // printf("DIR_Attributes : %x \n", resultEntry->DIR_Attr & 0xff);
            // printf("DIR_FstClusLO : %d \n", resultEntry->DIR_FstClusLO);
            // printf("DIR_FstClusHI : %d \n", resultEntry->DIR_FstClusHI);
        }
        else if (strcmp(dirEntry.DIR_name, "") == 0)
        {
            break;
        }

        //printf("\n");
        nextDataEntry += BYTESPERENTRY;
    }

    (*bytesCount) = nextDataEntry;

    return resultEntry;
}

/**
 * traversing to the different cluster by
 * going to the FAT table
*/
unsigned int displayCluster(unsigned int cluster, unsigned int *bytesCount)
{
    unsigned int nextCluster = cluster;
    unsigned int clusterOffset;

    //use to find each file of a cluster
    //runs until the bytescount is not equal to 512
    while ((*bytesCount) == bpbInfo.BPB_BytsPerSec || (*bytesCount) == -1)
    {
        nextCluster = getNextCluster(nextCluster);

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
    }

    return nextCluster;
}

unsigned int traverseCluster(unsigned int clusterOffset)
{
    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERENTRY);

    int nextDataEntry = 0;

    for (int i = 0; i < ENTRYPERCLUSTER; i++)
    {
        //seeking by data entry which is 32 bytes
        //clusterOffset += nextDataEntry;
        lseek(utilityProps.file, clusterOffset + nextDataEntry, SEEK_SET);

        read(utilityProps.file, info, BYTESPERENTRY);

        memcpy(&dirEntry, info, sizeof(struct DirEntry));

        dirEntry.DIR_name[11] = '\0';

        if (strcmp(dirEntry.DIR_name, "") == 0)
        {
            break;
        }
        if (i+1 == 16)
        {
          unsigned int newClusterValue;
          unsigned int emptyFATOffset = traverseFAT(&newClusterValue);

          printf("newClusterValue: %u \n", newClusterValue);
          printf("New Cluster Offset: %u \n", getClusterOffset(newClusterValue));
          i = 0;
        }

        //setting the next offset to look at
        nextDataEntry += BYTESPERENTRY;
    }

    //return clusterOffset;
    return clusterOffset + nextDataEntry;
}

//listing  cluster entries
int listDataEntry(unsigned int clusterOffset)
{
    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERENTRY);

    int nextDataEntry = 0;

    for (int i = 0; i < ENTRYPERCLUSTER; i++)
    {
        //seeking by data entry which is 32 bytes
        lseek(utilityProps.file, clusterOffset + nextDataEntry, SEEK_SET);

        read(utilityProps.file, info, BYTESPERENTRY);

        memcpy(&dirEntry, info, sizeof(struct DirEntry));

        dirEntry.DIR_name[11] = '\0';

        if (strcmp(dirEntry.DIR_name, "") != 0)
        {
            trimString(dirEntry.DIR_name);
            printf("%s  ", dirEntry.DIR_name);
            // printf("DIR_FileSize : %u \n", dirEntry.DIR_FileSize);
            // printf("DIR_Attributes : %x \n", dirEntry.DIR_Attr & 0xff);
            // printf("DIR_FstClusLO : %d \n", dirEntry.DIR_FstClusLO);
            //printf("DIR_FstClusHI : %d \n", dirEntry.DIR_FstClusHI);
        }
        else
        {
            break;
        }
        //setting the next offset to look at
        nextDataEntry += BYTESPERENTRY;
    }

    return nextDataEntry;
}

unsigned int traverseFAT(unsigned int *newClusterValue)
{

    unsigned int lastFATValue = 2;

    //get the offset/location of the root
    unsigned int fatOffset = getFatOffset(lastFATValue);
    printf("offset  %u", fatOffset);

    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERENTRY);

    //traversing through the  FAT
    for (int i = 0; i < bpbInfo.BPB_FATSz32; i++)
    {

        lseek(utilityProps.file, fatOffset, SEEK_SET);

        read(utilityProps.file, info, sizeof(lastFATValue));

        memcpy(&lastFATValue, info, sizeof(lastFATValue));

        if (lastFATValue == 0)
        {
            printf("\n %x \n", lastFATValue);

            printf("offset  %u \n", fatOffset);

            printf("newClusterValue: %d \n", i+1);
            (*newClusterValue) = i+1;
            break;
        }
        else
        {
            fatOffset += 4;
        }
    }

    return fatOffset;
}
