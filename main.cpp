/* 
 * File:   main.cpp
 * Author: jeremiegallee
 *
 * Created on 1 mai 2013, 19:06
 */

#include <cstdlib>
#include <stdio.h>
#include <limits.h>
#include <string.h>

//#define FIND_MODE

/* Verbose level, 0: no output, 1 output */
#define NO_VERBOSE 0
#define VERBOSE 1
#define DEBUG 2

using namespace std;

char* checkBackslash(char * stringToCheck, short VERBOSELEVEL);

/**
 * Command line tool that recursively search in the subdirectories of the 
 * current directory where you execute the command. It search the images and
 * open them in ONE INSTANCE of the Preview app of OS X. It avoid having a
 * window per picture.
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char** argv) {

    if (argc < 2) {
        perror("ERROR, you must give at least one argument\n");
        return 0;
    }

    short VERBOSELEVEL = NO_VERBOSE;
    if ((argc >= 3) && (strcmp(argv[2], "v") == 0)) {
        VERBOSELEVEL = VERBOSE;
        printf("Enabling VERBOSE\n");
    } else if ((argc >= 3) && (strcmp(argv[2], "V") == 0)) {
        VERBOSELEVEL = DEBUG;
        printf("Enabling DEBUG\n");
    }


    FILE * fp;
    FILE * fp_getPath;
    FILE * fp_executeFinalCommand;
    FILE * fp_filterSearchFiles;

    char commandGetPath[] = "pwd";
    char result[PATH_MAX];
    char* myPath;
    bool oneWasFound = false;

    /* Build filterSearchFiles command */
    char filterSearch[] = " && ls | grep ";
    strcat(filterSearch, argv[1]);


    fp_getPath = popen(commandGetPath, "r");

    while (fgets(result, PATH_MAX, fp_getPath) != NULL) {
        myPath = result;
        if (VERBOSELEVEL > 0)
            printf("Path: %s", myPath);
    }

    myPath = checkBackslash(myPath, VERBOSELEVEL);


    /* Build command string to explore the whole subdirectories */
#if FIND_MODE // Currently not working
    /* Yeah, horrible magic number, I know. Just for this string to work with 
     strcat() which ask for const char*... Pretty isn't it? */
    char str2[] = " -type f -name \"*.JPG\"";
    char exploreSubDirsCommand[10000] = "find ";
#else
    char str2[] = " && ls -R | grep ./";
    char exploreSubDirsCommand[10000] = "cd ";
#endif


    myPath[strlen(myPath) - 1] = 0;
    strcat(exploreSubDirsCommand, myPath);
    strcat(exploreSubDirsCommand, str2);

    if (VERBOSELEVEL > 0)
        printf("Exploring dirs command: %s\n", exploreSubDirsCommand);

    /* Instanciate string that will be executed with 'open' command (OS X)*/
    /* Yeah, the same that exploreSubDirsCommand[]... */
    char commandOpen[10000] = "open ";

    /* Execute command to explore subdirectories */
    fp = popen(exploreSubDirsCommand, "r");
    if (fp == NULL) {
        perror("Can't open pipe...\n");
        return 0;
    }

    while (fgets(result, PATH_MAX, fp) != NULL) {

        /* ------ COMPUTE command to filter only directories that contain at least
         one JPG file */
        
        /* remove 'CR' character */
        result[strlen(result) - 2] = 0;
        myPath = result;
        char command_FilterSearch[1000] = "cd ";
        myPath = checkBackslash(myPath, VERBOSELEVEL);
        strcat(command_FilterSearch, myPath);
        if(VERBOSELEVEL >= 2){
            printf("Corrected path: START%sEND\n", command_FilterSearch);
        }
        
        command_FilterSearch[strlen(command_FilterSearch) - 1] = 0;
        strcat(command_FilterSearch, filterSearch);
        if (VERBOSELEVEL > 0)
            printf("Searching command: %s\n", command_FilterSearch);
        /* ------ End compute commant to filter ---------------------------*/

        /* ------ EXECUTE command to filter only directories that contain at least
         one JPG file */
        fp_filterSearchFiles = popen(command_FilterSearch, "r");
        if (fgets(command_FilterSearch, PATH_MAX, fp_filterSearchFiles)) {
            if (VERBOSELEVEL >= 1)
                printf("Found image in directory %s\n", result);
            /* Remove ':' character (given by the 'cd' command */
            result[strlen(result) - 1] = 0;
            
            strcat(commandOpen, " ");
            strcat(commandOpen, result);
            strcat(commandOpen, "/*.");
            strcat(commandOpen, argv[1]);
            oneWasFound = true;
        }
    }

    if (VERBOSELEVEL >= 1)
        printf("Final command: %s\n", commandOpen);
    if (oneWasFound) {
        fp_executeFinalCommand = popen(commandOpen, "r");
        if (fp == NULL) {
            perror("Can't open pipe...\n");
            return 0;
        }
    } else {
        printf("No %s files found\n", argv[1]);
    }

    /* ------ CLOSE all pipes -------- */
    pclose(fp);
    pclose(fp_filterSearchFiles);
    pclose(fp_executeFinalCommand);
    pclose(fp_getPath);

    return 0;
}

/**
 * Check every space character to add a backslash befor. Otherwise, doing 
 * "cd /pwet lol/something won't make sense.
 * @param char* stringToCheck         the string to purify
 * @param short VERBOSELEVEL          Verbose level
 * @return the string purified.
 */
char* checkBackslash(char * stringToCheck, short VERBOSELEVEL){
    
        /* Check for space characters in pwd */
    char checkspace = *(stringToCheck);
    int sizePathString = strlen((const char*) stringToCheck);
    if (VERBOSELEVEL >= 2) {
        printf("PATH TO EXPLORE: %s\n", stringToCheck);
        printf("Size of string: %d\n", sizePathString);
    }
    int index = 0;
    while (checkspace != '\n') {

        if (VERBOSELEVEL >= 2) {
            printf("Check character: %c at %d\n", checkspace, index);
        }
        /* Check if space */
        if (checkspace == ' ') {
            /* Change with backslash character */
            *(stringToCheck + index) = '\\';
            if (VERBOSELEVEL >= 2) {
                printf("%s\n", stringToCheck);
            }
            if (VERBOSELEVEL > 0)
                if (VERBOSELEVEL >= 2) {
                    printf("FOUND SPACE! %c\n", checkspace);
                }
            /* Move all charatcers to one step further */
            for (int i = sizePathString - 1; i >= index; i--) {
                if (VERBOSELEVEL >= 2)
                    printf("Doing: %c, char: %d\n", stringToCheck[i], i);
                stringToCheck[i + 1] = stringToCheck[i];
            }
            /* REput the space character (removed with action *(myPath + index) = '\\' ) */
            stringToCheck[index + 1] = ' ';
            index++;
        }
        index++;
        checkspace = *(stringToCheck + index);
        //            sizePathString++;
    }

    
    return stringToCheck;
}
