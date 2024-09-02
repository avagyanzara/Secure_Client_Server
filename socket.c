#include "socket.h"

bool is_valid_socket(const Socket socket)
{
    return socket >= 0;
}

void close_socket(const Socket socket)
{
    close(socket);
}

void configure_address(struct addrinfo **bind_address, const char *ip_address, const char *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int return_code = getaddrinfo(ip_address, port, &hints, bind_address);
    if (return_code != 0) {
        exit_with_error_message("Failed to get address info", GET_ADDRESS_INFO_FAILURE_CODE);
    }
}

Socket create_server_socket(struct addrinfo *bind_address)
{
    HARD_ASSERT(bind_address != NULL, "Bind address is null.");
    Socket new_socket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);

    if (!is_valid_socket(new_socket)) {
        exit_with_error_message("Failed to create socket", errno);
    }

    if (bind(new_socket, bind_address->ai_addr, bind_address->ai_addrlen)) {
        exit_with_error_message("Failed to bind socket", errno);
    }

    freeaddrinfo(bind_address);
    return new_socket;
}

Socket create_client_socket(struct addrinfo *peer_address, const int connection_attempt_count, const int attempt_interval)
{
    HARD_ASSERT(peer_address != NULL, "Peer address is null.");
    Socket new_socket = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    if (!is_valid_socket(new_socket)) {
        exit_with_error_message("Failed to create socket", errno);
    }

    int return_code = 0;
    for (int i = 0; i < connection_attempt_count; ++i) {
        return_code = connect(new_socket, peer_address->ai_addr, peer_address->ai_addrlen);
        if (return_code == 0) {
            break;
        }
        printf("Failed to connect, attempt: %d\n", i + 1);
        sleep(attempt_interval);
    }

    freeaddrinfo(peer_address);
   
    if (return_code != 0) {
        exit_with_error_message("Failed to connect to server", SOCKET_RECEIVE_FAILURE_CODE);
    }

    return new_socket;
}

void listen_socket(const Socket server_socket)
{
    if (listen(server_socket, MAX_CONNECTION_COUNT) < 0) {
        exit_with_error_message("Failed to listen to specified port", SOCKET_LISTEN_FAILURE_CODE);
    }
}

Socket accept_client(struct sockaddr_storage *client_address, const Socket server_socket)
{
    socklen_t client_len = sizeof(*client_address);
    Socket client_socket = accept(server_socket, (struct sockaddr*)client_address, &client_len);    
    return client_socket;
}

char *receive_message(const Socket socket, int *size)
{
    int total_bytes = 0;
    int bytes_count = recv(socket, &total_bytes, sizeof(total_bytes), DUMMY_FLAGS);
    if (bytes_count == 0) {
        return NULL;
    }
    if (size != NULL) {
        *size = total_bytes;
    }
    if (bytes_count < 0) {
        exit_with_error_message("Failed to receive", SOCKET_RECEIVE_FAILURE_CODE);
    }
    char *message = (char*)malloc(total_bytes + 1);
    if (message == NULL) {
        exit_with_error_message("Failed to allocate memory", MALLOC_FAILURE_CODE);
    }

    char *buffer = message;
    int bytes_read = 0;
    while (bytes_read < total_bytes) {
        bytes_count = recv(socket, buffer, total_bytes - bytes_read, DUMMY_FLAGS);
        if (bytes_count <= 0) {
            exit_with_error_message("Failed to receive", SOCKET_RECEIVE_FAILURE_CODE);
        }
        if (bytes_count == 0) {
            free(message);
            return NULL;
        }
        buffer += bytes_count;
        bytes_read += bytes_count;
    }
    HARD_ASSERT(bytes_read == total_bytes, "Received data is corrupted");
    message[total_bytes] = '\0';
    return message;
}

int send_message(const Socket socket, const char *message, const int size)
{
    const int total_bytes = size;
    const int bytes_count = send(socket, &total_bytes, sizeof(total_bytes), DUMMY_FLAGS);
    if (bytes_count == 0) {
        return 0;
    }
    if (bytes_count < 0) {
        exit_with_error_message("Failed to send", SOCKET_SEND_FAILURE_CODE);
    }

    int bytes_sent = 0;
    const char *buffer = message;

    while (bytes_sent < total_bytes) {
        const int bytes_count = send(socket, buffer, total_bytes - bytes_sent, DUMMY_FLAGS);
        if (bytes_count == 0) {
            return 0;
        }
        if (bytes_count < 0) {
            exit_with_error_message("Failed to send", SOCKET_SEND_FAILURE_CODE);
        }
        buffer += bytes_count;
        bytes_sent += bytes_count;
    }
    HARD_ASSERT(bytes_sent == total_bytes, "Sent data is corrupted");
    return 1;
}

