#include <iostream>
#include <time.h>

#include "server/communicationserver.h"

using namespace std;

int main()
{

    char *server_name = "127.0.0.1";
    int tcp_port_number = 4444;
    int udp_port_number = 5555;

    CommunicationServer *comm_Server = new CommunicationServer(server_name, tcp_port_number, udp_port_number);
    int ret = comm_Server->init();
    if (ret)
    {
        printf(" * Server Crashed\n * Exit Code is: %d\n", ret);
    }

    return 0;
}
