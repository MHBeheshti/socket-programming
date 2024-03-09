#ifndef CLIENT_H 
#define CLIENT_H

#include <netdb.h>
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
#include <mutex> 
#include <condition_variable>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFERLEN 1 * 1024

namespace Socket
{
    class Client
    {
    public:
        Client();
        ~Client();
    private:
        std::thread send_th;
        std::condition_variable cv;
        std::mutex mtx;

        bool running = false;
        int connection;
        int connection_result;
        int byte_received;
        sockaddr_in server_info;
        char buffer[BUFFERLEN];
        std::string message;

        void set_server_info(void);
        void connect_to_server(void);
        void send_message(void);
        void receive_message(void);
        void set_ip(void);
    };

} // namespace Socket
#endif // CLIENT_H