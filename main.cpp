#include <iostream>
#include <time.h>

#include "server/communicationserver.h"

using namespace std;

int main()
{

    char *server_name = "127.0.0.1";
    int port_number = 4444;


    cout << sizeof(short) << " " << sizeof(int) << " " << sizeof(long) << endl;
    cout << ntohl(port_number) << endl;
    cout << ntohs(port_number) << endl;

    CommunicationServer *comm_Server = new CommunicationServer(server_name, port_number);
    int ret = comm_Server->init();
    if (ret)
    {
        printf(" * Failed to start the server\n * Exit Code is: %d\n", ret);
    }

    return 0;
}
