#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "names.h"
#include "database.h"

int main(int argc, const char *argv[])
{
    struct sockaddr_un sock_addr, from_addr;
    char buff[100];
    int sockfd, recv_len;
    socklen_t sockaddr_len = sizeof(sock_addr);

    // TODO: change type of socket tot SOCK_STREAM
    sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed!\n");
        return EXIT_FAILURE;
    }

    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path, SERVER_SOCKET_FILE, 108);
    unlink(SERVER_SOCKET_FILE);
    if (bind(sockfd, (struct sockaddr *) &sock_addr,
                sizeof(sock_addr)) < 0) {
        perror("Socket binding failed!\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    while ((recv_len = recvfrom(sockfd, buff, 100, 0,
                    (struct sockaddr *) &from_addr, &sockaddr_len)) > 0) {
        printf("recvfrom: %s\n", buff);
    }

    /*int sockfd1 = accept(sockfd, (struct sockaddr *) &from_addr, &sockaddr_len);
    while ((recv_len = recv(sockfd1, buff, 100, 0)) > 0) {
        printf("recvfrom: %s\n", buff);
    }
    printf("recv_len: %d\n", recv_len);
*/


    //test hashtable
    
    //now we have the value and key
    

    if (sockfd >= 0) {
        close(sockfd);
    }

    return EXIT_SUCCESS;
}
