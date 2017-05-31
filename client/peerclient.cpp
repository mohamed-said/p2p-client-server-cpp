#include "peerclient.h"


PeerClient::PeerClient(char *p_server_name, int16_t p_tcp_port_number, int16_t p_udp_port_number)
{
    tcp_port_number = p_tcp_port_number;
    udp_port_number = p_udp_port_number;
    server_name = p_server_name;
    my_username = new char[21];
    crypto = new encryption_methods();
}

PeerClient::~PeerClient()
{
    close(tcp_socket_fd);
    close(udp_socket_fd);
    delete server;
    delete server_name;
    delete my_username;
    server = nullptr;
    server_name = nullptr;
    my_username = nullptr;
}

pthread_t PeerClient::get_send_thred_id()
{
    return send_thread_id;
}

pthread_t PeerClient::get_recv_thread_id()
{
    return recv_thread_id;
}

/** returns 0 for success or error_code for errors */
int PeerClient::init()
{
    /** creating a UDP socket file descriptor */
    udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0)
    {
        fprintf(stderr, " * ERROR, creating the UDP socket\n");
        return errno;
    }

    puts(" ** UDP socket created");

    memset(tcp_message_buffer, 0, MAX_TCP_MSG_SIZE + 1);
    memset(udp_message_buffer, 0, MAX_UDP_MSG_SIZE + 1);
    memset(&tcp_server_socket_address, 0, sizeof(tcp_server_socket_address));
    memset(&udp_server_socket_address, 0, sizeof(udp_server_socket_address));

    socket_length = sizeof(udp_server_socket_address);

    server = gethostbyname(server_name);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        return NO_SUCH_HOST_ERROR;
    }

    /** fill the tcp server socket with necessary data */
    tcp_server_socket_address.sin_family = AF_INET;
    tcp_server_socket_address.sin_port = htons(tcp_port_number);
    tcp_server_socket_address.sin_addr = *(in_addr*) server->h_addr;


    /** fill the udp server socket with the necessary data */
    udp_server_socket_address.sin_family = AF_INET;
    udp_server_socket_address.sin_port = htons(udp_port_number);
    udp_server_socket_address.sin_addr = *(in_addr*) server->h_addr;


    return 0;
}



void *PeerClient::run_p2p_send(PeerClient *__peer_client)
{
    char message_buffer[64];
//    int16_t crypto_key = __peer_client->crypto->generate_key(__peer_client->my_username);

    printf("You: ");
    while (1)
    {
        fgets(message_buffer, 64, stdin);
        message_buffer[strlen(message_buffer) - 1] = '\0';
        // here we encrypt the message
//        __peer_client->crypto->encrypt_message(message_buffer, crypto_key);
        int16_t sendto_error = sendto(__peer_client->udp_socket_fd, message_buffer, 64, 0,
                                      (sockaddr*) &__peer_client->peer_udp_socket_data,
                                      sizeof(__peer_client->peer_udp_socket_data));
        if (sendto_error == -1)
        {
            fprintf(stderr, " * ERROR, sending data to peer!!\n");
            printf(" * (errno) -> %d\n", errno);
        }
    }

}

void *PeerClient::run_p2p_recv(PeerClient *__peer_client)
{
    char message_buffer[64];
//    int16_t crypto_key = __peer_client->crypto->generate_key(__peer_client->peer_username);
    socklen_t addr_size = sizeof(__peer_client->peer_udp_socket_data);
    while (1)
    {
        memset(message_buffer, 0, 64);
        int16_t recvfrom_error = recvfrom(__peer_client->udp_socket_fd, message_buffer,
                                          64, MSG_CONFIRM,
                                          (sockaddr*) &__peer_client->peer_udp_socket_data,
                                           &addr_size);
        if (recvfrom_error == -1)
        {
            fprintf(stderr, " * ERROR, receiving message from peer\n");
            printf(" * (errno) -> %d\n", errno);
            printf("Address: %s\n", inet_ntoa(__peer_client->peer_udp_socket_data.sin_addr));
            printf("Port: %d\n", ntohs(__peer_client->peer_udp_socket_data.sin_port));
            printf("Message Buffer: %s\n", message_buffer);

        }
        else
        {
            // here we decrypt the message
//            __peer_client->crypto->decrypt_message(message_buffer, crypto_key);
            if ((strcmp(__peer_client->peer_username, "") != 0) || __peer_client->peer_username == NULL)
                printf(YEL "\n%s: %s\n" RESET, __peer_client->peer_username, message_buffer);
            else
                printf(YEL"\n%s: %s\n" RESET, inet_ntoa(__peer_client->peer_udp_socket_data.sin_addr), message_buffer);
            printf("You: ");
        }
    }
}


