#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <omp.h>

#include "names.h"
#include "database.h"

void server_functs(struct mydb_t *db, char c_type, char **arg, int sockfd, struct sockaddr_un *addr, socklen_t addr_len)
{
    switch (c_type) {
        case C_PUT:
            {
                char res = mydb_put(db, arg[0], arg[1]) ? 0 : 1;
                sendto(sockfd, &res, 1, 0, (struct sockaddr *) addr, addr_len);
                break;
            }
        case C_GET:
            {
                const char *val = mydb_get(db, arg[0]);
                int count = !val ? 0 : 1;

                sendto(sockfd, &count, sizeof(int), 0,
                        (struct sockaddr *) addr, addr_len);

                if (count) {
                    int buf_size = strlen(val) + 1;

                    sendto(sockfd, &buf_size, sizeof(int), 0,
                            (struct sockaddr *) addr, addr_len);

                    sendto(sockfd, val, buf_size, 0,
                            (struct sockaddr *) addr, addr_len);
                }

                break;
            }
        case C_LIST:
            {
                size_t count;
                const char **list = mydb_list(db, &count);
                sendto(sockfd, &count, sizeof(int), 0,
                        (struct sockaddr *) addr, addr_len);

                for (size_t i = 0; i < count; i++) {
                    int buf_size = strlen(list[i]) + 1;

                    sendto(sockfd, &buf_size, sizeof(int), 0,
                            (struct sockaddr *) addr, addr_len);

                    sendto(sockfd, list[i], buf_size, 0,
                            (struct sockaddr *) addr, addr_len);
                }
                free(list);
                break;
            }
        case C_ERACE:
            {
                char res = mydb_erase(db, arg[0]) ? 0 : 1;
                sendto(sockfd, &res, 1, 0, (struct sockaddr *) addr, addr_len);
                break;
            }
        case C_EXIT:
            {
                mydb_close(db);
                char res = 1;
                sendto(sockfd, &res, 1, 0, (struct sockaddr *) addr, addr_len);
                exit(EXIT_SUCCESS);
            }
    }
}

int create_soket(int thr)
{
    struct sockaddr_un sock_addr;
    int sockfd;
    socklen_t sockaddr_len = sizeof(sock_addr);

    sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed!\n");
        return -1;
    }

    sock_addr.sun_family = AF_UNIX;
    sprintf(sock_addr.sun_path, "%s%d", SERVER_SOCKET_FILE, thr);
    unlink(sock_addr.sun_path);
    if (bind(sockfd, (struct sockaddr *) &sock_addr,
                sizeof(sock_addr)) < 0) {
        perror("Socket binding failed!\n");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int main(int argc, const char *argv[])
{
    /* Database init */
    struct mydb_t *db = mydb_init(FILE_DATA, FILE_MDATA, 1);
    if (!db) {
        printf("DB not created\n");
        return 1;
    }


#pragma omp parallel num_threads(SERVER_NUM_THREADS)
{
    int thr = omp_get_thread_num();

    /* Server init */
    int sockfd, recv_len;
    if ((sockfd = create_soket(thr)) < 0) {
        exit(EXIT_FAILURE);
    }

    int i = 0;

    struct sockaddr_un from_addr;
    socklen_t sockaddr_len = sizeof(struct sockaddr_un);
    char c_type, ibuf[20], *arg[2];
    int arg_len[2];

    for(;;) {
        recv_len = recvfrom(sockfd, ibuf, 20, 0,
                    (struct sockaddr *) &from_addr, &sockaddr_len);

        c_type = (unsigned char) ibuf[0];
        arg_len[0] = *((int*)&ibuf[1]);
        arg_len[1] = *((int*)&ibuf[1 + sizeof(int)]);

        printf("%d] type: %d, len1 %d, len2 %d\n", thr, (int) c_type, arg_len[0], arg_len[1]);

        for (i = 0; i < 2 && arg_len[i] > 0; i++) {
            arg[i] = malloc(arg_len[i]);
            recv_len = recvfrom(sockfd, arg[i], arg_len[i], 0,
                        (struct sockaddr *) &from_addr, &sockaddr_len);
            printf("%d] arg%d \"%s\"\n", thr, i, arg[i]);
        }


        server_functs(db, c_type, arg, sockfd,
                        &from_addr, sockaddr_len);

        printf("%d-----------------------\n",thr);
        fflush(stdout);
    }

    if (sockfd >= 0) {
        close(sockfd);
    }
}


    return EXIT_SUCCESS;
}
