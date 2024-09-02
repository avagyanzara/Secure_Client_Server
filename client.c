#include "utility.h"
#include "socket.h"
#include "encryption.h"

#define ATTEMPT_COUNT 10
#define ATTEMPT_TIMEOUT 1 // 1s
#define RECEIVE_TIMEOUT 5 // 5s

#define ARGUMENT_COUNT 7
#define HELP_ARGUMENT_COUNT 2

void send_hallo_message(Socket socket, char *message);

int main(int argc, char *argv[])
{
    if (HELP_ARGUMENT_COUNT == argc && strcmp(argv[HELP_ARGUMENT_COUNT - 1], "-h") == 0) {
        printf("Usage: %s -i [server ip] -p [server port] -m [hallo message]\n", argv[0]);
        return 0;
    }

    if (argc != ARGUMENT_COUNT) {
        char buffer[LOCAL_BUFFER_SIZE];
        sprintf(buffer, "Wrong number of arguments.\nUsage: %s -i [server ip] -p [server port] -m [hallo message]\n", argv[0]);
        exit_with_error_message(buffer, INVALID_ARGUMENT_CODE);
    }

    char *ip = (char*)NULL;
    char *port = (char*)NULL;
    char *message = (char*)NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 >= argc) {
                exit_with_error_message("Incomplete server ip argument", INVALID_ARGUMENT_CODE);
            }
            ip = copy_string(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc) {
                exit_with_error_message("Incomplete server port argument", INVALID_ARGUMENT_CODE);
            }
            port = copy_string(argv[++i]);
        } else if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                exit_with_error_message("Incomplete hallo message", INVALID_ARGUMENT_CODE);
            }
            message = copy_string(argv[++i]);
        } else {
            exit_with_error_message("Invalid argument specified", INVALID_ARGUMENT_CODE);
        }
    }

    struct addrinfo *peer_address;
    configure_address(&peer_address, ip, port);
    Socket socket = create_client_socket(peer_address, ATTEMPT_COUNT, ATTEMPT_TIMEOUT);
    send_hallo_message(socket, message);

    close_socket(socket);
    free(port);
    free(ip);
    free(message);
   
    return 0;
}

void send_hallo_message(Socket socket, char *message)
{
    printf("[%s] Send hallo message: %s\n", timestamp(), message);
    const int size = strlen(message);
    char* encrypted_message1 = encrypt_message(message, size);

    if (0 == send_message(socket, encrypted_message1, size)) {
        printf("[%s] Connection closed.\n", timestamp());
        return;
    }

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(socket, &read_set);

    struct timeval timeout;
    timeout.tv_sec = RECEIVE_TIMEOUT;
    timeout.tv_usec = DUMMY_FLAGS;
    const int return_code = select(socket + 1, &read_set, 0, 0, &timeout);
    if (return_code == 0) {
        exit_with_error_message("Failed to receive value - time expired", SOCKET_RECEIVE_FAILURE_CODE);
    }
    if (return_code < 0) {
        exit_with_error_message("Failed to receive value", SOCKET_RECEIVE_FAILURE_CODE);
    }

    int message_size = 0;
    char *encrypted_message = receive_message(socket, &message_size);
    if (encrypted_message == NULL) {
        printf("[%s] Connection closed.\n", timestamp());
        return;
    }
    const char *received_message = decrypt_message(encrypted_message, message_size);
    printf("[%s] Received message: %s\n", timestamp(), received_message);
    free(encrypted_message);
}

