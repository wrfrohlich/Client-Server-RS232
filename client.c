//-------------------------------------------------------------------------------
//
//              Name: Eng. William da Rosa Fröhlich
//
//              Project: Client Serial
//
//-------------------------------------------------------------------------------
//--------------------     Libraries       --------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <windows.h>
#include "rs232.h"

//-------------------------------------------------------------------------------
//--------------------     Global Variable       --------------------------------

int     cport_nr = 1,         // COM2
        size_name = 0;

char    id[1]={'0'};       // To control the data

char    ok[1] = {'0'},  // To control the answer
        path[] = {"C:\\temp\\client\\files\\"},         // Monitored Directory
        name_file[] = {NULL},                           // File
        total_path[] = {"C:\\temp\\client\\files\\"},   // Total Path
        str_null[] = {NULL};

//-------------------------------------------------------------------------------
//--------------------     Main Function       ---------------------------------

int main()
{

//--------------------     Variables       --------------------------------------

    int     bdrate=9600;       // Speed - 9600 baud
    char    mode[]={'8','N','1',0},
            answer = {'A'};
    struct  dirent *dir_ent;

    DIR     *directory = opendir("C:\\temp\\client\\files\\");

//--------------------     Starts Serial Communication       --------------------

    if(RS232_OpenComport(cport_nr, bdrate, mode, 0))
    {
        printf("COM Port: Error\n");
        return(0);
    }
    else
    {
        printf("COM Port: OK\n");
        if(directory == NULL)
        {
            printf("DIR: Error\n");
            return(0);
        }
        else
        {
            printf("DIR: OK\n");
            while ((dir_ent = readdir(directory)) != NULL)
            {
                if (strcmp(dir_ent->d_name, ".") != 0 && strcmp(dir_ent->d_name, "..") != 0)
                {
                    get_name(dir_ent->d_name);
                }
                strcpy(name_file, str_null);
            }
            closedir(directory);

            while (answer != 'Y' && answer != 'N' && answer != 'y' && answer != 'n')
            {
                printf("\nCheck again?\nPress Y / N\n");
                scanf("%c", &answer);
                if (answer == 'Y' || answer == 'y')
                {
                    answer = 'A';
                    RS232_CloseComport(cport_nr);
                    main();
                }
            }

        }
    }

    return(0);
}

//-------------------------------------------------------------------------------
//--------------------     Function Get Name File       -------------------------

void get_name(char* str)
{
    FILE    *f;

    f = fopen("File.txt", "w");
    fwrite(str, sizeof(char), strlen(str), f);
    fclose(f);
    f = fopen("File.txt", "r");
    fseek(f, sizeof(char), SEEK_END);
    size_name = ftell(f)-1;
    rewind(f);
    fread(name_file, sizeof(char), size_name, f);
    printf("File: %s - Size: %i\n", name_file, size_name);
    fclose(f);

    read_dir();

    printf("\nWaiting...\n");
}

//-------------------------------------------------------------------------------
//--------------------     Function Read Directory       ------------------------

int read_dir()
{

//--------------------     Variables       --------------------------------------

    int result = 0;

    id[0] = '1';

//--------------------     1º Step       ----------------------------------------
    RS232_SendBuf(cport_nr, id, 1);

    while(1)
    {
        RS232_PollComport(cport_nr, ok, 1);
        if (ok[0] == '1')
        {
            printf("OK Returned - %c\n", ok[0]);
//--------------------     2º Step (A)      --------------------------------------
            result = send_name();

            if (result != 0)
            {
                result = 0;
                id[0] = '4';
                printf("ID - Function Name - %c\n", id[0]);
//--------------------     5º Step       ----------------------------------------
                RS232_SendBuf(cport_nr, id, 1);

                while(1)
                {
                    RS232_PollComport(cport_nr, ok, 1);

                    if (ok[0] == '5')
                    {
                        printf("OK Returned - %c\n", ok[0]);
                        result = send_file();

                        printf("Transference: OK!\n");

                        return(1);
                    }
                    #ifdef _WIN32
                    Sleep(1000);
                    #else
                    usleep(1000);  /* sleep for 1 miliSeconds */
                    #endif
                }
            }
        }
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(1000);  /* sleep for 1 miliSeconds */
        #endif
    }
}

