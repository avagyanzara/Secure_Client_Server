#include "utility.h"
#include "socket.h"
#include "encryption.h"

#define ARGUMENT_COUNT 5
#define HELP_ARGUMENT_COUNT 2
#define FIVE_MINUTE 300

void handle_client_requests(const Socket socket, fd_set *main_set, fd_set *read_set, const char *encrypted_message, const int size);
void accept_connections(const Socket server_socket, const char *encrypted_message, const int size);

int main(int argc, char *argv[])
{
    if (HELP_ARGUMENT_COUNT == argc && strcmp(argv[HELP_ARGUMENT_COUNT - 1], "-h") == 0) {
        printf("Usage: %s -r [receiving port] -m [reply message]\n", argv[0]);
        return 0;
    }

    if (argc != ARGUMENT_COUNT) {
        char buffer[LOCAL_BUFFER_SIZE];
        sprintf(buffer, "Wrong number of arguments.\nUsage: %s -r [receiving port] -m [reply message]", argv[0]);
        exit_with_error_message(buffer, INVALID_ARGUMENT_CODE);
    }

    char *port = (char*)NULL;
    char *message = (char*)NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 >= argc) {
                exit_with_error_message("Incomplete port argument", INVALID_ARGUMENT_CODE);
            }
            port = copy_string(argv[++i]);
        } else if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                exit_with_error_message("Incomplete port argument", INVALID_ARGUMENT_CODE);
            }
            message = copy_string(argv[++i]);
        } else {
            exit_with_error_message("Invalid argument specified", INVALID_ARGUMENT_CODE);
        }
    }

    const int size = strlen(message);
    char *encrypted_message = encrypt_message(message, size);

    struct addrinfo *bind_address;
    configure_address(&bind_address, 0, port);
    Socket server_socket = create_server_socket(bind_address);

    printf("[%s] Start server on port %s.\n", timestamp(), port);
    listen_socket(server_socket);

    printf("[%s] Waiting for connections.\n", timestamp());
    accept_connections(server_socket, encrypted_message, size);
    close_socket(server_socket);
    free(message);
    free(port);
   
    return 0;
}

void handle_client_requests(const Socket socket, fd_set *main_set, fd_set *read_set, const char *encrypted_message, const int size)
{
    int received_size = 0;
    char *received_message = receive_message(socket, &received_size);
    if (received_message == NULL) {
        printf("[%s] Closing connection...\n", timestamp());
        close_socket(socket);
        FD_CLR(socket, main_set);
        FD_CLR(socket, read_set);
        return;
    }

    char *client_message = decrypt_message(received_message, received_size);
    if (strcmp(client_message, "Exit server") == 0) {
        printf("[%s] Closing connection...\n", timestamp());
        close_socket(socket);
        FD_CLR(socket, main_set);
        FD_CLR(socket, read_set);
        return;
    }

    printf("[%s] Received message: %s\n", timestamp(), client_message);
    free(client_message);

    if (0 == send_message(socket, encrypted_message, size)) {
        printf("[%s] Closing connection...\n", timestamp());
    }
    close_socket(socket); // TODO, handle errors
    FD_CLR(socket, main_set);
    FD_CLR(socket, read_set);
}

void accept_connections(const Socket server_socket, const char *encrypted_message, const int size)
{
    fd_set main_set;
    FD_ZERO(&main_set);
    FD_SET(server_socket, &main_set);

    Socket max_socket = server_socket;
    struct timeval timeout;
    timeout.tv_sec = FIVE_MINUTE;
    timeout.tv_usec = DUMMY_FLAGS;

    while (true) {
        fd_set read_set = main_set;
        const int return_code = select(max_socket + 1, &read_set, 0, 0, &timeout);
        if (return_code == 0) {
            printf("[%s] Server waiting timeout expired.\n", timestamp());
            break;
        }
        if (return_code < 0) {
            exit_with_error_message("Failed to accept connections", CONNECTION_ACCEPT_FAILURE_CODE);
        }
        for (Socket socket = 1; socket <= max_socket; ++socket) {
            if (!FD_ISSET(socket, &read_set)) {
                continue;
            }

            // socket is the server socket listening to connections
            if (socket == server_socket) {
                struct sockaddr_storage client_address;
                const Socket client_socket = accept_client(&client_address, server_socket);

                if (!is_valid_socket(client_socket)) {
                    exit_with_error_message("Failed to accept connection", CONNECTION_ACCEPT_FAILURE_CODE);
                    continue;
                }

                FD_SET(client_socket, &main_set);
                if (client_socket > max_socket) {
                    max_socket = client_socket;
                }
            } else { // socket is created to communicate with client socket
                handle_client_requests(socket, &main_set, &read_set, encrypted_message, size);
            }
        }
    }
}

