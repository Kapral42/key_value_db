#pragma once

/* Client/Server */
#define SERVER_SOCKET_FILE "./tmp/.server.socket"
#define CLIENT_SOCKET_FILE "./tmp/.client.socket"

/* Client operations */
#define C_LIST  0
#define C_PUT   1
#define C_GET   2
#define C_ERACE 3

/* DB */
#define FILE_DATA "./data.mydb"
#define FILE_MDATA "./mdata.mydb"
#define MAX_DELETED_ELEM 10
#define HASHTAB_SIZE 100
#define SERVER_NUM_THREADS 4
