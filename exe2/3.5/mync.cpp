#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

// Open a TCP server on the specified port and accept a single connection
void start_tcp_server(int port, int *fd)
{
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Initialize serv_addr structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Bind the socket to the specified port
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    std::cout << "Waiting for client connection on port: " << port << std::endl;

    // Listen for incoming connections
    listen(sockfd, 5);

    // Accept a connection
    *fd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (*fd < 0)
    {
        perror("accept");
        close(sockfd);
        exit(1);
    }

    std::cout << "The connection was established successfully" << std::endl;

    // Close the listening socket (not needed after accepting a connection)
    close(sockfd);
}

// Function to start a TCP client
void start_tcp_client(const char *hostname, int port, int *fd)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Create a socket
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Get server information by hostname
    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; // Set address family to IPv4

    // Copy the server's IP address to the server address structure
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port); // Convert port to network byte order and set it

    // Attempt to connect to the server
    if (connect(*fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect");
        close(*fd);
        exit(1);
    }

    std::cout << "The connection was established successfully" << std::endl;
}

// Function to transfer data from input_fd to output_fd
void transfer_data(int input_fd, int output_fd)
{
    char buffer[1024];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(input_fd, buffer, sizeof(buffer) - 1)) > 0)
    {
        // Write the buffer to the output_fd
        bytes_written = write(output_fd, buffer, bytes_read);
        if (bytes_written < 0)
        {
            perror("write");
            break;
        }
    }

    if (bytes_read < 0)
    {
        perror("read");
    }
}

//Handle input/output redirection when the -e parameter is present

void handle_redirection_e(int argc, char *argv[], const char *local_host, int &input_fd, int &output_fd, int &in_and_out_fd)
{
    // Handle input redirection

    if (strncmp(argv[3], "-i", 2) == 0)
    {
        if (argc == 5)
        {
            if (strncmp(argv[4], "TCPS", 4) == 0)
            {
                start_tcp_server(atoi(argv[4] + 4), &input_fd);
            }
            else if (strncmp(argv[4], "TCPC", 4) == 0)
            {
                start_tcp_client(local_host, atoi(argv[4] + 14), &input_fd);
            }

            // if argc>3
        }
        else
        {
            if (strncmp(argv[4], "TCPS", 4) == 0)
            {
                start_tcp_server(atoi(argv[4] + 4), &input_fd);
            }
            else if (strncmp(argv[4], "TCPC", 4) == 0)
            {
                start_tcp_client(local_host, atoi(argv[4] + 14), &input_fd);
            }

            if (strncmp(argv[5], "-o", 2) == 0)
            {
                if (strncmp(argv[6], "TCPC", 4) == 0)
                {
                    start_tcp_client(local_host, atoi(argv[6] + 14), &output_fd);
                }
                else if (strncmp(argv[6], "TCPS", 4) == 0)
                {
                    start_tcp_server(atoi(argv[6] + 4), &output_fd);
                }
            }
        }
    }
    else if (strncmp(argv[3], "-o", 2) == 0)
    {
        if (strncmp(argv[4], "TCPS", 4) == 0)
        {
            start_tcp_server(atoi(argv[4] + 4), &output_fd);
        }
        else if (strncmp(argv[4], "TCPC", 4) == 0)
        {
            start_tcp_client(local_host, atoi(argv[4] + 14), &output_fd);
        }
    }
    else if (strncmp(argv[3], "-b", 2) == 0)
    {
        if (strncmp(argv[4], "TCPS", 4) == 0)
        {
            start_tcp_server(atoi(argv[4] + 4), &in_and_out_fd);
        }
        else if (strncmp(argv[4], "TCPC", 4) == 0)
        {
            start_tcp_client(local_host, atoi(argv[4] + 14), &in_and_out_fd);
        }
    }
}

//Handle input/output redirection when the -e parameter is not present