int PeerClient::start_peer_communication()
{
    int16_t pthread_error_send = pthread_create(&send_thread_id, 0, (void*(*)(void*))run_p2p_send, this);
    if (pthread_error_send)
    {
        fprintf(stderr, "ERROR, creating p2p send thread\n");
        return -1;
    }

    int16_t pthread_error_recv = pthread_create(&recv_thread_id, 0, (void*(*)(void*))run_p2p_recv, this);
    if (pthread_error_recv)
    {
        fprintf(stderr, "ERROR, creating p2p recv thread\n");
        return -1;
    }

    return 0;
}

/**
 * TCP message
 *  returns 0 for success or error_code for errors
 */
int PeerClient::send_register_message(char p_username[])
{
    strcpy(my_username, p_username);
    /** creating a TCP socket file descriptor */
    tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0)
    {
        fprintf(stderr, "ERROR, creating the TCP socket\n");
        return errno;
    }

    short connect_error = connect(tcp_socket_fd,
                                  (sockaddr*) &tcp_server_socket_address,
                                  (socklen_t) sizeof(tcp_server_socket_address));

    if (connect_error)
    {
        fprintf(stderr, "ERROR, connecting to TCP server\n");
        return errno;
    }


    strcpy(tcp_message_buffer, "REGISTER");

    short send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, "ERROR, sending data to host\n");
        return errno;
    }

    short recv_error = recv(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, "ERROR, readeing response from server\n");
        return errno;
    }
    
    /* temp  */
    printf("Server Sent: %s\n", tcp_message_buffer);
    
    if (strcmp("SENDFIRSTUDP", tcp_message_buffer) == 0)
    {
        puts(" * Server ready to receive first UDP message");
        return 0;
    }
    puts(" * Server isn't ready to receive UDP message\n");
    return -1;
}



/**
 * TCP message
 * returns 0 for success or error_code for errors
 */
int PeerClient::send_peer_connection_request()
{
    tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0)
    {
        fprintf(stderr, "ERROR, creating peer connection TCP socket\n");
        return errno;
    }

    short connect_error = connect(tcp_socket_fd,
                                  (sockaddr*) &tcp_server_socket_address,
                                  (socklen_t) sizeof(tcp_server_socket_address));
    if (connect_error)
    {
        fprintf(stderr, "ERROR, connecting to server\n");
        return errno;
    }


    strcpy(tcp_message_buffer, "REQUESTPEER");
    short send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, "ERROR, sending peer request to server\n");
        return errno;
    }

    short recv_error = recv(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, "ERROR, server not ready!!");
        return errno;
    }
    else if (strcmp(tcp_message_buffer, "SENDUSERNAME") == 0)
    {
        puts("Please enter a username to connect to (max 20 chars): ");
        fgets(peer_username, 20, stdin);
        peer_username[strlen(peer_username) - 1] = '\0';
        puts("Thanks\n");

        strcpy(tcp_message_buffer, peer_username);
    }
    else
    {
        fprintf(stderr, "ERROR, Invalid response, server not ready");
        return errno;
    }


//    printf("[DEBUGGING][send_peer_connection_request][tcp_message_buffer]: username before: %s\n", tcp_message_buffer);

    // sending username
    send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, "ERROR, sending peer username to server\n");
        return errno;
    }
    recv_error = recv(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, "ERROR, receiving response from server\n");
        return errno;
    }
    else if (strcmp(tcp_message_buffer, "USERNAMENOTFOUND") == 0)
    {
        fprintf(stderr, "ERROR, Username is not found!!!\n");
        return -1;
    }
    else
    {
        puts("Peer Data received successfully");

        peer_udp_socket_data.sin_family = AF_INET;
        memcpy(&peer_udp_socket_data.sin_addr.s_addr, tcp_message_buffer, 4);
        memcpy(&peer_udp_socket_data.sin_port, tcp_message_buffer + 4, 2);


        in_addr addr;
        memcpy(&addr.s_addr, tcp_message_buffer, 4);
        uint16_t prt;
        memcpy(&prt, tcp_message_buffer + 4, 2);

        printf("Addr: %s\n", inet_ntoa(addr));
        printf("Port: %hu\n", ntohs(prt));

        puts("-------------------------------");
    }

    return 0;
}

/**
 * UDP message
 * returns 0 for success and error_code for errors
 */
int PeerClient::send_udp_first_msg()
{
    puts(" ** Sending UDP first message");

    strcpy(udp_message_buffer, my_username);

    int16_t sendto_error = sendto(udp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE,
           0, (sockaddr*) &udp_server_socket_address, socket_length);

    if (sendto_error  == -1)
    {
        fprintf(stderr, " * ERROR, sending UDP message\n");
        return errno;
    }

    puts(" ** UDP message sent");

    int16_t recv_error = recv(tcp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, " * ERROR, receving first UDP response from server\n");
        return errno;
    }

    puts("REGISTERED SUCCESSFULLY");

    return 0;
}

