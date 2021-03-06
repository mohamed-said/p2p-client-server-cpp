#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <string.h>

#include "encryption_methods.h"

#define MAX_TCP_MSG_SIZE 1400
#define MAX_UDP_MSG_SIZE 64
#define NO_SUCH_HOST_ERROR 1000
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


class PeerClient {

private:

    int tcp_socket_fd;                      /* TCP socket descriptor */
    int udp_socket_fd;                      /* UDP socket descriptor */

    int16_t tcp_port_number;                /* Server TCP port number to connect to */
    int16_t udp_port_number;                /* Server UDP port number to send data on */

    hostent *server;                        /* Server address */

    sockaddr_in tcp_server_socket_address;  /* TCP server socket data */
    sockaddr_in udp_server_socket_address;  /* UDP server socket data */

    sockaddr_in peer_udp_socket_data;

    int socket_length;

    char *server_name;
    char *my_username;
    char peer_username[21];

    char tcp_message_buffer[MAX_TCP_MSG_SIZE + 1];
    char udp_message_buffer[MAX_UDP_MSG_SIZE + 1];

    pthread_t send_thread_id;
    pthread_t recv_thread_id;

    encryption_methods *crypto;

public:

    /* Constructor */
    PeerClient(char*, int16_t, int16_t);

    /* Destructor */
    ~PeerClient();

    /* Initializes messaging buffers and socket addresses */
    int init();

    /* TCP register request message */
    int send_register_message(char[]);

    /* TCP connect to peer request message */
    int send_peer_connection_request();

    /* UDP identification first message */
    int send_udp_first_msg();

    /* starts p2p send/recv threads */
    int start_peer_communication();

    /* messages receive thread */
    static void *run_p2p_recv(PeerClient *__peer_client);

    /* messages send thread */
    static void *run_p2p_send(PeerClient *__peer_client);

    pthread_t get_send_thred_id();
    pthread_t get_recv_thread_id();

};

