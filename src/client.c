#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "names.h"

int connect_to_server(int nserv, char *client_socket_f)
{
    int sockfd;
    struct sockaddr_un sock_addr;
    socklen_t sockaddr_len = sizeof(sock_addr);

    sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed!\n");
        return -1;
    }

    sock_addr.sun_family = AF_UNIX;
    sprintf(sock_addr.sun_path, "%s%d", CLIENT_SOCKET_FILE, getpid());
    unlink(sock_addr.sun_path);
    if (bind(sockfd, (struct sockaddr *) &sock_addr,
                sizeof(sock_addr)) < 0) {
        perror("Socket binding failed!\n");
        return -1;
    }

    if (client_socket_f)
        sprintf(client_socket_f, "%s", sock_addr.sun_path);

    /* Trying connect to any server socket */
    for (int i = 0; i < nserv; i++) {
        sprintf(sock_addr.sun_path, "%s%d", SERVER_SOCKET_FILE, i);
        if (connect(sockfd, (struct sockaddr *) &sock_addr, sockaddr_len) >= 0) {
            break;
        }
    }
    if (sockfd < 0) {
        perror("Connection to server failed!\n");
        return -1;
    }

    return sockfd;
}

void usage()
{
    printf("Usage: ./client COMMAND [COMMAND ARGS]\n");
    printf("COMMAND:\n\tlist\n\tput <key> <value>");
    printf("\n\tget <key>\n\terace <key>\n");
}

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        usage();
        return EXIT_FAILURE;
    }

    int n_arg = 0;
    unsigned char c_type;
    if (!strcmp(argv[1], "list")) {
        c_type = C_LIST;
    } else if (!strcmp(argv[1], "put")) {
        c_type = C_PUT;
        n_arg = 2;
    } else if (!strcmp(argv[1], "get")) {
        c_type = C_GET;
        n_arg = 1;
    } else if (!strcmp(argv[1], "erace")) {
        c_type = C_ERACE;
        n_arg = 1;
    } else {
        printf("Incorrect command!\n");
        usage();
        return EXIT_FAILURE;
    }
    if (argc < n_arg + 2) {
        printf("Incorrect args!\n");
        usage();
        return EXIT_FAILURE;
    }

    /* Init connection to server */
    int sockfd;
    char client_socket_f[108];
    sockfd = connect_to_server(SERVER_NUM_THREADS, client_socket_f);
    if (sockfd < 0)
        exit(EXIT_FAILURE);


    int ibuf_len = 1 + sizeof(int) * 2;
    int buf1_len = n_arg >= 1 ? strlen(argv[2]) + 1 : 0;
    int buf2_len = n_arg >= 2 ? strlen(argv[3]) + 1 : 0;

    unsigned char *ibuf = malloc(ibuf_len);
    ibuf[0] = c_type;
    memcpy(ibuf + 1, &buf1_len, sizeof(int));
    memcpy(ibuf + 1 + sizeof(int), &buf2_len, sizeof(int));

   // strcpy(buf, "Hello server!");
    send(sockfd, ibuf, ibuf_len, 0);

    if (buf1_len)
        send(sockfd, argv[2], buf1_len, 0);
    if (buf1_len)
        send(sockfd, argv[3], buf2_len, 0);
    //    perror("Send message failed!\n");

    char res = 0;
    recv(sockfd, &res, 1, 0);
    if (res == 1)
        printf("SUCCESS\n");
    else
        printf("FAILURE\n");



    if (sockfd >= 0) {
        close(sockfd);
    }

    unlink(client_socket_f);

    return EXIT_SUCCESS;
}
