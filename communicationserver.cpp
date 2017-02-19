#include "communicationserver.h"


CommunicationServer::CommunicationServer(char *p_server_address, int p_port_number) {
    port_number = p_port_number;
    str_server_address.assign(p_server_address);
    peer_address_length = sizeof(struct sockaddr_in);
    init();
}


/* TODO: this method should return an integet indicating correctness*/

int CommunicationServer::init()
{
    server_socket_data.sin_family = AF_INET;
    server_socket_data.sin_addr.s_addr = INADDR_ANY;
    server_socket_data.sin_port = htonl(port_number);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        fprintf(stderr, "ERROR creating socket\n");
        return SOCKET_ERROR;
    }

    if (bind(socket_fd, (sockaddr*) &server_socket_data, sizeof(server_socket_data)))
    {
        fprintf(stderr, "ERROR binding address\n");
        return BIND_ERROR;
    }

    short listen_error = listen(socket_fd, 10);
    if (listen_error < 0)
    {
        fprintf(stderr, "ERROR, can't listen on the socket\n");
        return LISTEN_ERROR;
    }

    pthread_t thread_id;

    while (true)
    {
        short accept_error = accept(socket_fd, (sockaddr*) &server_socket_data, (socklen_t*) sizeof(server_socket_data));
        if (accept_error < 0)
        {
            fprintf(stderr, "ERROR accepting client connection\n");
            return ACCEPT_ERROR;
        }

        short pthread_error = pthread_create(&thread_id, NULL, (void*(*)(void*))handle_peer_tcp_connection, this);
        if (pthread_error)
        {
            fprintf(stderr, "ERROR, could not create peer client thread\n");
            return PTHREAD_ERROR;
        }

        puts("Client Accepted");
    }
    return 0; // success
}

void* CommunicationServer::handle_peer_tcp_connection(CommunicationServer *__communication_server)
{
    int socket_descriptor = __communication_server->socket_fd;
    short read_error = read(socket_descriptor, __communication_server->message_buffer, MAX_MSG_SIZE);
    if (read_error < 0 || read_error < MAX_MSG_SIZE)
    {
        fprintf(stderr, "ERROR, reading data from socket\n");
    }

    PeerData *peer_data = new PeerData();
    /***
     * TODO
     * parse the message buffer that's filled in the read method
     * and fill the peer_data object
     */

    PeerHolder::get_instance()->register_peer(peer_data);
}

/** get peer details to connect with */
PeerData* CommunicationServer::get_peer_details(string& p_username)
{
    return PeerHolder::get_instance()->get_peer_by_username(p_username);
}
