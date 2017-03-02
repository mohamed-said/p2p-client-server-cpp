#ifndef PEERDATA_H
#define PEERDATA_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

struct PeerData {

    int socket_fd;
    int peer_id;
    string username;
    sockaddr_in client_sockert_address;

};



#endif // PEERDATA_H
