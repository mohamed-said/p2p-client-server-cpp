#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "peerclient.h"

using namespace std;

int main()
{

  char* server_name = "127.0.0.1";
  int16_t server_tcp_port_number = 4444;
  int16_t server_udp_port_number = 5555;

  puts("Please Enter a username (max 20 chars) : ");
  char *username = "kogo";
  
  /*
  short chr_count = scanf("%s", username);
  while (chr_count > 20)
  {
      puts("Please Enter a valid username (max 20 chars) : ");
      memset(username, 0, 20);
      chr_count = scanf("%s", username);
  }
  */

  
  PeerClient *peerclient = new PeerClient(server_name, server_tcp_port_number, server_udp_port_number, username);

  int ret_init = peerclient->init();
  if (ret_init)
  {
      fprintf(stderr, " * Failed to init buffers and socket data\n * Exit code is: %d\n", ret_init);
  }
  else
  {
      puts("Peer buffers initialized successfully");
  }


  int ret_reg_msg = peerclient->send_register_message();
  if (ret_reg_msg)
  {
    printf(" * Failed to send message \n * Exit code is: %d\n", ret_reg_msg);
  }
  else
  {
      puts("Register messgae sent successfully");
  }

  int ret_frst_udp = peerclient->send_udp_first_msg();
  if (ret_frst_udp)
  {
      printf(" * Failed to send first UDP messgae\n * Exit code is: %d\n", ret_frst_udp);
  }
  else
  {
      puts("First UDP message sent successfully");
  }

  return 0;
}
