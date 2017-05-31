#include "peerholder.h"

PeerHolder* PeerHolder::peer_holder_instance = nullptr;

PeerHolder::PeerHolder()
{
    pthread_mutex_init(&registration_mutex, nullptr);
}

bool PeerHolder::register_peer(PeerData *peer)
{
    pthread_mutex_lock(&registration_mutex);
    peers[peer->username] = peer;
    pthread_mutex_unlock(&registration_mutex);
}

PeerData* PeerHolder::get_peer_by_username(string& p_username)
{
    if (peers.find(p_username) == peers.end())
        return (PeerData*) NULL;
    pthread_mutex_lock(&registration_mutex);
    PeerData *peer_data = peers[p_username];
    pthread_mutex_unlock(&registration_mutex);
    return peer_data;
}

PeerHolder* PeerHolder::get_instance()
{
    if (peer_holder_instance == nullptr)
        peer_holder_instance = new PeerHolder();
    return peer_holder_instance;
}


PeerHolder::~PeerHolder()
{
    pthread_mutex_destroy(&registration_mutex);
    delete peer_holder_instance;
    peer_holder_instance = nullptr;
}
