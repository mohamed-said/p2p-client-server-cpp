#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "peerclient.h"

using namespace std;

int main()
{

  char* server_name = "192.168.1.106";
  int port_number = 4444;

  PeerClient *peerclient = new PeerClient(server_name, port_number);
  int ret = peerclient->init();
  if (ret)
  {
    fprintf(stderr, " * Failed to init buffers and socket data\n");
    return 0;
  }
  int ret2 = peerclient->send_register_message();
  if (ret2)
  {
    printf(" * Failed to send message \n * Exit code is: %d\n", ret2);
    return 0;
  }


  return 0;
}