//-------------------------------------------------------------------------------
//--------------------     Function Send Name       -----------------------------

int send_name()
{

//--------------------     Variables       --------------------------------------

    char size_c[3];

    printf("Name: %s\n", name_file);
    sprintf(size_c, "%i", size_name);
    printf("Name Size: %s\n", size_c);

    printf("ID Name Size Sent\n");
    id[0] = '2';
//--------------------     2º Step (B)      --------------------------------------
    RS232_SendBuf(cport_nr, id, 1);

    while(1)
    {
        RS232_PollComport(cport_nr, ok, 1);

        if (ok[0] == '2')
        {
            printf("OK Returned - %c\n", ok[0]);
            id[0]='4';
//--------------------     3º Step       ----------------------------------------
            RS232_SendBuf(cport_nr, size_c, 3);

            while(1)
            {
                RS232_PollComport(cport_nr, ok, 1);

                if (ok[0] == '3')
                {
                    printf("OK Returned - %c\n", ok[0]);
                    printf("Name sent\n");
//--------------------     4º Step       ----------------------------------------
                    RS232_SendBuf(cport_nr, name_file, size_name);
                    while(1)
                    {
                        RS232_PollComport(cport_nr, ok, 1);

                        if (ok[0] == '4')
                        {
                            printf("OK Returned - %c\n", ok[0]);
                            return(1);
                        }
                        #ifdef _WIN32
                        Sleep(1000);
                        #else
                        usleep(1000);  /* sleep for 1 miliSeconds */
                        #endif
                    }
                }
                #ifdef _WIN32
                Sleep(1000);
                #else
                usleep(1000);  /* sleep for 1 miliSeconds */
                #endif
            }
        }
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(1000);  /* sleep for 1 miliSeconds */
        #endif
    }
}

//-------------------------------------------------------------------------------
//--------------------     Function Send File       -----------------------------


int send_file()
{

//--------------------     Variables       --------------------------------------

    FILE    *f;

    int     size = 0,
            size_int = 0;

    char    size_char[4] = {NULL},
            data_file[1024] = {NULL};

    id[0] = '5';
//--------------------     6º Step       ----------------------------------------
    RS232_SendBuf(cport_nr, id, 1);
    while(1)
    {
        RS232_PollComport(cport_nr, ok, 1);
        if(ok[0] == '6')
        {
            printf("OK Returned - %c\n", ok[0]);

            strcpy(total_path, path);
            strcat(total_path, name_file);

            printf("Dir: %s\n", total_path);

            f = fopen(total_path, "rb");
            printf("File: Read\n");
            if (f != 0)
            {
                printf("File: OK\n");

                fseek(f, sizeof(char), SEEK_END);
                size = ftell(f)-1;
                printf("File Size: %i\n", size);
                if(size <= 1024)
                {
                    printf("File Smaller than 1024\n");
                    id[0] = '6';
//--------------------     7º Step       ----------------------------------------
                    RS232_SendBuf(cport_nr, id, 1);
                    while(1)
                    {
                        RS232_PollComport(cport_nr, ok, 1);
                        if(ok[0] == '7')
                        {
                            printf("OK Returned - %c\n", ok[0]);
                            size_int = size / 1;
                            sprintf(size_char, "%i", size_int);
                            printf("File Size: %i (int) - %s (char)\n", size_int, size_char);

//--------------------     8º Step       ----------------------------------------
                            RS232_SendBuf(cport_nr, size_char, 4);
                            while(ok[0] != '8')
                            {
                                RS232_PollComport(cport_nr, ok, 1);
                                #ifdef _WIN32
                                Sleep(1000);
                                #else
                                usleep(1000);  /* sleep for 1 miliSeconds */
                                #endif
                            }
                            printf("OK Returned - %c\n", ok[0]);
                            rewind(f);

                            fread(data_file, sizeof(char), size_int, f);

//--------------------     9º Step       ----------------------------------------
                            RS232_SendBuf(cport_nr, data_file, 1024);
                            printf("Data Sent\n");

                            while(1)
                            {
                                RS232_PollComport(cport_nr, ok, 1);
                                if (ok[0] == '9')
                                {
                                    printf("OK Returned - %c\n", ok[0]);
                                    return(1);
                                }
                                #ifdef _WIN32
                                Sleep(1000);
                                #else
                                usleep(1000);  /* sleep for 1 miliSeconds */
                                #endif
                            }
                        }
                        #ifdef _WIN32
                        Sleep(1000);
                        #else
                        usleep(1000);  /* sleep for 1 miliSeconds */
                        #endif
                    }
                }
                else
                {
                    printf("File Bigger than 1024\n");
                    id[0] = '7';
//--------------------     7º Step       ----------------------------------------
                    RS232_SendBuf(cport_nr, id, 1);
                    while(1)
                    {
                        RS232_PollComport(cport_nr, ok, 1);
                        if(ok[0] == '7')
                        {
                            printf("OK Returned - %c\n", ok[0]);

                            big_file(size);
                            return(1);
                        }
                        #ifdef _WIN32
                        Sleep(1000);
                        #else
                        usleep(1000);  /* sleep for 1 miliSeconds */
                        #endif
                    }
                }
            }
            else
            {
                printf("File: Error\n");
                return(0);
            }
        }
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(1000);  /* sleep for 1 miliSeconds */
        #endif
    }
}

