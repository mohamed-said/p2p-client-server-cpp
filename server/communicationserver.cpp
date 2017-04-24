#include "communicationserver.h"


CommunicationServer::CommunicationServer(char *p_server_address, int16_t p_tcp_port_number, int16_t p_udp_port_number)
{
    tcp_port_number = p_tcp_port_number;
    udp_port_number = p_udp_port_number;
    str_server_address.assign(p_server_address);
    socket_address_size = sizeof(server_tcp_socket_data);
}


CommunicationServer::~CommunicationServer()
{
    close(tcp_server_socket_fd);
    close(udp_server_socket_fd);
    close(tcp_client_socket_fd);
}


/* TODO: this method should return an integet indicating correctness*/

int CommunicationServer::init()
{
    server_tcp_socket_data.sin_family = AF_INET;
    server_tcp_socket_data.sin_port = htons(tcp_port_number);
    server_tcp_socket_data.sin_addr.s_addr = INADDR_ANY;

    tcp_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_server_socket_fd < 0)
    {
        fprintf(stderr, "ERROR creating TCP socket\n");
        return errno;
    }

    if (bind(tcp_server_socket_fd, (sockaddr*) &server_tcp_socket_data, socket_address_size))
    {
        fprintf(stderr, "ERROR binding TCP address\n");
        return errno;
    }

    int16_t listen_error = listen(tcp_server_socket_fd, 10);
    if (listen_error < 0)
    {
        fprintf(stderr, "ERROR, can't listen on the socket\n");
        return errno;
    }
    else
    {
        puts(" * Server is running");
        printf(" * Address: %s:%d\n", str_server_address.c_str(), tcp_port_number);
    }


    int16_t udp_init_error = init_udp_server();
    if (udp_init_error)
    {
        fprintf(stderr, "ERROR, binding UDP server\n");
        printf(" * (errno) -> %d\n", udp_init_error);
    }

    pthread_t thread_id;

    while (true)
    {
        printf("Waiting...\n");
        tcp_client_socket_fd = accept(tcp_server_socket_fd, (sockaddr*) &server_tcp_socket_data, &socket_address_size);
        if (tcp_client_socket_fd < 0)
        {
            fprintf(stderr, "ERROR accepting client connection\n");
            return errno;
        }
//        void*(*) task = decideTask();

        int16_t pthread_error = pthread_create(&thread_id, NULL, (void*(*)(void*))handle_peer_tcp_connection, this);
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

int CommunicationServer::init_udp_server()
{
    udp_server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_server_socket_fd < 0)
    {
        fprintf(stderr, "ERROR, creating UDP socket\n");
        return errno;
    }

    memset(&server_udp_socket_data, 0, socket_address_size);
    memset(&client_udp_socket_data, 0, socket_address_size);

    server_udp_socket_data.sin_family = AF_INET;
    server_udp_socket_data.sin_port = htons(udp_port_number);
    server_udp_socket_data.sin_addr.s_addr = INADDR_ANY;

    if ( bind(udp_server_socket_fd, (sockaddr*) &server_udp_socket_data, socket_address_size) )
    {
        fprintf(stderr, "ERROR, binding UDP address\n");
        return errno;
    }

    return 0; // success

}

