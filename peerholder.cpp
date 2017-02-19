#include "peerholder.h"

PeerHolder* PeerHolder::peer_holder_instance = NULL;

PeerHolder::PeerHolder()
{
    init();
}

void PeerHolder::init()
{
    pthread_mutex_init(registration_mutex, NULL);
}

bool PeerHolder::register_peer(PeerData &peer)
{
    pthread_mutex_lock(registration_mutex);
    peers[peer.username] = peer;
    pthread_mutex_unlock(registration_mutex);
}

PeerData* PeerHolder::get_peer_by_username(string& p_username)
{
    pthread_mutex_lock(registration_mutex);
    PeerData *peer_data = peers[p_username];
    pthread_mutex_unlock(registration_mutex);
    return peer_data;
}

static PeerHolder* PeerHolder::get_instance()
{
    if (peer_holder_instance == NULL)
        peer_holder_instance = new PeerHolder();
    return peer_holder_instance;
}

