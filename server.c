//-------------------------------------------------------------------------------
//
//              Name: Eng. William da Rosa Fröhlich
//
//              Project: Server Serial
//
//-------------------------------------------------------------------------------
//--------------------     Libraries       --------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "rs232.h"

//-------------------------------------------------------------------------------
//--------------------     Global Variable       --------------------------------

int    cport_nr=0;          // COM1

char    id[1]={'0'};        // To control the data

char    ok[1] = {'0'},      // To control the answer
        path[] = {"C:\\temp\\server\\received\\"},      // Directory to save
        total_path[] = {NULL};                          // Total Path

//-------------------------------------------------------------------------------
//--------------------     Main Function       ----------------------------------

int main()
{

//--------------------     Variables       --------------------------------------

    int bdrate=9600;       // Speed - 9600 baud
    char mode[]={'8','N','1',0};

//--------------------     Starts Serial Communication       --------------------

    if(RS232_OpenComport(cport_nr, bdrate, mode, 0))
    {
        printf("COM Port: Error\n");
        return(0);
    }
    else
    {
        printf("COM Port: OK\n");
        listen_com_port();
    }

    return(0);
}

//-------------------------------------------------------------------------------
//--------------------     Function Listen COM Port       -----------------------

void listen_com_port()
{
    while(1)
    {
//--------------------     1º Step       ----------------------------------------
//--------------------     5º Step       ----------------------------------------
        RS232_PollComport(cport_nr, id, 1);

        if (id[0] == '1')
        {
            printf("ID Received: %c\n", id[0]);
            ok[0] = '1';
            RS232_SendBuf(cport_nr, ok, 1);
            printf("OK Sent: %c\n", ok[0]);
            printf("Go to: receive_name\n");
//--------------------     2º Step       ----------------------------------------
            receive_name();
        }


        if (id[0] == '4')
        {
            printf("ID Received: %c\n", id[0]);
            ok[0] = '5';
            RS232_SendBuf(cport_nr, ok, 1);
            printf("OK Sent: %c\n", ok[0]);
            printf("Go to: receive_file\n");
//--------------------     6º Step       ----------------------------------------
            receive_file();
            printf("\nWaiting...\n");
        }
        #ifdef _WIN32
        Sleep(100);
        #else
        usleep(1000);  /* sleep for 1 miliSeconds */
        #endif
    }
}

//-------------------------------------------------------------------------------
//--------------------     Function Receive Name       --------------------------

