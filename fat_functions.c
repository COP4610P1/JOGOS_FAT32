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

/**
 * Get the FAT location for a cluster value
*/
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

/**
 * Searches the sector for the queryString.
 * Returns the entry of the result
 * bytesCount stores the amount of bytes read for 
 * checking if the cluster has ended
*/
struct DirEntry *searchSectorEntry(unsigned int clusterOffset, char *querytext, unsigned int *bytesCount)
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

        dirEntry.DIR_name[10] = '\0';
        trimString(dirEntry.DIR_name);

        //  printf("dir name size %d", (int)sizeof(dirEntry.DIR_name));

        if (strcmp(dirEntry.DIR_name, querytext) == 0)
        {
            memcpy(resultEntry, &dirEntry, sizeof(struct DirEntry));

            // printf("\nDIR_name : %s \n", resultEntry->DIR_name);
            // printf("DIR_FileSize : %u \n", resultEntry->DIR_FileSize);
            //printf("DIR_Attributes : %x \n", resultEntry->DIR_Attr & 0xff);
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

/**
 * Searches the cluster for the queryString.
 * Returns the offset of the result
 * bytesCount stores the amount of bytes read for 
 * checking if the cluster has ended
*/
unsigned int searchSectorOffset(unsigned int clusterOffset, char *queryString, unsigned int *bytesCount)
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

        if (strcmp(dirEntry.DIR_name, queryString) == 0)
        {
            unsigned int newClusterValue;
            unsigned int emptyFATOffset = traverseFAT(&newClusterValue);

            printf("newClusterValue: %u \n", newClusterValue);
            printf("New Cluster Offset: %u \n", getClusterOffset(newClusterValue));
            i = 0;
            break;
        }
        //setting the next offset to look at
        nextDataEntry += BYTESPERENTRY;
    }

    (*bytesCount) = nextDataEntry;
    //return clusterOffset;
    return clusterOffset + nextDataEntry;
}

