#ifndef COMMUNICATIONSERVER_H
#define COMMUNICATIONSERVER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>


#include "peerholder.h"
#include "peerdata.h"

#define MAX_MSG_SIZE  2500
//#define SOCKET_ERROR  1000
//#define BIND_ERROR    2000
//#define LISTEN_ERROR  3000
//#define ACCEPT_ERROR  4000
//#define PTHREAD_ERROR 5000

using namespace std;

class CommunicationServer {

private:

    int16_t tcp_server_socket_fd;                   // socket file descriptor
    int16_t tcp_client_socket_fd;                   // socket file descriptor to read data upon
    socklen_t socket_size;
    int port_number;                                // server port number
    socklen_t socket_address_length;                // length of peer address
    string str_server_address;                      // server (IP) address
    sockaddr_in server_socket_data;                 // socket address struct holding socket family type, port and ip address
    char message_buffer[MAX_MSG_SIZE + 1];          // buffer that will receive the incoming data

public:

    CommunicationServer(char *p_server_address, int p_port_number);

    /* Init socket connection */
    int init();

    /* thread function to handle incomming connection*/
    static void* handle_peer_tcp_connection(CommunicationServer*);

    /* get peer details to connect with */
    PeerData* get_peer_details(string&);

};

#endif // COMMUNICATIONSERVER_H

