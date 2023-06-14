#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <fstream>
#include <boost/asio.hpp>
#include "app_protocol.hpp"
#include <random>

using boost::asio::ip::tcp;
using namespace std;

typedef std::deque<app_message> app_message_queue;

class chat_client
{
public:
    chat_client(boost::asio::io_context &io_context,
                const tcp::resolver::results_type &endpoints)
        : io_context_(io_context),
          socket_(io_context)
    {
        do_connect(endpoints);
    }

    void write(app_message &msg, char* input)
    {
        if (player_num == 0)
        {
            msg.set_x1(atoi(input));
            msg.set_y1(atoi(input));
            msg.set_num(0);
        }
        else
        {
            msg.set_x2(atoi(input));
            msg.set_y2(atoi(input));
            msg.set_num(1);
        }
        boost::asio::post(io_context_,
                          [this, msg]()
                          {
                              bool write_in_progress = !write_msgs_.empty();
                              write_msgs_.push_back(msg);
                              if (!write_in_progress)
                              {
                                  do_write();
                              }
                          });
    }
    void close()
    {
        boost::asio::post(io_context_, [this]()
                          { socket_.close(); });
    }

private:
    void do_connect(const tcp::resolver::results_type &endpoints)
    {
        boost::asio::async_connect(socket_, endpoints,
                                   [this](boost::system::error_code ec, tcp::endpoint)
                                   {
                                       if (!ec)
                                       {
                                           printf("Connected to the Server\n");
                                           //    uint32_t sent_bytes = 0;
                                           //    std::random_device rd;
                                           //    std::mt19937 gen(rd());
                                           //    std::uniform_int_distribution<> dis(30000, 50000);
                                           //    port_receiver = dis(gen);
                                           //    sent_bytes = boost::asio::write(socket_,
                                           // 								   boost::asio::buffer(&port_receiver, sizeof(port_receiver)));
                                           //    std::unique_lock<std::mutex> lock(mutex_);
                                           //    input_received = true;
                                           //    input_cv.notify_one();
                                           do_read();
                                       }
                                       else
                                       {
                                           socket_.close();
                                       }
                                   });
    }

    void do_read()
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(&read_msg, sizeof(read_msg)), // read from read_msg_body only till the length identified in the header
                                [this](boost::system::error_code ec, std::size_t length)
                                {
                                    if (!ec)
                                    {
                                        std::cout << "Message From the server: " << length << endl;
                                        printf("Receiving Image: %d %d %d", read_msg.get_x1(), read_msg.get_x2(), read_msg.get_num());
                                        player_num = read_msg.get_num();
                                        std::cout << "\n";
                                        do_read(); // Continue reading the next message
                                    }
                                    else
                                    {
                                        socket_.close();
                                    }
                                });
    }
    void do_write()
    {
        boost::asio::async_write(socket_,
                                 boost::asio::buffer(&write_msgs_.front(), // write the queue message to the server
                                                     sizeof(write_msgs_.front())),
                                 [this](boost::system::error_code ec, std::size_t length)
                                 {
                                     if (!ec)
                                     {
                                         printf("Client writing a message to the server %lu\n", length);
                                         //  printf("Sending Image: %d %d\n",  write_msgs_.front().get_x1(), write_msgs_.front().get_y1());
                                         write_msgs_.pop_front();  // pop the message
                                         if (!write_msgs_.empty()) // check if other messages are to be sent
                                         {
                                             do_write();
                                         }
                                     }
                                     else
                                     {
                                         socket_.close();
                                     }
                                 });
    }

private:
    boost::asio::io_context &io_context_;
    tcp::socket socket_;
    app_message read_msg;
    app_message_queue write_msgs_;
    uint8_t player_num;
};

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: chat_client <host> <port>";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(argv[1], argv[2]);
        chat_client c(io_context, endpoints);

        std::thread t([&io_context]()
                      { io_context.run(); });

        char line[app_message::msg_length];
        while (std::cin.getline(line, 8))
        {
            if (strcmp(line, "q") == 0)
                break;
            app_message msg;
            c.write(msg, line);
        }

        c.close();
        t.join();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}