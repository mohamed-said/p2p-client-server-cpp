#include <iostream>
#include <time.h>

#include "communicationserver.h"

using namespace std;

int main(int argc, char **argv)
{

    char *server_name = new char[50];
    int tcp_port_number = 4444;
    int udp_port_number = 5555;
  
    if (argc > 1)
    {
        strcpy(server_name, argv[1]);
    }
    
    CommunicationServer *comm_Server = new CommunicationServer(server_name, tcp_port_number, udp_port_number);
    int ret = comm_Server->init();
    if (ret)
    {
        printf(" * Server Crashed\n * Exit Code is: %d\n", ret);
    }

    puts("OK");

    string user = "kogo";
    PeerData *peer = comm_Server->get_peer_details(user);

    cout << "Peer Username: " << peer->username << endl;
    cout << "Peer Port: " << ntohs(peer->client_sockert_address.sin_port) << endl;
    cout << "Peer address: " << peer->client_sockert_address.sin_addr.s_addr << endl;

    return 0;
}
