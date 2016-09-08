#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "names.h"

int main(int argc, const char *argv[])
{
    struct sockaddr_un sock_addr;
    char buff[100];
    int sockfd, recv_len;
    socklen_t sockaddr_len = sizeof(sock_addr);

    sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed!\n");
        return EXIT_FAILURE;
    }

    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path, CLIENT_SOCKET_FILE, 108);
    unlink(CLIENT_SOCKET_FILE);
    if (bind(sockfd, (struct sockaddr *) &sock_addr,
                sizeof(sock_addr)) < 0) {
        perror("Socket binding failed!\n");
        return EXIT_FAILURE;
    }

    strncpy(sock_addr.sun_path, SERVER_SOCKET_FILE, 108);
    if (connect(sockfd, (struct sockaddr *) &sock_addr, sockaddr_len) < 0) {
        perror("Connection to server failed!\n");
        return EXIT_FAILURE;
    }

    strcpy(buff, "Hello server!");
    if (send(sockfd, buff, strlen(buff) + 1, 0) == -1) {
        perror("Send message failed!\n");
    }

    if (sockfd >= 0) {
        close(sockfd);
    }

    return EXIT_SUCCESS;
}