//-------------------------------------------------------------------------------
//--------------------     Function File Bigger than 1024 bytes       -----------

int big_file(int size)
{

//--------------------     Variables       --------------------------------------

    FILE    *f;

    int     num_parts_int = 0,
            rest_int = 0;

    char    data_file[1024] = {NULL},
            num_parts_char[10] = {NULL},
            rest_char[10] = {NULL};

    printf("Inside the big_file Function\n");


    strcpy(total_path, path);
    strcat(total_path, name_file);

    printf("Dir: %s\n", total_path);

    f = fopen(total_path, "rb");

    num_parts_int = size / 1024;
    sprintf(num_parts_char, "%i", num_parts_int);
    printf("Number of Parts: %i (int) - %s (char)\n", num_parts_int, num_parts_char);
//--------------------     8º Step       ----------------------------------------
    RS232_SendBuf(cport_nr, num_parts_char, 10);
    while(ok[0] =! '9')
    {
        RS232_PollComport(cport_nr, ok, 1);
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(1000);  /* sleep for 1 miliSeconds */
        #endif
    }

    rest_int = size % 1024;
    sprintf(rest_char, "%i", rest_int);
    printf("Rest: %i (int) - %s (char)\n", rest_int, rest_char);

//--------------------     9º Step       ----------------------------------------
    RS232_SendBuf(cport_nr, rest_char, 10);
    while(ok[0] =! 'A');
    {
        RS232_PollComport(cport_nr, ok, 1);
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(1000);  /* sleep for 1 miliSeconds */
        #endif
    }

    int i = 0;
    for(i = 0; i <= num_parts_int; i++)
    {
        if (i == num_parts_int)
        {
            fseek(f,(i*1024),SEEK_SET);
            fread(data_file, sizeof(char), rest_int, f);
//--------------------     10º Step (A)      ------------------------------------
            RS232_SendBuf(cport_nr, data_file, rest_int);
            printf("Last part\n");
        }
        else
        {
            fseek(f,(i*1024),SEEK_SET);
            fread(data_file, sizeof(char), 1024, f);
//--------------------     10º Step (B)      ------------------------------------
            RS232_SendBuf(cport_nr, data_file, 1024);
            printf("This is the part: %i\n", (i + 1));
        }

        if (i == (500*i))
        {
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(100000);  /* sleep for 10 miliSeconds */
            #endif
        }
    }
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
