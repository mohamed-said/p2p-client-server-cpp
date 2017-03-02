#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <string.h>

#define MAX_TCP_MSG_SIZE 2500
#define MAX_UDP_MSG_SIZE 64


#define NO_SUCH_HOST_ERROR 1000

class PeerClient {

private:

    int tcp_socket_fd;                      /* TCP socket descriptor */
    int udp_socket_fd;                      /* UDP socket descriptor */
    int port_number;                        /* Server port number to connect to */
    hostent *server;                        /* Server address */
    sockaddr_in tcp_server_socket_address;  /* TCP server socket data */
    sockaddr_in udp_server_socket_address;  /* UDP server socket data */
    char *server_name;
    char *username;
    char tcp_message_buffer[MAX_TCP_MSG_SIZE + 1];
    char udp_message_buffer[MAX_UDP_MSG_SIZE + 1];
    pthread_t send_thread_id;
    pthread_t recv_thread_id;


public:

    /* Constructor */
    PeerClient(char*, int, char*);

    /* Initializes messaging buffers and socket addresses */
    int init();

    /* TCP register request message */
    int send_register_message();

    /* TCP connect to peer request message */
    int send_peer_connection_request();

    /* UDP identification first message */
    int send_udp_first_msg();

    /* starts p2p send/recv threads */
    int start_peer_communication();

    /* messages receive thread */
    static void *run_p2p_recv(void*);

    /* messages send thread */
    static void *run_p2p_send(void*);

};

