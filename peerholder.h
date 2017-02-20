#ifndef PEERHOLDER_H
#define PEERHOLDER_H


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

#include "peerdata.h"

#define MSG_MAX_LENGTH 100

using namespace std;

/** Singleton peer holder */
class PeerHolder
{

private:
    map<string, PeerData*> peers;
    pthread_mutex_t *registration_mutex;
    static PeerHolder *peer_holder_instance;

    PeerHolder();
    void init();

public:

    static PeerHolder* get_instance();
    bool register_peer(PeerData *peer);
    PeerData* get_peer_by_username(string&);

};

#endif // PEERHOLDER_H
