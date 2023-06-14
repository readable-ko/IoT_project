# 23-1 IoT Final Project [![license](https://img.shields.io/badge/Handong-IoT%20class-yellow)](LICENSE)
## Play 1 on 1 Soccer game with your terminal
## 더 글로리조, 1 ON 1 Soccer version 1.0
This repository is made by
* HGU 17 [Ko Yeong Kwang](https://github.com/readable-ko)
* HGU 18 [Jo Ha Seong]()

This powerpoint is created for the explain. [Google Slide](https://docs.google.com/presentation/d/1V1Mb-gftc5J1BWbL1Qt95pkN7dhGs9hrGvpWuY_q3NE/edit?usp=sharing).

This repository shows how to set up sever, client and how to execute.


1. [Setting Up Environment](#setting-up-environment)
   * [Using c++11](#using-c11)
   * [Using Ncurses library](#using-ncurses-library)
   * [Using boost library](#using-boost-library)
2. [Execute](#execute)
   * [Run on Window / Ubuntu](#run-on-window--ubuntu)
   * [Run on Mac OS](#run-on-mac-os)


# Setting Up Environment
## Using c++11
#### terminal
```bash
# install g++ if you don't
sudo apt-get install g++

#check installed c++ version
g++ --version
```

## Download Github File
Download `all the git files on this repo`

## Using Ncurses library
#### Window/Ubuntu
```bash
# install ncurses for get an input without enter.
sudo apt-get install libncurses5-dev libncursesw5-dev
```

#### MacOS
```bash
# install ncurses for get an input without enter.
brew install ncurses
```

## Using boost library
#### Window/Ubuntu
[you can also download on here](https://www.boost.org/users/download/)
```bash
# install ncurses for get an input without enter.
sudo apt-get install libboost-all-dev
```

#### MacOS
```bash
# install boost for get an input without enter.
brew install boost
```

# Execute
We already included ap_protocol.hpp header file so, you need to give.

## Run on Window / Ubuntu
### Run Server
```bash
#Make a execute file for server
g++ server.cpp -o ['execute file name'] app_protocol.hpp -lpthread -lboost_system -lncurses -std=c++11
```
```bash
#Give any port number you wanna use
./['execute file name'] ['port number']
```

### Run Client
```bash
#Make a execute file for client
g++ client.cpp -o ['execute file name'] app_protocol.hpp -lpthread -lboost_system -lncurses -std=c++11
```
```bash
# Run demo client with server's ip address and port number you selected.
./['execute file name'] ['IPv4 address'] ['port number']
```

it may possible to use IPv6 but we did not check it. Recommend to use IPv4.

## Run on Mac OS
Similar to above window method.

### Run Server
```bash
#Make a execute file for server
g++ server.cpp -o ['execute file name'] app_protocol.hpp -lpthread -lncurses -I ['your boost system include location'] -lboost_system -L ['your boost system lib location'] -std=c++11

#example
g++ server.cpp -o ['execute file name'] app_protocol.hpp -lpthread -lncurses -I /opt/homebrew/Cellar/boost/1.81.0_1/include -lboost_system -L /opt/homebrew/Cellar/boost/1.81.0_1/lib -std=c++11
```

```bash
#Give any port number you wanna use
./['execute file name'] ['port number']
```

### Run Client
```bash
#Make a execute file for client
g++ client.cpp -o ['execute file name'] app_protocol.hpp -lpthread -lncurses -I ['your boost system include location'] -lboost_system -L ['your boost system lib location'] -std=c++11

#example
g++ client.cpp -o ['execute file name'] app_protocol.hpp -lpthread -lncurses -I /opt/homebrew/Cellar/boost/1.81.0_1/include -lboost_system -L /opt/homebrew/Cellar/boost/1.81.0_1/lib -std=c++11
```
```bash
# Run demo client with server's ip address and port number you selected.
./['execute file name'] ['IPv4 address'] ['port number']
```

# TODO
* [ ] Apply On WebAssembly
* [ ] Split int bit to use less resources
* [ ] Improve network delay differences between users
* [ ] Add obstruction and ball holding capabilities
* [ ] Add items such as an increase in the number of openings

# References

  Boost: boost async chatting program server, client [example](https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/examples/cpp11_examples.html)
  * [SERVER](https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/example/cpp11/chat/chat_server.cpp)
  * [CLIENT](https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/example/cpp11/chat/chat_client.cpp)
  
  Ncurses: I refered how to use ncurses examples
  * [NCURSES](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html)
  
  Our project is inspired by these previous Midterm Exam in IoT laboratories class in HGU.(Prof. Y. M. KO)
