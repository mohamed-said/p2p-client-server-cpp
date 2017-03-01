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

        short pthread_error = pthread_create(&thread_id, NULL, (void*(*)(void*))handle_peer_tcp_connection, (void*) &tcp_client_socket_fd);
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

void* CommunicationServer::handle_peer_tcp_connection(void* client_socket)
{
    puts(" *** So far so good !!!");
    char message[MAX_MSG_SIZE + 1];
    puts(" *** Got here !!");
    int16_t socket_descriptor = *(int16_t*)client_socket;

    puts("So what??");

    int16_t read_error = read(socket_descriptor, message, sizeof(message));
    if (read_error < 0)
    {
        fprintf(stderr, " * ERROR, reading message from the client\n");
        printf(" * (errno) -> %d\n", errno);
    }

    /**
      *
      * Temporary Implementation
      * Show the comming messages to test that the server
      * accept incoming connections
      *
     */

    printf("Client Sent: %s\n", message);

    strcpy(message, "SENDFIRSTUDP");

    int16_t send_error = send(socket_descriptor, message, sizeof(message), MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, " * ERROR, sending response to client\n");
        printf(" * (errno) -> %d\n", errno);
    }
    else
    {
        puts(" * Responded Successfully");
    }
    /***
     * TODO
     * check if the incoming message is a register message or
     * a peer connection messgae (asking to connect with some peer)
     * /

    PeerData *peer_data = new PeerData();
    /***
     * TODO
     * parse the message buffer that's filled in the read method
     * and fill the peer_data object
     */

    // PeerHolder::get_instance()->register_peer(peer_data);

}

/** get peer details to connect with */
PeerData* CommunicationServer::get_peer_details(string& p_username)
{
    return PeerHolder::get_instance()->get_peer_by_username(p_username);
}
