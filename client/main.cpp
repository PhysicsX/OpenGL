#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include<unistd.h>

#include <iostream>

using namespace std;

#define DATA "we are the best..."

#define NAME "../abc"

int main(void)
{
     int sock;
     struct sockaddr_un server;
     char buf[1024];

     sock = socket(AF_UNIX, SOCK_STREAM, 0);
     if (sock < 0) {
         perror("opening stream socket");
         exit(1);
     }
     server.sun_family = AF_UNIX;
     strcpy(server.sun_path, NAME);

     if (connect(sock, (struct sockaddr *) &server, sizeof(struct
sockaddr_un)) < 0) {
         close(sock);
         perror("connecting stream socket");
         exit(1);
     }
     if (write(sock, DATA, sizeof(DATA)) < 0)
         perror("writing on stream socket");
     close(sock);

     return 0;
}
