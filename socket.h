#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "utility.h"

#define MAX_CONNECTION_COUNT 128

typedef int Socket;

bool is_valid_socket(const Socket socket);
void close_socket(const Socket socket);
void configure_address(struct addrinfo **bind_address, const char *ip_address, const char *port);
Socket create_server_socket(struct addrinfo *bind_address);
Socket create_client_socket(struct addrinfo *peer_address, const int connection_attempt_count, const int attempt_interval);
void listen_socket(const Socket server_socket);
Socket accept_client(struct sockaddr_storage *client_address, const Socket server_socket);

char *receive_message(const Socket socket, int *size);
int send_message(const Socket socket, const char *message, const int size);

#endif // __SOCKET_H__
