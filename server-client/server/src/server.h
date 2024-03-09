#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <future>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <mutex> 
#include <condition_variable>

#define TRUE   1
#define PORT 8080
#define BUFFERLEN 1 * 1024

namespace Socket
{
    class Server
    {
    public:
        Server();
        ~Server();
    private:
    std::thread send_th; 
    std::condition_variable cv;
    std::mutex mtx;

    bool running = false;
    sockaddr_in server_addr;
    sockaddr_in client_address; 
	int client_address_size = sizeof(client_address);
    int server_socket, new_client;
    int status;
    int byte_received;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    char buffer[BUFFERLEN];
    std::string message;

    void bind_addr();
    void send_message(void);
    void listen_socket(void);
    void accept_client(void);
    void accept_connection(void);
    void print_client_info(void);
    void receive_message(void);
    };
    
} // namespace Socket
#endif // SERVER_H
