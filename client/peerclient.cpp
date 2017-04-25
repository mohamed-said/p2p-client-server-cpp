#include "peerclient.h"


PeerClient::PeerClient(char *p_server_name, int16_t p_tcp_port_number, int16_t p_udp_port_number)
{
    tcp_port_number = p_tcp_port_number;
    udp_port_number = p_udp_port_number;
    server_name = p_server_name;
    username = new char[21];
}


/** returns 0 for success or error_code for errors */
int PeerClient::init()
{
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



void *PeerClient::run_p2p_send(void *args) {}

void *PeerClient::run_p2p_recv(void *args) {}


int PeerClient::start_peer_communication()
{

    /**
     * TODO
     * run a thread handling the p2p message sending and receiving
     */

    short pthread_error_send = pthread_create(&send_thread_id, 0, run_p2p_send, 0);
    if (pthread_error_send)
    {
        fprintf(stderr, "ERROR, creating p2p send thread\n");
        return -1;
    }

    short pthread_error_recv = pthread_create(&recv_thread_id, 0, run_p2p_recv, 0);
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
    strcpy(username, p_username);
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

    short send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, "ERROR, sending data to host\n");
        return errno;
    }

    short recv_error = recv(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, "ERROR, readeing response from server\n");
        return errno;
    }
    
    /* temp  */
    printf("Server Sent: %s\n", tcp_message_buffer);

//    close(tcp_socket_fd);
    
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
        fprintf(stderr, "ERRPR, creating peer connection TCP socket\n");
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
    short send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, "ERROR, sending peer request to server\n");
        return errno;
    }

    short recv_error = recv(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, "ERROR, server not ready!!");
        return errno;
    } else if (strcmp(tcp_message_buffer, "SENDUSERNAME") == 0)
    {
        char peer_username[21];
        puts("Please enter a username to connect to (max 20 chars): ");
        short chr_count = scanf("%s", peer_username);
        while (chr_count > 20)
        {
            puts("Please Enter a valid username (max 20 chars) : ");
            memset(peer_username, 0, 20);
            chr_count = scanf("%s", peer_username);
        }
        puts("Thanks\n");

        strcpy(tcp_message_buffer, peer_username);
    }
    else
    {
        fprintf(stderr, "ERROR, Invalid response, server not ready");
        return errno;
    }

    // sending username
    send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, "ERROR, sending peer username to server\n");
        return errno;
    }

    recv_error = recv(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
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
        printf("[DEBUGGING][send_peer_connection][tcp_message_buffer]: %s\n", tcp_message_buffer);
        in_addr addr;
        memcpy(&addr.s_addr, tcp_message_buffer, 4);
        uint16_t prt;
        memcpy(&prt, tcp_message_buffer + 4, 2);

        printf("Addr: %s\n", inet_ntoa(addr));
        printf("Port: %hu\n", ntohs(prt));
        puts("-------------------------------");
    }

    close(tcp_socket_fd);
    return 0;
}

/**
 * UDP message
 * returns 0 for success and error_code for errors
 */
int PeerClient::send_udp_first_msg()
{
    puts(" ** Sending UDP first message");
    /** creating a UDP socket file descriptor */
    udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0)
    {
        fprintf(stderr, " * ERROR, creating the UDP socket\n");
        return errno;
    }

    puts(" ** UDP socket created");

    strcpy(udp_message_buffer, username);
    printf("DEBUGGING: usr: %s - %s\n", udp_message_buffer, username);
    short sendto_error = sendto(udp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE + 1,
           0, (sockaddr*) &udp_server_socket_address, socket_length);


    if (sendto_error  == -1)
    {
        fprintf(stderr, " * ERROR, sending UDP message\n");
        return errno;
    }

    puts(" ** UDP message sent");

    /***
     * TODO
     * check if I will receive the response in TCP or UDP
     * I recommend TCP ,, because it's a control message
     */


    int16_t recv_error = recv(tcp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, " * ERROR, receving first UDP response from server\n");
        return errno;
    }

    puts("REGISTERED SUCCESSFULLY");

    /** Now we can start sending and receiving p2p messages */
//    start_peer_communication();
    return 0;
}

