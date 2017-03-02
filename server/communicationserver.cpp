#include "communicationserver.h"


CommunicationServer::CommunicationServer(char *p_server_address, int p_port_number) {
    port_number = p_port_number;
    str_server_address.assign(p_server_address);
}


/* TODO: this method should return an integet indicating correctness*/

int CommunicationServer::init()
{
    server_socket_data.sin_family = AF_INET;
    server_socket_data.sin_addr.s_addr = INADDR_ANY;
    server_socket_data.sin_port = htons(port_number);

    socket_address_length = sizeof(server_socket_data);

    tcp_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_server_socket_fd < 0)
    {
        fprintf(stderr, "ERROR creating socket\n");
        return errno;
    }

    //bind(tcp_socket_fd, (sockaddr*) &server_socket_data, sizeof(server_socket_data));

    if (bind(tcp_server_socket_fd, (sockaddr*) &server_socket_data, (socklen_t) sizeof(server_socket_data)))
    {
        fprintf(stderr, "ERROR binding address\n");
        return errno;
    }


    short listen_error = listen(tcp_server_socket_fd, 10);
    if (listen_error < 0)
    {
        fprintf(stderr, "ERROR, can't listen on the socket\n");
        return errno;
    }
    else
    {
        puts(" * Server is running");
        printf(" * Address: %s:%d\n", str_server_address.c_str(), port_number);
    }



    pthread_t thread_id;


    while (true)
    {
        printf("Waiting...\n");
        tcp_client_socket_fd = accept(tcp_server_socket_fd, (sockaddr*) &server_socket_data, &socket_address_length);
        if (tcp_client_socket_fd < 0)
        {
            fprintf(stderr, "ERROR accepting client connection\n");
            return errno;
        }

        short pthread_error = pthread_create(&thread_id, NULL, (void*(*)(void*))handle_peer_tcp_connection, this);
        if (pthread_error)
        {
            /**
             * EAGAIN (can't create another thread, max is reached)
             * EINVAL (The value specified by attr is invalid)
             * EPERM  (The caller does not have appropriate permission to set the required scheduling parameters or scheduling policy)
             */
            fprintf(stderr, "ERROR, could not create peer client thread\n");
            return pthread_error; // pthread error code

        }

        puts("Connection Accepted");
    }
    return 0; // success
}

void* CommunicationServer::handle_peer_tcp_connection(CommunicationServer *__server_obj)
{
    int16_t socket_descriptor = __server_obj->tcp_client_socket_fd;

    int16_t read_error = read(socket_descriptor, __server_obj->message_buffer, sizeof(__server_obj->message_buffer));
    if (read_error < 0)
    {
        fprintf(stderr, " * ERROR, reading message from the client\n");
        printf(" * (errno) -> %d\n", errno);
    }


    printf("Client Sent: %s\n", __server_obj->message_buffer);

    /***
     * TODO
     * check if the incoming message is a register message or
     * a peer connection messgae (asking to connect with some peer)
     */


    strcpy(__server_obj->message_buffer, "SENDFIRSTUDP");

    int16_t send_error = send(socket_descriptor, __server_obj->message_buffer, sizeof(__server_obj->message_buffer), MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, " * ERROR, sending response to client\n");
        printf(" * (errno) -> %d\n", errno);
    }
    else
    {
        puts(" * Triggered First UDP");
    }

    __server_obj->socket_size = sizeof(__server_obj->server_socket_data);
    int16_t recv_from_error = recvfrom(__server_obj->tcp_server_socket_fd,              /* socket file */
                                       __server_obj->message_buffer,                    /* buffer to receive into */
                                       sizeof(__server_obj->message_buffer),            /* size of buffer */
                                       MSG_CONFIRM,                                     /* flags */
                                       (sockaddr*) &__server_obj->server_socket_data,   /* socket address */
                                       &__server_obj->socket_size);                     /* size of socket address */


    if (recv_from_error == -1)
    {
        fprintf(stderr, " * ERROR, receiving first UDP message");
        printf(" * (errno) -> %d\n", errno);
    }

    printf("Client username: %s\n", __server_obj->message_buffer);

    PeerData *peer_data = new PeerData();
    peer_data->client_sockert_address = __server_obj->server_socket_data;
    peer_data->username = __server_obj->message_buffer;

    // PeerHolder::get_instance()->register_peer(peer_data);

    strcpy(__server_obj->message_buffer, "REGISTERED SUCCESSFULLY");

    send_error = send(socket_descriptor, __server_obj->message_buffer, sizeof(__server_obj->message_buffer), MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, " * ERROR, sending registration response\n");
        printf(" * (errno) -> %d\n", errno);
    }

}

/** get peer details to connect with */
PeerData* CommunicationServer::get_peer_details(string& p_username)
{
    return PeerHolder::get_instance()->get_peer_by_username(p_username);
}
