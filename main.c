#include <stdio.h>
#include <string.h>
#include "project3.h"

int main()
{
    char command[100];

    while (command != "exit")
    {
        printf("$ ");
        scanf("%s",command);

        if (strcmp(command,"info") == 0)
        {
        }
        else if (strcmp(command,"ls") == 0)
        {
        }
        else if (strcmp(command,"exit") == 0)
        {
            
            _testexit();
        }
    }

    return 0;
}