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

    utilityProps.prevParentCluster = utilityProps.currentCluster = bpbInfo.BPB_RootClus;
}

/**
 * ls functions
*/
void lsCommand(struct CommandList *commandList)
{
    fflush(stdout);
    // utilityProps.currentCluster = 435; // using as a trigger to test each directory
    unsigned int bytesCount = -1;
    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);
    unsigned int currentCluster = utilityProps.currentCluster;

    struct DirEntry *resultEntry;

    if (commandList->length < 2)
    {
        bytesCount = listDataEntry(clusterOffset);
        currentCluster = displayCluster(currentCluster, &bytesCount);
    }
    else if (commandList->length == 2)
    {
        resultEntry = searchSectorEntry(clusterOffset, commandList->commands[1], &bytesCount);

        while ((bytesCount == bpbInfo.BPB_BytsPerSec || bytesCount == -1))
        {

            currentCluster = getNextCluster(currentCluster);
            resultEntry = searchSectorEntry(getClusterOffset(currentCluster),
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
                currentCluster = displayCluster(currentCluster, &bytesCount);
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

/***/

void lsCommand2(struct CommandList *temp, struct CommandList *commandList)
{
    fflush(stdout);
    // utilityProps.currentCluster = 435; // using as a trigger to test each directory
    unsigned char arr[256];
    int i = 0;
    unsigned int bytesCount = -1;
    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);
    unsigned int currentCluster = utilityProps.currentCluster;

    struct DirEntry *resultEntry;

    if (temp->length < 2)
    {
        //printf("CommandList: %s %s %s \n", commandList->commands[0], commandList->commands[1]);
        bytesCount = listDataEntry2(clusterOffset, commandList->commands[1]);
        printf("bytesCount: %d \n", bytesCount);
        currentCluster = displayCluster(currentCluster, &bytesCount);
    }
    else if (temp->length == 2)
    {
        resultEntry = searchSectorEntry(clusterOffset, temp->commands[1], &bytesCount);

        while ((bytesCount == bpbInfo.BPB_BytsPerSec || bytesCount == -1))
        {

            currentCluster = getNextCluster(currentCluster);
            resultEntry = searchSectorEntry(getClusterOffset(currentCluster),
                                            temp->commands[1], &bytesCount);
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
                currentCluster = displayCluster(currentCluster, &bytesCount);
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
    fflush(stdout);
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
    fflush(stdout);
    unsigned int bytesCount;
    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);
    unsigned int currentCluster = utilityProps.currentCluster;

    struct DirEntry *resultEntry = searchSectorEntry(clusterOffset, commandList->commands[1], &bytesCount);

    while ((bytesCount == bpbInfo.BPB_BytsPerSec))
    {

        currentCluster = getNextCluster(currentCluster);
        resultEntry = searchSectorEntry(getClusterOffset(currentCluster),
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
    printf("\n prev cluster 1 : %u\n", utilityProps.prevParentCluster);
    utilityProps.prevParentCluster = utilityProps.currentCluster;
    fflush(stdout);
    unsigned int bytesCount;
    //get the current cluster offset(location) for the data
    unsigned int clusterOffset = getClusterOffset(utilityProps.currentCluster);

    //using the currentCluster property to traverse to each cluster
    unsigned int currentCluster = utilityProps.currentCluster;

    struct DirEntry *resultEntry = searchSectorEntry(clusterOffset, commandList->commands[1], &bytesCount);

    //searching the different clusters for a specific entry
    while ((bytesCount == bpbInfo.BPB_BytsPerSec))
    {
        currentCluster = getNextCluster(currentCluster);
        resultEntry = searchSectorEntry(getClusterOffset(currentCluster),
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
            utilityProps.prevParentCluster = utilityProps.currentCluster;
            utilityProps.currentCluster = currentCluster;
        }
    }
    else
    {
        printf("ERROR: File doesn't exits");
    }

    printf("\n prev cluster 2 %u\n", utilityProps.prevParentCluster);
    printf("\n cluster  %u\n", utilityProps.currentCluster);
}

void creatCommand(struct CommandList *commandList)
{
    fflush(stdout);
    if (commandList->commands[1] == NULL)
    {
        printf("ERROR : enter a file name");
        return;
    }

    unsigned int *bytesCount;

    struct DirEntry *result = searchSectorEntry(getClusterOffset(utilityProps.currentCluster), commandList->commands[1], bytesCount);

    if (strlen(result->DIR_name) != 0 && (result->DIR_Attr == 32))
    {
        printf("ERROR : file already exists");
        return;
    }

    // utilityProps.currentCluster = 435;
    // struct DirEntry *newDirEntry;
    unsigned int nextCluster = utilityProps.currentCluster;

    //creating a new file
    struct DirEntry *newDirEntry = createNewDirEntryStruct(commandList->commands[1], 0, 32);
    //adding the new entry
    addDirEntry(newDirEntry, utilityProps.currentCluster);
}

void mkdirCommand(struct CommandList *commandList)
{
    fflush(stdout);
    if (commandList->commands[1] == NULL)
    {
        printf("ERROR : enter a file name");
        return;
    }

    unsigned int bytesCount;

    struct DirEntry *result = searchSectorEntry(getClusterOffset(utilityProps.currentCluster), commandList->commands[1], &bytesCount);
    //printf("\n dir atri 0x%X\n \n", result->DIR_Attr);
    //printf("Input x:");
    // scanf("%hhu", &result->DIR_Attr);
    // printf("\n dir name %s\n", result->DIR_name);

    if (strlen(result->DIR_name) != 0 && (result->DIR_Attr == 16))
    {
        printf("ERROR : file already exists");
        return;
    }

    unsigned int newClusterValue = setNewFATValue(NEWCLUSTER);

    struct DirEntry *newDirEntry = createNewDirEntryStruct(commandList->commands[1], newClusterValue, 16);

    //adding the new entry
    addDirEntry(newDirEntry, utilityProps.currentCluster);

    //adding the default directory entries
    //addDefaultDirectoryEntry(newClusterValue);
}

void mvCommand(struct CommandList *commandList)
{
    fflush(stdout);
    printf("move\n");

    unsigned int *bytesCount;

    // TODO modify this to search all clusters
    struct DirEntry *toEntry = searchSectorEntry(getClusterOffset(utilityProps.currentCluster), commandList->commands[2], bytesCount);

    if (strlen(toEntry->DIR_name) == 0)
    {
        printf("\nERROR : directory doesn't exist");
        return;
    }

    if (toEntry->DIR_Attr != 16)
    {
        printf("\nERROR : not a directory");
        return;
    }

    // TODO modify this to search all clusters
    struct DirEntry *fromEntry = searchSectorEntry(getClusterOffset(utilityProps.currentCluster), commandList->commands[1], bytesCount);

    //getting the cluster
    unsigned int cluster = toEntry->DIR_FstClusHI;

    cluster << 4;
    cluster += toEntry->DIR_FstClusLO;

    addDirEntry(fromEntry, cluster);
}

void cpCommand(struct CommandList *commandList)
{
    fflush(stdout);
    ///printf("move\n");

    unsigned int *bytesCount;
    unsigned int storeCluster;

    struct DirEntry *toEntry = searchSectorEntry(getClusterOffset(utilityProps.currentCluster), commandList->commands[2], bytesCount);

    struct DirEntry *originalFile = searchSectorEntry(getClusterOffset(utilityProps.currentCluster), commandList->commands[1], bytesCount);

    if (strlen(originalFile->DIR_name) == 0 || originalFile->DIR_name == NULL)
    {
        printf("\nERROR : file doesn't exist\n");
        return;
    }

    if (originalFile->DIR_Attr != ATTR_ARCHIVE)
    {
        printf("\nERROR : not a file\n");
        return;
    }

    if (strlen(toEntry->DIR_name) == 0 || toEntry->DIR_Attr != ATTR_DIRECTORY)
    {
        // printf("\nfolder doesn't exist %s\n", toEntry->DIR_name);
        //fflush(stdout);
        storeCluster = utilityProps.currentCluster;

        memcpy(originalFile->DIR_name, commandList->commands[2], sizeof(commandList->commands[2]));
    }
    else
    {
        //getting the cluster
        storeCluster = toEntry->DIR_FstClusHI;

        storeCluster << 4;
        printf("\nstoreCluster %u\n", storeCluster);
        storeCluster += toEntry->DIR_FstClusLO;
    }

    //addDirEntry(originalFile, storeCluster);
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

int stringCompare(char *str1, char *str2, int size)
{

    //for()
}