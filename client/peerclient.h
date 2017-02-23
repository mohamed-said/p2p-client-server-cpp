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

    int tcp_socket_fd, udp_socket_fd;
    int port_number;
    hostent *server;
    sockaddr_in server_socket_address;
    char *server_name;
    char *username;
    char tcp_message_buffer[MAX_TCP_MSG_SIZE + 1];
    char udp_message_buffer[MAX_UDP_MSG_SIZE + 1];

public:

    PeerClient(string&, int);
    int init();
    int send_register_message();
    int send_peer_connection_request(string &p_username);
    int send_udp_first_msg();
    int start_peer_communication();

};

