#ifndef PEERDATA_H
#define PEERDATA_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

struct PeerData {

    int socket_fd;
    short port_number;
    int peer_id;
    string server_address;
    string username;
    sockaddr_in server_sockert_address;

};



#endif // PEERDATA_H