/*
* listing  cluster entries
*/
int listDataEntry(unsigned int clusterOffset)
{
    //printf("\ncluster offset \n %u", clusterOffset);
    unsigned char *info = (unsigned char *)malloc(sizeof(unsigned char) * BYTESPERENTRY);

    int nextDataEntry = 0;
    struct DirEntry entry;

    for (int i = 0; i < ENTRYPERCLUSTER; i++)
    {
        //seeking by data entry which is 32 bytes
        lseek(utilityProps.file, clusterOffset + nextDataEntry, SEEK_SET);

        read(utilityProps.file, &entry, BYTESPERENTRY);

        //memcpy(, info, sizeof(struct DirEntry));

        //TODO entry.DIR_name[11] = '\0';
        //strncmp()
        //don't worry about empty names
        //apply the long name
        //attribute & LONGNAME
        //test << 4

        //hi and low
        if (strcmp(entry.DIR_name, "") != 0)
        {
            // if (LONGNAME == (entry.DIR_Attr & LONGNAME))
            // {
            trimString(entry.DIR_name);
            printf("%s  ", entry.DIR_name);
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

/**
 * Traverse the FAT until a empty slot is found
*/
unsigned int traverseFAT(unsigned int *newClusterValue)
{

    unsigned int lastFATValue = 2;

    //get the offset/location of the root
    unsigned int fatOffset = getFatOffset(lastFATValue);
    printf("current root offset  %u", fatOffset);

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

            //printf("empty offset  %u \n", fatOffset);

            //printf("newClusterValue: %d \n", i + 1);
            (*newClusterValue) = i + 1;
            break;
        }
        else
        {
            fatOffset += 4;
        }
    }

    return fatOffset;
}

/**
 * returns the new cluster number and offset of the 
 * cluster through the params
 * 
 * updating the FAT
*/
unsigned int setNewFATValue(unsigned int cluster)
{

    unsigned int newClusterValue;
    unsigned int emptyFATOffset = traverseFAT(&newClusterValue);

    printf("newClusterValue: %u \n", newClusterValue);
    // printf("+1 Cluster Offset: %u \n", getClusterOffset(newClusterValue + 1));
    printf("New Cluster Offset: %u \n", getClusterOffset(newClusterValue));

    lseek(utilityProps.file, emptyFATOffset, SEEK_SET);
    write(utilityProps.file, &cluster, BYTES4);

    return newClusterValue;
}

/**
 * create new direntry 
 * Params : name, nextCluster, dir attr
*/
struct DirEntry *createNewDirEntryStruct(char *name, unsigned int nextCluster, unsigned int attr)
{

    //creating a new file
    struct DirEntry *dirEntry = (struct DirEntry *)malloc(sizeof(struct DirEntry));

    strcpy(dirEntry->DIR_name, name);
    dirEntry->DIR_FileSize = 0;
    dirEntry->DIR_FstClusHI = nextCluster;
    dirEntry->DIR_FstClusLO = 0;
    dirEntry->DIR_Attr = attr;
    dirEntry->DIR_NTRes = 0;
    dirEntry->DIR_CrtTimeTenth = 0;
    dirEntry->DIR_CrtTime = 0;
    dirEntry->DIR_CrtDate = 0;
    dirEntry->DIR_LstAccDate = 0;
    dirEntry->DIR_WrtTime = 0;
    dirEntry->DIR_WrtDate = 0;

    return dirEntry;
}

/**
 * This function add a new directory or file to a cluster
*/
void addDirEntry(struct DirEntry *newDirEntry, unsigned int cluster)
{
    unsigned int bytesCount;
    unsigned int newClusterValue;

    //using the currentCluster property to traverse to each cluster
    unsigned int currentCluster = cluster;
    unsigned int lastCluster = cluster;

    //find the next free entry in the cluster
    unsigned int freeEntryOffset = searchSectorOffset(getClusterOffset(currentCluster),
                                                      "", &bytesCount);

    while ((bytesCount == bpbInfo.BPB_BytsPerSec))
    {
       // printf("\nenter loop here\n");
        currentCluster = getNextCluster(currentCluster);
        if (currentCluster != -2)
        {

           // printf("\ncurrent cluster offset  %u\n", getClusterOffset(currentCluster));
            freeEntryOffset = searchSectorOffset(getClusterOffset(currentCluster),
                                                 "", &bytesCount);
        }
        else
        {
            //printf("end of cluster");
            // currentCluster = utilityProps.currentCluster;
            //set the new FAT value
            newClusterValue = setNewFATValue(NEWCLUSTER);
            unsigned int fatOffset = getFatOffset(lastCluster);

            //modify the last FATValue that associated with the current cluster.
            lseek(utilityProps.file, fatOffset, SEEK_SET);
            write(utilityProps.file, &newClusterValue, BYTES4);
        }

        lastCluster = currentCluster;
    }

    //printf("free entry space %u", freeEntryOffset);

    //finding the location and writing to the file
    lseek(utilityProps.file, freeEntryOffset, SEEK_SET);

    write(utilityProps.file, newDirEntry, sizeof(struct DirEntry));
}


int listDataEntry2(unsigned int clusterOffset, char * queryString)
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
            if (strcmp(dirEntry.DIR_name, queryString) == 0 && dirEntry.DIR_Attr != 16)
            {
              printf("%s  \n", dirEntry.DIR_name);
              //printf("nextDataEntry: %d \n", nextDataEntry);
              struct DirEntry *emptyDirEntry = (struct DirEntry *)malloc(sizeof(struct DirEntry));

              emptyDirEntry->DIR_name[0] = '\0';
              //printf("DirName: %s", emptyDirEntry->DIR_name);
              emptyDirEntry->DIR_FileSize = 0;
              emptyDirEntry->DIR_FstClusHI = 0;
              emptyDirEntry->DIR_FstClusLO = 0;
              emptyDirEntry->DIR_Attr = 0;
              emptyDirEntry->DIR_NTRes = 0;
              emptyDirEntry->DIR_CrtTimeTenth = 0;
              emptyDirEntry->DIR_CrtTime = 0;
              emptyDirEntry->DIR_CrtDate = 0;
              emptyDirEntry->DIR_LstAccDate = 0;
              emptyDirEntry->DIR_WrtTime = 0;
              emptyDirEntry->DIR_WrtDate = 0;

              lseek(utilityProps.file, clusterOffset + nextDataEntry, SEEK_SET);

              write(utilityProps.file, emptyDirEntry, sizeof(struct DirEntry));
              break;
            }
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
    //printf("nextDataEntry: %d \n", nextDataEntry);
    return nextDataEntry + clusterOffset;
}



unsigned int getEmptyEntryOffset(unsigned cluster)
{
}