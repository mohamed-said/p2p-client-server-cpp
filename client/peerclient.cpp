#include "peerclient.h"


PeerClient::PeerClient(char *p_server_name, int p_port_number)
{
    *port_number = *p_port_number;
    server_name = p_server_name;
}


/** returns 0 for success or error_code for errors */
int PeerClient::init()
{
    /**
     * TODO
     * init socket data and connect to the server
     */
    memset(tcp_message_buffer, 0, MAX_TCP_MSG_SIZE + 1);
    memset(udp_message_buffer, 0, MAX_UDP_MSG_SIZE + 1);
    memset(&server_socket_address, 0, sizeof(server_socket_address));

    server = gethostbyname(server_name.c_str());
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        return NO_SUCH_HOST_ERROR;
    }

    /** fill the sockaddr_in struct with corresponding data */
    server_socket_address.sin_family = AF_INET;
    server_socket_address.sin_port = htonl(port_number);
    server_socket_address.sin_addr = *(in_addr*) server->h_addr;

    puts("Please Enter a username (max 20 chars) : ");
    short chr_count = scanf("%s", &username);
    while (chr_count > 20)
    {
        puts("Please Enter a valid username (max 20 chars) : ");
        memset(username, 0, 20);
        chr_count = scanf("%s", &username);
    }
    return 0;
}

int PeerClient::start_peer_communication()
{
    /**
     * run a thread handling the p2p message sending and receiving
     */
}




/**
 * TCP message
 *  returns 0 for success or error_code for errors
 */
int PeerClient::send_register_message()
{
    /** creating a TCP socket file descriptor */
    tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0)
    {
        fprintf(stderr, "ERROR, creating the TCP socket\n");
        return errno;
    }

    short connect_error = connect(tcp_socket_fd,
                                  (sockaddr*) &server_socket_address,
                                  (socklen_t) sizeof(server_socket_address));

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
    if (recv_error || recv_error == -1)
    {
        fprintf(stderr, "ERROR, readeing response from serve\n");
        return errno;
    }

    if (strcmp("SENDFIRSTUDP", tcp_message_buffer) == 0)
    {
        send_udp_first_msg();
    }
    else
    {
        puts("Server isn't ready for receiving UDP Message\n");
    }

    printf("Server response upon registration: %s\n", tcp_message_buffer);

    close(tcp_socket_fd);
    return 0;
}



/**
 * TCP message
 * returns 0 for success or error_code for errors
 */
int PeerClient::send_peer_connection_request(string &p_username)
{
    tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0)
    {
        fprintf(stderr, "ERRPR, creating peer connection TCP socket\n");
        return errno;
    }

    short connect_error = connect(tcp_socket_fd,
                                  (sockaddr*) &server_socket_address,
                                  (socklen_t) sizeof(server_socket_address));
    if (connect_error)
    {
        fprintf(stderr, "ERROR, connecting to server\n");
        return errno;
    }

    /**
     * Temporary Logic Implementation
     * ------------------------------------------------------------------------
     * We ask the user to enter the name of the peer he wants to connect to
     * and the server send us back the UDP address of that peer
     * so that I can start p2p messaging with that user
     */

    char peer_username[20];
    puts("Please enter a username to connect to (max 20 chars): ");
    short chr_count = scanf("%s", &peer_username);
    while (chr_count > 20)
    {
        puts("Please Enter a valid username (max 20 chars) : ");
        memset(peer_username, 0, 20);
        chr_count = scanf("%s", &peer_username);
    }

    /** ----------------------------------------------------------------------- */

    strcpy(tcp_message_buffer, p_username.c_str());

    short send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderrr, "ERROR, sending peer username to server\n");
        return errno;
    }

    short recv_error = recv(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, "ERROR, receiving response from server\n");
        return errno;
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
    /** creating a UDP socket file descriptor */
    udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0)
    {
        fprintf(stderr, "ERROR, creating the UDP socket\n");
        return errno;
    }

    /*
     * TODO
     * initialize the udp buffer with the necessary data
     */

    short sendto_error = sendto(udp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE + 1,
           MSG_NOSIGNAL | MSG_CONFIRM,
           (sockaddr*) &server_socket_address, (socklen_t) sizeof(server_socket_address));

    if (sendto_error  == -1)
    {
        fprintf(stderr, "ERROR, sending UDP message\n");
        return errno;
    }

    /** Now we can start sending and receiving p2p messages */
    start_peer_communication();

}

