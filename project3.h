#include <stdio.h>
<<<<<<< Updated upstream
=======
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
void lsCommand2(struct CommandList *temp, struct CommandList *CommandList);

//info command
void infoCommand();

//size command
void sizeCommand(struct CommandList *commandList);

//cd command
void cdCommand(struct CommandList *commandList);
>>>>>>> Stashed changes


void _testexit(void);

<<<<<<< Updated upstream
    
=======
//rm commnand
void rmCommand(struct CommandList *commandList);


#endif
>>>>>>> Stashed changes
