#include <iostream>
#include <time.h>

#include "server/communicationserver.h"

using namespace std;

int main()
{

    char *server_name = "192.168.1.106";
    int port_number = 4444;
    CommunicationServer *comm_Server = new CommunicationServer(server_name, port_number);
    comm_Server->init();

    return 0;
}
