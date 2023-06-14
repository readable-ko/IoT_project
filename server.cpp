
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "app_protocol.hpp"
#include <fstream>
#include <mutex>

using boost::asio::ip::address;
using boost::asio::ip::tcp;
using namespace std;
#define BUFSIZE 128
//----------------------------------------------------------------------

typedef std::deque<app_message> app_message_queue;

//----------------------------------------------------------------------

class player
{
public:
	virtual ~player() {}
	virtual void deliver(app_message &msg) = 0;
};

typedef std::shared_ptr<player> player_ptr;
//----------------------------------------------------------------------
std::mutex mutex_[10];
std::condition_variable cv[10];
std::vector<std::thread> threads;


class game_room
{
public:
	bool get_status() const
	{
		return status;
	}
	void set_room_num(uint8_t val)
	{
		room_num = val;
	}
	void join(player_ptr player)
	{
		printf("%d\n", room_num);
		players_.insert(player);
		if (get_participants_size() == 2)
		{
			set_status(true);
			// auto future = std::async(std::launch::async, &chat_room::make_image, this);
			threads.emplace_back(&game_room::make_image, this);
			threads[room_num-1].detach();
		}                                                                                                                      
	}
	void leave(player_ptr player)
	{
		cout << "Participant Leaving" << endl;
		cout << get_participants_size() << endl;
		players_.erase(player);
		set_status(false);
	}
	void make_image()
	{
		cout << "Making Image" << endl;
		while (1)
		{
			if(get_participants_size() < 2)
			{
				set_status(true);
				printf("Participant\n");
				break;
			}
			sleep(1);
			// msg.set_Ball_x(change_ball(msg.get_Ball_x()));
			// msg.set_Ball_y(change_ball(msg.get_Ball_y()));
			// msg.set_Ball_x(10);
			// msg.set_Ball_y(10);
			// msg.set_x1(10);
			// msg.set_y1(10);
			// msg.set_x2(10);
			// msg.set_y2(10);
			// msg.set_Score_x(10);
			// msg.set_Score_y(10);
			std::unique_lock<std::mutex> lk(mutex_[room_num]);
			cv[room_num].wait(lk, [this]
					{ return !from_player; });
			deliver(msg);
			lk.unlock();
		}
	}
	uint8_t change_ball(uint8_t prev)
	{
		uint8_t after = 0;
		after = prev + 1;
		return after;
	}
	void change_from_player(app_message &recv_msg)
	{
		cout << "Received" << endl;
		std::lock_guard<mutex> lock(mutex_[room_num]);
		from_player = true;
		if (recv_msg.get_num() == 0)
		{
			msg.set_x1(recv_msg.get_x1());
			msg.set_y1(recv_msg.get_y1());
		}
		else
		{
			msg.set_x2(recv_msg.get_x2());
			msg.set_y2(recv_msg.get_y2());
		}
		cv[room_num].notify_all();
		from_player = false;
	}
	void deliver(app_message &msg)
	{
		cout << get_participants_size() << endl;
		for (auto player : players_)
		{
			player->deliver(msg);
		}
	}
	int get_participants_size() const
	{
		return players_.size();
	}

private:
	std::set<player_ptr> players_;
	void set_status(bool value)
	{
		status = value;
	}
	bool status = false;
	app_message msg;
	bool from_player = false;
	uint8_t room_num;
	// std::future<void> future;
};

class chat_session
	: public player,
	  public std::enable_shared_from_this<chat_session>
{
public:
	chat_session(tcp::socket socket, std::shared_ptr<game_room> room, uint8_t player_num)
		: socket_(std::move(socket)),
		  room_(room),
		  player_num(player_num)
	{
	}

	void start()
	{
		room_->join(shared_from_this()); // Join a room
		do_read();
	}

	void deliver(app_message &msg)
	{
		bool write_in_progress = !write_msgs_.empty(); // if the queue is empty then progress variable is false
		msg.set_num(player_num);
		write_msgs_.push_back(msg);
		// if (!write_in_progress) //if progress variable is false
		// {
		do_send_image();
		// }
	}

private:
	void do_read()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
								boost::asio::buffer(&read_msg, sizeof(read_msg)),
								[this, self](boost::system::error_code ec, std::size_t /*length*/)
								{
									if (!ec)
									{
										room_->change_from_player(read_msg);
										do_read();
									}
									else
									{
										room_->leave(shared_from_this());
									}
								});
	}

	void do_send_image()
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_,
								 boost::asio::buffer(&write_msgs_.front(),
													 sizeof(write_msgs_.front())),
								 [this, self](boost::system::error_code ec, std::size_t /*length*/)
								 {
									 if (!ec)
									 {
										//  cout << "Send Image\n"
										// 	  << flush;
										 write_msgs_.pop_front();
										 if (!write_msgs_.empty())
										 {
											 do_send_image();
										 }
									 }
									 else
									 {
										 room_->leave(shared_from_this());
									 }
								 });
	}
	tcp::socket socket_;
	std::shared_ptr<game_room> room_;
	app_message read_msg;
	app_message send_msg;
	app_message_queue write_msgs_;
	uint8_t player_num;
};

class chat_server
{
public:
	chat_server(boost::asio::io_context &io_context,
				const tcp::endpoint &endpoint)
		: io_context(io_context),
		  acceptor_(io_context, endpoint)
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
			{
				if (!ec)
				{
					std::cout << "Connected\n" << flush;
					uint8_t player_num = 0;
					if (room_list.size() == 0)
					{
						auto new_room = std::make_shared<game_room>();
						new_room->set_room_num(room_num + 1);
						room_list.push_back(new_room);
					}
					else
					{
						int empty = 0;
						for (auto &room : room_list)
						{
							if (room->get_status() == false)
							{
								empty = 1;
								player_num += 1;
							}
						}
						if (empty == 0)
						{
							auto new_room = std::make_shared<game_room>();
							room_num += 1;
							new_room->set_room_num(room_num + 1);
							room_list.push_back(new_room);
						}
					}
					printf("%d\n", room_num);
					std::make_shared<chat_session>(std::move(socket), room_list[room_num], player_num)->start();
				}
				do_accept();
			});
	}
	const tcp::endpoint endpoint;
	std::vector<std::shared_ptr<game_room>> room_list;
	boost::asio::io_context &io_context;
	tcp::acceptor acceptor_;
	uint8_t room_num = 0;
};

//----------------------------------------------------------------------

int main(int argc, char *argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: chat_server <port> [<port> ...]\n";
			return 1;
		}

		boost::asio::io_context io_context;

		std::list<chat_server> servers;
		for (int i = 1; i < argc; ++i)
		{
			tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
			servers.emplace_back(io_context, endpoint);
		}

		io_context.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}