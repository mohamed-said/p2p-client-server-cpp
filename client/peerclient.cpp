#include "peerclient.h"


PeerClient::PeerClient(char *p_server_name, int p_port_number, char *p_username)
{
    port_number = p_port_number;
    server_name = p_server_name;
    username = p_username;
}


/** returns 0 for success or error_code for errors */
int PeerClient::init()
{

    memset(tcp_message_buffer, 0, MAX_TCP_MSG_SIZE + 1);
    memset(udp_message_buffer, 0, MAX_UDP_MSG_SIZE + 1);
    memset(&tcp_server_socket_address, 0, sizeof(tcp_server_socket_address));
    memset(&udp_server_socket_address, 0, sizeof(udp_server_socket_address));

    server = gethostbyname(server_name);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        return NO_SUCH_HOST_ERROR;
    }

    /** fill the tcp server socket with necessary data */
    tcp_server_socket_address.sin_family = AF_INET;
    tcp_server_socket_address.sin_port = htons(port_number);
    tcp_server_socket_address.sin_addr = *(in_addr*) server->h_addr;


    /** fill the udp server socket with the necessary data */
    udp_server_socket_address.sin_family = AF_INET;
    udp_server_socket_address.sin_port = htons(port_number);
    udp_server_socket_address.sin_addr = *(in_addr*) server->h_addr;


    return 0;
}



void *PeerClient::run_p2p_send(void *args)
{
/*
    // Handle p2p receive

    short recv_error = 0;
    while (true)
    {
        recv_error = recvfrom(udp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE + 1,
                                    MSG_NOSIGNAL | MSG_CONFIRM,
                                    (sockaddr*) &udp_server_socket_address,
                                    (socklen_t) sizeof(udp_server_socket_address));

       
        // TODO
        // should handle a proper way to stop the thread
       if (recv_error == -1)
       {
            fprintf(stderr, "ERROR, receiving message from peer!!!\n");
       }
       printf("%s\n", udp_message_buffer);
    }
*/
}

void *PeerClient::run_p2p_recv(void *args)
{
/*
    short send_error = 0; // success by default
    while (true)
    {
        puts("Enter your message: ");

        // should check if the message size is correct 
        scanf("%s", &udp_message_buffer);

        send_error = send(udp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE + 1, MSG_CONFIRM);

        
         // TODO
         // should handle a proper way to stop the thread !

        if (send_error == -1)
        {
            fprintf(stderr, "ERROR, sending message to peer!!\n");
        }
    }
*/
}


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

    close(tcp_socket_fd);
    
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

    /**
     * Temporary Logic Implementation
     * ------------------------------------------------------------------------
     * We ask the user to enter the name of the peer he wants to connect to
     * and the server send us back the UDP address of that peer
     * so that I can start p2p messaging with that user
     */

    char peer_username[20];
    puts("Please enter a username to connect to (max 20 chars): ");
    short chr_count = scanf("%s", peer_username);
    while (chr_count > 20)
    {
        puts("Please Enter a valid username (max 20 chars) : ");
        memset(peer_username, 0, 20);
        chr_count = scanf("%s", peer_username);
    }
    puts("Thanks\n");

    /** ----------------------------------------------------------------------- */

    strcpy(tcp_message_buffer, peer_username);

    short send_error = send(tcp_socket_fd, tcp_message_buffer, MAX_TCP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (send_error == -1)
    {
        fprintf(stderr, "ERROR, sending peer username to server\n");
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

    short sendto_error = sendto(udp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE + 1,
           MSG_NOSIGNAL | MSG_CONFIRM,
           (sockaddr*) &udp_server_socket_address, sizeof(udp_server_socket_address));

    puts(" ** UDP message sent");

    if (sendto_error  == -1)
    {
        fprintf(stderr, " * ERROR, sending UDP message\n");
        return errno;
    }

    /***
     * TODO
     * check if I will receive the response in TCP or UDP
     * I recommend TCP ,, because it's a control message
     */

    int16_t recv_error = recv(tcp_socket_fd, udp_message_buffer, MAX_UDP_MSG_SIZE + 1, MSG_NOSIGNAL);
    if (recv_error == -1)
    {
        fprintf(stderr, " * ERROR, receving first UDP response from server");
        return errno;
    }

    puts("REGISTERED SUCCESSFULLY");

    /** Now we can start sending and receiving p2p messages */
//    start_peer_communication();
    return 0;
}

