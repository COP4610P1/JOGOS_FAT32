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

/**
 * ls functions
*/
void lsCommand(struct CommandList *commandList)
{
    // utilityProps.currentCluster = 435; // using as a trigger to test each directory
    unsigned int bytesCount = -1;
    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);
    unsigned int currentCluster = utilityProps.currentCluster;

    struct DirEntry *resultEntry;

    if (commandList->length < 2)
    {
        bytesCount = listDataEntry(clusterOffset);
        currentCluster = traverseCluster(currentCluster, &bytesCount);
    }
    else if (commandList->length == 2)
    {
        resultEntry = searchSector(clusterOffset, commandList->commands[1], &bytesCount);

        while ((bytesCount == bpbInfo.BPB_BytsPerSec || bytesCount == -1))
        {

            currentCluster = getNextCluster(currentCluster);
            resultEntry = searchSector(getClusterOffset(currentCluster),
                                       commandList->commands[1], &bytesCount);
        }

        if (resultEntry != NULL)
        {

            currentCluster = (resultEntry->DIR_FstClusLO > resultEntry->DIR_FstClusHI)
                                 ? resultEntry->DIR_FstClusLO
                                 : resultEntry->DIR_FstClusHI;

            if (currentCluster == 0)
            {
                printf("%s", resultEntry->DIR_name);
            }
            else
            {

                bytesCount = listDataEntry(getClusterOffset(currentCluster));
                currentCluster = traverseCluster(currentCluster, &bytesCount);
            }
        }
        else
        {
            printf("ERROR: File doesn't exits");
        }
    }
    else
    {
        printf("ERROR: too many arguements");
    }
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

/**
 * size command
*/

void sizeCommand(struct CommandList *commandList)
{

    unsigned int bytesCount;
    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);
    unsigned int currentCluster = utilityProps.currentCluster;

    struct DirEntry *resultEntry = searchSector(clusterOffset, commandList->commands[1], &bytesCount);

    while ((bytesCount == bpbInfo.BPB_BytsPerSec))
    {

        currentCluster = getNextCluster(currentCluster);
        resultEntry = searchSector(getClusterOffset(currentCluster),
                                   commandList->commands[1], &bytesCount);
    }

    if (resultEntry != NULL)
    {
        printf("%u", resultEntry->DIR_FileSize);
    }
    else
    {
        printf("ERROR: File doesn't exits");
    }
}

/**
 * cd command
*/

void cdCommand(struct CommandList *commandList)
{
    unsigned int bytesCount;
    //get the current cluster offset(location) for the data
    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);

    //using the currentCluster property to traverse to each cluster
    unsigned int currentCluster = utilityProps.currentCluster;

    struct DirEntry *resultEntry = searchSector(clusterOffset, commandList->commands[1], &bytesCount);

    //searching the different clusters for a specific entry
    while ((bytesCount == bpbInfo.BPB_BytsPerSec))
    {
        currentCluster = getNextCluster(currentCluster);
        resultEntry = searchSector(getClusterOffset(currentCluster),
                                   commandList->commands[1], &bytesCount);
    }

    if (resultEntry != NULL)
    {
        //the cluster associated with the directory
        currentCluster = (resultEntry->DIR_FstClusLO > resultEntry->DIR_FstClusHI)
                             ? resultEntry->DIR_FstClusLO
                             : resultEntry->DIR_FstClusHI;
        // printf("%u", currentCluster);
        if (currentCluster != 0)
        {
            utilityProps.currentCluster = currentCluster;
        }
    }
    else
    {
        printf("ERROR: File doesn't exits");
    }
}

void creatCommand(struct CommandList *commandList)
{
    utilityProps.currentCluster = 435;
    struct DirEntry *newDirEntry;
    unsigned int fatValue = getNextCluster(utilityProps.currentCluster);
    char test[8] = "FFFFFFFF";

    //printf("\n FAT Value : %u", fatValue);
    while (fatValue < ENDOFCLUSTER)
    {
        printf("\n FAT Value : %u", fatValue);
        fatValue = getNextCluster(fatValue);
    }
}
/**
 * others
*/

//get the data from the file
void getFileData(int file)
{
    unsigned char *info;

    lseek(file, 11, SEEK_SET);

    info = (unsigned char *)malloc(sizeof(unsigned char) * 512);

    read(file, info, 512);

    memcpy(&bpbInfo, info, sizeof(struct BPBInfo));
}

//trim a string of spaces
void trimString(unsigned char *str)
{
    int count = 0;

    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != ' ')
        {
            str[count++] = str[i];
        }
    }

    str[count] = '\0';
}
