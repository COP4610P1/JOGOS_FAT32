#include "project3.h"


<<<<<<< Updated upstream
void _testexit(void){
    int i = 0;
    printf("Hello world!");
}
=======
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

void lsCommand2(struct CommandList *temp, struct CommandList *commandList)
{
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
      bytesCount = listDataEntry2(clusterOffset,commandList);
      printf ("bytesCount: %d \n", bytesCount);
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
    if (commandList->commands[1] == NULL)
    {
        printf("ERROR : enter a file name");
        return;
    }

    // utilityProps.currentCluster = 435;
    // struct DirEntry *newDirEntry;
    unsigned int nextCluster = utilityProps.currentCluster;
    // //updating fat
    // unsigned int emptyFATOffset = traverseFAT();
    // lseek(utilityProps.file, emptyFATOffset, SEEK_SET);
    // unsigned int test = NEWCLUSTER;
    // write(utilityProps.file, &test, 4);

    //creating a new file
    struct DirEntry *dirEntry = (struct DirEntry *)malloc(sizeof(struct DirEntry));

    strcpy(dirEntry->DIR_name, commandList->commands[1]);
    dirEntry->DIR_FileSize = 0;
    dirEntry->DIR_FstClusHI = 0;
    dirEntry->DIR_FstClusLO = 0;
    dirEntry->DIR_Attr = 0;
    dirEntry->DIR_NTRes = 0;
    dirEntry->DIR_CrtTimeTenth = 0;
    dirEntry->DIR_CrtTime = 0;
    dirEntry->DIR_CrtDate = 0;
    dirEntry->DIR_LstAccDate = 0;
    dirEntry->DIR_WrtTime = 0;
    dirEntry->DIR_WrtDate = 0;

    unsigned int storedClusterValue = getNextCluster(nextCluster);

    printf("\n 1 FAT Value : %u", storedClusterValue);

    if (storedClusterValue >= ENDOFCLUSTER)
    {
        storedClusterValue = nextCluster;
    }
    else
    {
        while (storedClusterValue < 0)
        {
            storedClusterValue = getNextCluster(storedClusterValue);
        }
    }

    unsigned int storedClusterOffset = getClusterOffset(storedClusterValue);

    printf("\n FAT Value : %u", storedClusterValue);

    printf("\n Cluster Offset : %u", storedClusterOffset);
    printf("\n Cluster Value : %u", utilityProps.currentCluster);
    unsigned int *bytesCount;
    unsigned int saveSpotOffset = searchSectorOffset(storedClusterOffset, "", bytesCount);

    printf("\n Entry Offset : %u", saveSpotOffset);

    lseek(utilityProps.file, saveSpotOffset, SEEK_SET);

    write(utilityProps.file, dirEntry, sizeof(struct DirEntry));
}

void mkdirCommand(struct CommandList *commandList)
{
    if (commandList->commands[1] == NULL)
    {
        printf("ERROR : enter a file name");
        return;
    }

    //using the currentCluster property to traverse to each cluster
    unsigned int currentCluster = utilityProps.currentCluster;

    unsigned int newClusterValue = setNewFATValue(NEWCLUSTER);

    struct DirEntry *newDirEntry = createDirEntry(commandList->commands[1], newClusterValue, 16);

    unsigned int bytesCount;

    //find the next free entry in the cluster
    unsigned int freeEntryOffset = searchSectorOffset(getClusterOffset(currentCluster),
                                                      "", &bytesCount);
    printf("\nbytescount %u\n", bytesCount);

    //searching the different clusters for a free slot
    while ((bytesCount == bpbInfo.BPB_BytsPerSec))
    {
        currentCluster = getNextCluster(currentCluster);
        if (currentCluster != -2)
        {

            printf("\ncurrent cluster offset \n %u", getClusterOffset(currentCluster));
            freeEntryOffset = searchSectorOffset(getClusterOffset(currentCluster),
                                                 "", &bytesCount);
        }else{
            //set the new FAT value
            //modify the last FATValue that associated with the current cluster.
            //stored the new fat value in the hold fat offset
            //create the new cluster and set it to the currentcluster
            // set the default values
        }
    }

    printf("free entry space %u", freeEntryOffset);

    //finding the location and writing to the file
    lseek(utilityProps.file, freeEntryOffset, SEEK_SET);

    write(utilityProps.file, newDirEntry, sizeof(struct DirEntry));
}

void rmCommand(struct CommandList *commandList)
{

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
>>>>>>> Stashed changes