int receive_name()
{

//--------------------     Variables       --------------------------------------

    FILE    *f;

    int     received = 0,
            size_int = 0;

    char    size_char[3] = {NULL},
            name_file[] = {NULL};

    while(1)
    {
//--------------------     2º Step       ----------------------------------------
        RS232_PollComport(cport_nr, id, 1);
        if (id[0] == '2')
        {
            printf("ID Received: %c\n", id[0]);
            ok[0] = '2';
            RS232_SendBuf(cport_nr, ok, 1);
            printf("OK Sent: %c\n", ok[0]);

            while(1)
            {
//--------------------     3º Step       ----------------------------------------
                received = RS232_PollComport(cport_nr, size_char, 3);
                if (received != 0)
                {
                    received = 0;
                    printf("Name Size (char) Received: %s\n", size_char);
                    ok[0] = '3';
                    RS232_SendBuf(cport_nr, ok, 1);
                    printf("OK Sent: %c\n", ok[0]);
                    size_int = atoi(size_char);
                    printf("Name Size (int): %i\n", size_int);

                    while(1)
                    {
//--------------------     4º Step       ----------------------------------------
                        received = RS232_PollComport(cport_nr, name_file, size_int);

                        if (received != 0)
                        {
                            received = 0;
                            printf("Name Received: %s\n", name_file);
                            ok[0] = '4';
                            RS232_SendBuf(cport_nr, ok, 1);
                            printf("OK Sent: %c\n", ok[0]);

                            strcpy(total_path, path);
                            strcat(total_path, name_file);

                            printf("Dir: %s\n", total_path);

                            f = fopen(total_path, "wb");
                            if (f != 0)
                            {
                                printf("File: Created\n");
                                fclose(f);
                                return(1);
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
                        usleep(100000);  /* sleep for 1 Second */
                        #endif
                    }
                }
                #ifdef _WIN32
                Sleep(1000);
                #else
                usleep(100000);  /* sleep for 100 miliSeconds */
                #endif
            }
        }
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(100000);  /* sleep for 100 miliSeconds */
        #endif
    }
}

//-------------------------------------------------------------------------------
//--------------------     Function Receive File       -----------------------

int receive_file()
{

//--------------------     Variables       --------------------------------------

    FILE    *f;

    int     received = 0,
            size_int = 0;

    char    size_char[4]={NULL},
            data_file[1024]={NULL};

    while(1)
    {
//--------------------     6º Step       ----------------------------------------
        RS232_PollComport(cport_nr, id, 1);
        if (id[0] == '5')
        {
            printf("ID Received: %c\n", id[0]);
            ok[0] = '6';
            RS232_SendBuf(cport_nr, ok, 1);
            printf("OK Sent: %c\n", ok[0]);
            while(1)
            {
//--------------------     7º Step (A)       -------------------------------------
                RS232_PollComport(cport_nr, id, 1);
                if (id[0] == '6')
                {
                    printf("ID Received: %c\n", id[0]);
                    printf("File Smaller than 1024\n");
                    ok[0] = '7';
                    RS232_SendBuf(cport_nr, ok, 1);
                    printf("OK Sent: %c\n", ok[0]);
                    while(received == 0)
                    {
//--------------------     8º Step       ----------------------------------------
                        received = RS232_PollComport(cport_nr, size_char, 4);
                        #ifdef _WIN32
                        Sleep(100);
                        #else
                        usleep(1000);  /* sleep for 1 miliSeconds */
                        #endif
                    }
                    received = 0;
                    size_int = atoi(size_char);
                    printf("File Size: %i (int) - %s (char)\n", size_int, size_char);
                    ok[0] = '8';
                    RS232_SendBuf(cport_nr, ok, 1);
                    while(1)
                    {
//--------------------     9º Step       ----------------------------------------
                        received = RS232_PollComport(cport_nr, data_file, size_int);
                        if(received != 0)
                        {
                            received = 0;
                            printf("Data Received: %s\n", data_file);
                            ok[0] = '9';
                            RS232_SendBuf(cport_nr, ok, 1);
                            printf("OK Sent: %c\n", ok[0]);

                            printf("Dir: %s\n", total_path);
                            f = fopen(total_path, "ab");

                            if (f != 0)
                            {
                                printf("File: OK\n");

                                fwrite(data_file, sizeof(char), size_int, f);
                                printf("File: Written\n");
                                strcpy(total_path, path);
                                fclose(f);
                                return(1);
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
                        usleep(100000);  /* sleep for 100 miliSeconds */
                        #endif
                    }
                }

                if (id[0] == '7')
                {
                    printf("ID Received: %c\n", id[0]);
                    printf("File Bigger than 1024\n");
                    ok[0] = '7';
                    RS232_SendBuf(cport_nr, ok, 1);
                    printf("OK Sent: %c\n", ok[0]);
//--------------------     7º Step (B)       -------------------------------------
                    received = big_file();
                    if(received != 0)
                    {
                        return(1);
                    }
                }
                #ifdef _WIN32
                Sleep(1000);
                #else
                usleep(100000);  /* sleep for 100 miliSeconds */
                #endif
            }

        }
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(100000);  /* sleep for 100 miliSeconds */
        #endif
    }
}

//-------------------------------------------------------------------------------
//--------------------     Function File Bigger than 1024 bytes       -----------

int big_file()
{

//--------------------     Variables       --------------------------------------

    FILE    *f;

    int     received = 0,
            num_parts_int = 0,
            rest_int = 0;

    char    data_file[1024]={NULL},
            num_parts_char[10]={NULL},
            rest_char[10]={NULL};


    printf("Inside the big_file Function\n");

    while(1)
    {
//--------------------     8º Step       ----------------------------------------
        received = RS232_PollComport(cport_nr, num_parts_char, 10);
        if(received != 0)
        {
            received = 0;
            num_parts_int = atoi(num_parts_char);
            printf("Number of Parts: %i (int) - %s (char)\n", num_parts_int, num_parts_char);
            ok[0] = '9';
            RS232_SendBuf(cport_nr, ok, 1);
            printf("OK Sent: %c\n", ok[0]);
            while(1)
            {
//--------------------     9º Step       ----------------------------------------
                received = RS232_PollComport(cport_nr, rest_char, 10);
                if(received != 0)
                {
                    received = 0;
                    rest_int = atoi(rest_char);
                    printf("Rest: %i (int) - %s (char)\n", rest_int, rest_char);
                    ok[0] = 'A';
                    RS232_SendBuf(cport_nr, ok, 1);
                    printf("OK Sent: %c\n", ok[0]);

                    printf("Dir: %s\n", total_path);
                    f = fopen(total_path, "ab");
                    int i = 0;
                    for(i = 0; i <= num_parts_int; i++)
                    {
                        received = 0;
                        if(i == num_parts_int)
                        {
                            printf("Last part\n");
                            while(received == 0)
                            {
//--------------------     10º Step (A)      ------------------------------------
                                received = RS232_PollComport(cport_nr, data_file, rest_int);
                            }
                            fwrite(data_file, sizeof(char), rest_int, f);
                            received = 0;
                            printf("File: Written\n");
                            strcpy(total_path, path);
                            fclose(f);
                            return(1);
                        }
                        else
                        {
                            while(received == 0)
                            {
//--------------------     10º Step (B)      ------------------------------------
                                received = RS232_PollComport(cport_nr, data_file, 1024);
                            }
                            fwrite(data_file, sizeof(char), 1024, f);
                            received = 0;
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
                #ifdef _WIN32
                Sleep(100);
                #else
                usleep(1000);  /* sleep for 1 miliSeconds */
                #endif
            }
        }
        #ifdef _WIN32
        Sleep(100);
        #else
        usleep(1000);  /* sleep for 1 miliSeconds */
        #endif
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
