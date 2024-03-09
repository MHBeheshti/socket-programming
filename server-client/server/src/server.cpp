#include "server.h"
#include <iostream>

namespace Socket
{
Server::Server()
{
    try
    {
        std::cout << "creating socket ..." << std::endl;

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        bind_addr();
        listen_socket();
        std::cout << "socket created!" << std::endl;
        accept_client();
    }
    catch(const std::exception& e)
    {
        std::cerr << "socket failed :" << e.what() << std::endl;
        exit(EXIT_FAILURE);  
    }
    running = true;
    send_th = std::thread(&Server::send_message, this);
    receive_message();
}

Server::~Server()
{
    std::cout << "socket destructor ..." << std::endl;
    running = false;
    if (send_th.joinable())
        send_th.join();
    close(server_socket); 
    std::cout << "socket destructor done!" << std::endl;
}

void Server::bind_addr(){
    try
    {
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        {
            throw std::exception();
        }

    }
    catch(const std::exception& e)
    {
        std::cerr << "failed bind : " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::listen_socket(void)
{
    try
    {
        status = listen(server_socket, SOMAXCONN);
        if (status < 0)
        {
            throw std::exception();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "listen ABORT !" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::accept_client(void)
{
    try
    {
        std::cout << "wait for connection ..." << std::endl;
        accept_connection();
        print_client_info();
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR on accept: " << e.what() << std::endl;
    }
}

void Server::accept_connection(void)
{
    new_client = accept(server_socket, (sockaddr *)&client_address, (socklen_t *)&client_address_size);
    if (new_client < 0)
    {
        throw std::exception();
    }
}

void Server::print_client_info(void)
{
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
    if (getnameinfo((sockaddr *)&client_address, sizeof(client_address), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        std::cout << host << " Connected on port " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client_address.sin_addr, host, NI_MAXHOST);
        std::cout << host << " Connected on port " << ntohs(client_address.sin_port) << std::endl;
    }
}

void Server::receive_message()
{
    std::unique_lock<std::mutex> lck(mtx);
    while (cv.wait_for(lck, std::chrono::milliseconds(10)) == std::cv_status::timeout)
    {
        try
        {
            memset(buffer, 0, BUFFERLEN);
            byte_received = recv(new_client, buffer, sizeof(buffer), 0);
            if (byte_received < 0)
            {
                std::cout << "ERROR in receive" << std::endl;
                close(new_client);
                return;
            }
            if (byte_received == 0)
            {
                std::cout << "client disconnected." << std::endl;
                close(new_client);
                return;
            }

            std::cout << std::endl << host << "; " << service << ": " << buffer << "\nYou: " << std::flush;
        }
        catch (const std::exception &e)
        {
            std::cerr << "ERROR in message processing: " << e.what() << std::endl;
        }
    }
}

void Server::send_message()
{
    while (running)
    {
        try
        {
            std::cout << "You: ";
            getline(std::cin, message);
            send(new_client, message.c_str(), message.length() + 1, 0);
            cv.notify_one();
        }
        catch (const std::exception &e)
        {
            std::cerr << "ERROR in write message: " << e.what() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
}
