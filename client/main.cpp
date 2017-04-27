#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "peerclient.h"

using namespace std;

char username[21];

void get_username()
{
    puts("Please Enter a username (max 20 chars) : ");
    fgets(username, 20, stdin);
    printf("[DEBUGGING][main][get_username][username]: %s\n", username);
}

void signal_handler(int sig)
{
    switch(sig)
    {
    case SIGINT:
        fprintf(stderr, "Exiting Program Immediately...\n");
        abort();
    default:
        fprintf(stderr, "Unknown Signal\n");
        abort();
    }
}

int main(int argc, char **argv)
{
    signal(SIGINT, signal_handler);

    char server_name[50];
    int16_t server_tcp_port_number = 4444;
    int16_t server_udp_port_number = 5555;

    if (argc == 2)
    {
        if (strcmp(argv[1], "-h") == 0)
        {
            printf("usage: \n* Enter \'REGISTER\' for registration.\n* Enter \'REQUESTPEER\' for P2P establishment.\n");
            exit(0);
        } else
        {
            // need to be verified
            strcpy(server_name, argv[1]);
        }
    }

    PeerClient *peerclient = new PeerClient(server_name, server_tcp_port_number, server_udp_port_number);
    int ret_init = peerclient->init();
    if (ret_init)
    {
        fprintf(stderr, " * Failed to init buffers and socket data\n * Exit code is: %d\n", ret_init);
    }
    else
    {
        puts("Peer buffers initialized successfully");
    }

    char cmd[20];
    while (1)
    {
        puts("Enter a command:");
        puts("Register (r) | Request Peer (rp) | Connect (c) | Quit (q)");
        puts("-----------------------------------------");
        printf("cmd: ");
        scanf("%s", cmd);
        if (strcmp(cmd, "r") == 0)
        {
            get_username();
            int ret_reg_msg = peerclient->send_register_message(username);
            printf("[DEBUGGING][main][username]: %s\n", username);
            if (ret_reg_msg)
            {
                printf(" * Failed to send message \n * Exit code is: %d\n", ret_reg_msg);
            }
            else
            {
                puts("Register messgae sent successfully");
            }

            int ret_frst_udp = peerclient->send_udp_first_msg();
            if (ret_frst_udp)
            {
                printf(" * Failed to send first UDP messgae\n * Exit code is: %d\n", ret_frst_udp);
            }
            else
            {
                puts("First UDP message sent successfully");
            }
        }
        else if (strcmp(cmd, "rp") == 0)
        {
            peerclient->send_peer_connection_request();
        }
        else if (strcmp(cmd, "c") == 0)
        {
            peerclient->start_peer_communication();
            pthread_join(peerclient->get_recv_thread_id(), NULL);
        }
        else if (strcmp(cmd, "q") == 0)
        {
            puts("Goodbye!!!");
            break;
        }
        else
        {
            puts("Unknown Command!!");
        }
    }
    return 0;
}