void* CommunicationServer::handle_peer_tcp_connection(CommunicationServer *__server_obj)
{

    int16_t send_error;

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

    if (strcmp(__server_obj->message_buffer, "REGISTER") == 0)
    {
        strcpy(__server_obj->message_buffer, "SENDFIRSTUDP");

        // Bonne Voyage
        send_error = send(socket_descriptor, __server_obj->message_buffer, sizeof(__server_obj->message_buffer), MSG_NOSIGNAL);
        if (send_error == -1)
        {
            fprintf(stderr, " * ERROR, sending response to client\n");
            printf(" * (errno) -> %d\n", errno);
        }
        else
        {
            puts(" * Triggered First UDP");
        }

        int16_t recv_from_len = recvfrom(__server_obj->udp_server_socket_fd,                   /* socket file */
                                           __server_obj->message_buffer,                       /* buffer to receive into */
                                           sizeof(__server_obj->message_buffer),               /* size of buffer */
                                           MSG_CONFIRM,                                        /* flags */
                                           (sockaddr*) &__server_obj->client_udp_socket_data,  /* socket address */
                                           &__server_obj->socket_address_size);                /* size of socket address */

        cout << "Address: " <<  inet_ntoa(__server_obj->client_udp_socket_data.sin_addr) << endl;
        cout << "Port: " << ntohs(__server_obj->client_udp_socket_data.sin_port) << endl;

        if (recv_from_len == -1)
        {
            fprintf(stderr, " * ERROR, receiving first UDP message");
            printf(" * (errno) -> %d\n", errno);
        }
        else
        {
            puts("UDP Message received!!!");
        }

        printf("Client username: %s\n", __server_obj->message_buffer);
        puts("---------------------");

        PeerData *peer_data = new PeerData();
        peer_data->client_sockert_address = __server_obj->client_udp_socket_data;
        peer_data->username = __server_obj->message_buffer;
        // maybe we should check if the username already exists (maybe)
        PeerHolder::get_instance()->register_peer(peer_data);

        strcpy(__server_obj->message_buffer, "REGISTERED SUCCESSFULLY");

        send_error = send(socket_descriptor, __server_obj->message_buffer,
                          sizeof(__server_obj->message_buffer), MSG_NOSIGNAL);

        if (send_error == -1)
        {
            fprintf(stderr, " * ERROR, sending registration response\n");
            printf(" * (errno) -> %d\n", errno);
        }

    }
    else if (strcmp(__server_obj->message_buffer, "REQUESTPEER") == 0)
    {
        strcpy(__server_obj->message_buffer, "SENDUSERNAME");

        // Bonne Voyage
        send_error = send(socket_descriptor, __server_obj->message_buffer, sizeof(__server_obj->message_buffer), MSG_NOSIGNAL);
        if (send_error == -1)
        {
            fprintf(stderr, " * ERROR, sending response to client\n");
            printf(" * (errno) -> %d\n", errno);
        }
        else
        {
            puts(" * Waiting for username");
        }

        // read username
        int16_t read_error = read(socket_descriptor, __server_obj->message_buffer, sizeof(__server_obj->message_buffer));
        if (read_error < 0)
        {
            fprintf(stderr, " * ERROR, reading message from the client\n");
            printf(" * (errno) -> %d\n", errno);
        }
        else
        {
            printf(" * Username received ...\n");
        }

        string requested_username = __server_obj->message_buffer;
        PeerData *peer_data = __server_obj->get_peer_details(requested_username);

        if (peer_data == NULL)
        {
            puts("Username not found!!!");
            strcpy(__server_obj->message_buffer, "USERNAMENOTFOUND");
        }
        else
        {
            printf(" * Username \" %s \" found.\n", __server_obj->message_buffer);

            memset(__server_obj->message_buffer, 0, MAX_MSG_SIZE + 1);
            // copy the peer data to a byte array buffer to send back to the requesting client

            memcpy(__server_obj->message_buffer,
                   &peer_data->client_sockert_address.sin_addr,
                   sizeof(peer_data->client_sockert_address.sin_addr));
            memcpy(__server_obj->message_buffer + sizeof(peer_data->client_sockert_address.sin_addr.s_addr),
                   &peer_data->client_sockert_address.sin_port,
                   sizeof(peer_data->client_sockert_address.sin_port));

//            printf("Message Length: %d\n", strlen(__server_obj->message_buffer));

//            in_addr addr;
//            memcpy(&addr.s_addr, __server_obj->message_buffer, 4);
//            uint16_t prt;
//            memcpy(&prt, __server_obj->message_buffer + 4, 2);

//            printf("Addr: %s\n", inet_ntoa(addr));
//            printf("Port: %hu\n", ntohs(prt));
        }

        // Bonne Voyage
        send_error = send(socket_descriptor, __server_obj->message_buffer,
                          sizeof(__server_obj->message_buffer), MSG_NOSIGNAL);

        if (send_error == -1)
        {
            fprintf(stderr, " * ERROR, sending peer details\n");
            printf(" * (errno) -> %d\n", errno);
        }
        else
        {
            puts("Response sent successfully.");
            puts("----------------------------");
        }

    }
    else
    {
        puts("Unknown Command or Request");
        return (void*) NULL;
    }

    /*
     * I've seen things you people wouldn't believe.
     * Attack ships on fire off the shoulder of Orion.
     * I watched C-beams glitter in the dark near the Tannhäuser Gate.
     * All those moments will be lost in time, like tears in rain.
     * Time to die :)
    */
    /* Thread Dies Here :) */
}

/** get peer details to connect with */
PeerData* CommunicationServer::get_peer_details(string& p_username)
{
    return PeerHolder::get_instance()->get_peer_by_username(p_username);
}