void handle_redirection(int argc, char *argv[], const char *local_host, int &input_fd, int &output_fd, int &in_and_out_fd)
{
    // Handle input redirection

    if (strncmp(argv[1], "-i", 2) == 0)
    {
        if (argc == 3)
        {
            if (strncmp(argv[2], "TCPS", 4) == 0)
            {
                start_tcp_server(atoi(argv[2] + 4), &input_fd);
            }
            else if (strncmp(argv[2], "TCPC", 4) == 0)
            {
                start_tcp_client(local_host, atoi(argv[2] + 14), &input_fd);
            }

            // if argc>3
        }
        else
        {
            if (strncmp(argv[2], "TCPS", 4) == 0)
            {
                start_tcp_server(atoi(argv[2] + 4), &input_fd);
            }
            else if (strncmp(argv[2], "TCPC", 4) == 0)
            {
                start_tcp_client(local_host, atoi(argv[2] + 14), &input_fd);
            }

            if (strncmp(argv[3], "-o", 2) == 0)
            {
                if (strncmp(argv[4], "TCPC", 4) == 0)
                {
                    start_tcp_client(local_host, atoi(argv[4] + 14), &output_fd);
                }
                else if (strncmp(argv[4], "TCPS", 4) == 0)
                {
                    start_tcp_server(atoi(argv[4] + 4), &output_fd);
                }
            }
        }
    }
    else if (strncmp(argv[1], "-o", 2) == 0)
    {
        if (strncmp(argv[2], "TCPS", 4) == 0)
        {
            start_tcp_server(atoi(argv[2] + 4), &output_fd);
        }
        else if (strncmp(argv[2], "TCPC", 4) == 0)
        {
            start_tcp_client(local_host, atoi(argv[2] + 14), &output_fd);
        }
    }
    else if (strncmp(argv[1], "-b", 2) == 0)
    {
        if (strncmp(argv[2], "TCPS", 4) == 0)
        {
            start_tcp_server(atoi(argv[2] + 4), &in_and_out_fd);
        }
        else if (strncmp(argv[2], "TCPC", 4) == 0)
        {
            start_tcp_client(local_host, atoi(argv[2] + 14), &in_and_out_fd);
        }
    }
}

int main(int argc, char *argv[])
{
    const char *local_host = "127.0.0.1"; // Localhost IP address
    int input_fd = -1, output_fd = -1, in_and_out_fd = -1;

    if (argc < 2)
    {
        printf("Usage: %s [-e command] [-i|-o|-b <TCPS|TCPC><port>]\n", argv[0]);
        return 1;
    }
    // Check if the first argument is "-e"
    bool execute_program = strcmp(argv[1], "-e") == 0;
    if (execute_program && argc < 3)
    {
        printf("Please provide the program name and its arguments after -e\n");
        return 1;
    }

    if (execute_program)
    {
        // Split the second argument into program name and arguments
        char *program = strtok(argv[2], " ");
        char *arguments = strtok(NULL, "");
        char *new_argv[] = {program, arguments, NULL};

        // Handle input/output redirection
        handle_redirection_e(argc, argv, local_host, input_fd, output_fd, in_and_out_fd);

        // Fork the process to create a child process
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            return 1;
        }
        else if (pid == 0)
        {
            // Child process
            // Redirect standard input if input_fd is set
            if (input_fd != -1)
            {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            // Redirect standard output if output_fd is set
            if (output_fd != -1)
            {
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }
            
            // Redirect both standard input and output if in_and_out_fd is set
            if (in_and_out_fd != -1)
            {
                dup2(in_and_out_fd, STDIN_FILENO);
                dup2(in_and_out_fd, STDOUT_FILENO);
                close(in_and_out_fd);
            }

            execvp(program, new_argv);

            // If execvp returns, there was an error
            perror("execvp");
            return 1;
        }
        else
        {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    }
    else
    {
        // Handle redirection
        handle_redirection(argc, argv, local_host, input_fd, output_fd, in_and_out_fd);

        if (argc == 3)
        {

            if (input_fd != -1)
            {
                transfer_data(input_fd, STDOUT_FILENO);
                close(input_fd);
            }

            if (output_fd != -1)
            {
                transfer_data(STDIN_FILENO, output_fd);
                close(output_fd);
            }
            if (in_and_out_fd != -1)
            {
                transfer_data(in_and_out_fd, in_and_out_fd);
                close(in_and_out_fd);
            }
        }

        else
        {

            transfer_data(input_fd, output_fd);
        }
    }

    return 0;
}
