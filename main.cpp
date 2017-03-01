#include <iostream>
#include <time.h>

#include "server/communicationserver.h"

using namespace std;

int main()
{

    char *server_name = "127.0.0.1";
    int port_number = 4444;


    CommunicationServer *comm_Server = new CommunicationServer(server_name, port_number);
    int ret = comm_Server->init();
    if (ret)
    {
        printf(" * Server Crashed\n * Exit Code is: %d\n", ret);
    }

    return 0;
}
