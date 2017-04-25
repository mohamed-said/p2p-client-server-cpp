#ifndef COMMUNICATIONSERVER_H
#define COMMUNICATIONSERVER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string>


#include "peerholder.h"
#include "peerdata.h"

#define MAX_MSG_SIZE    1400 
//#define SOCKET_ERROR  1000
//#define BIND_ERROR    2000
//#define LISTEN_ERROR  3000
//#define ACCEPT_ERROR  4000
//#define PTHREAD_ERROR 5000

using namespace std;

class CommunicationServer {

private:

    int16_t tcp_server_socket_fd;                   // TCP server socket file descriptor
    int16_t udp_server_socket_fd;                   // UDP server socket file descriptor

    int16_t tcp_client_socket_fd;                   // TCP client socket file descriptor to read data upon

    socklen_t socket_address_size;                  // size of sockaddr_in struct

    int16_t tcp_port_number;                        // tcp server port number
    int16_t udp_port_number;                        // udp server port number

    string str_server_address;                      // server (IP) address

    sockaddr_in server_tcp_socket_data;             // socket address struct holding server TCP data
    sockaddr_in server_udp_socket_data;             // socket address struct holding server UDP data
    sockaddr_in client_udp_socket_data;             // socket address struct holding client UDP data

    char message_buffer[MAX_MSG_SIZE + 1];          // buffer that will receive the incoming data

public:

    CommunicationServer(char*, int16_t, int16_t);
    ~CommunicationServer();

    /* Init socket connection */
    int init();

    /* Initializes a UDP server */
    int init_udp_server();

    /* thread function to handle incomming connection*/
    static void* handle_peer_tcp_connection(CommunicationServer*);

    /* get peer details to connect with */
    PeerData* get_peer_details(string&);

};

#endif // COMMUNICATIONSERVER_H

