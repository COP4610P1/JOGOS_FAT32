#ifndef PROJ3
#define PROJ3

#include <stdio.h>
#include "fat_functions.h"

typedef enum
{
    false,
    true
} boolean;

//use to store the commands and it parms (tokens)
struct CommandList
{
    int length;
    char **commands;
};

struct UtilityProps
{
    int file;
    unsigned int rootDirSectors;
    unsigned int dataSec;
    unsigned int countOfClusters;
    unsigned int firstDataSector;

    /**
     * this will be used for cd command to
     * store the current cluster/location
    */
    unsigned int currentCluster;
};

//variables
struct UtilityProps utilityProps;
struct BPBInfo bpbInfo;
struct DirEntry dirEntry;

int file;

//functions

void setBPBInfo(int file);

void getFileData(int file);

void displayBPBInfo();

void trimString(unsigned char *str);

void clusterCount();

/**
 * commands handler
*/

//ls commands
void lsCommand(struct CommandList *CommandList);

//info command
void infoCommand();

#endif