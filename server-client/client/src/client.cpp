#include "client.h"
#include <iostream>

namespace Socket
{
    Client::Client(/* args */)
    {
        try
        {
            std::cout << "Create socket ..." << std::endl;
            connection = socket(AF_INET, SOCK_STREAM, 0);
            if (connection < 0)
            {
                throw std::runtime_error("Could not create socket.");
            }
            std::cout << "socket created." << std::endl;
            set_server_info();
            connect_to_server();

            running = true;
            send_th = std::thread(&Client::send_message, this);
            receive_message();

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    Client::~Client()
    {
        running = false;
        std::cout << "socket destructor ..." << std::endl;
        running = false;
        if (send_th.joinable())
            send_th.join();
        close(connection);
        std::cout << "socket destructor done!" << std::endl;
    }

    void Client::set_server_info()
    {
        try
        {
            server_info.sin_family = AF_INET;
            server_info.sin_port = htons(SERVER_PORT);
            set_ip();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void Client::set_ip()
    {
        if (inet_pton(AF_INET, SERVER_IP, &server_info.sin_addr) <= 0)
        {
            throw std::exception();
        }
    }

    void Client::connect_to_server()
    {
        try
        {
            std::cout << "connecting to server ..." << std::endl;
            while (true)
            {
                connection_result = connect(connection, (sockaddr*)&server_info, sizeof(server_info));
                if (connection_result < 0)
                {
                    throw std::exception();
                }else
                {
                    break;
                }

                usleep(2 * 1000);
            }
            std::cout << "connected to server." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Can not Connect to server ... !" << '\n';
        }
    }

    void Client::send_message()
    {
        while (running)
        {
            try
            {
                std::cout << "You: ";
                getline(std::cin, message);
                if (!message.empty())
                {
                    if (send(connection, message.c_str(), message.length() + 1, 0) < 0)
                    {
                        throw std::runtime_error("send failed !");
                    }
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void Client::receive_message()
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (cv.wait_for(lck, std::chrono::milliseconds(10)) == std::cv_status::timeout)
        {
            byte_received = recv(connection, buffer, BUFFERLEN, 0);
            if (byte_received > 0)
            {
                std::cout << "\nserver: " << std::string(buffer, byte_received) << "\nYou: " << std::flush;
            }
            memset(buffer, 0, BUFFERLEN);
        }
    }

} // namespace Socket
