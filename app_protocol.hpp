#ifndef APP_MESSAGE_HPP
#define APP_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>

class app_message
{
public:
    static constexpr std::size_t msg_length = 8;
    app_message():
    Player1_x(0), Player1_y(0), Player2_x(0), Player2_y(0), Ball_x(0), Ball_y(0), Score_x(0), Score_y(0), player_num(0)
    {
    }
    uint8_t get_x1()
    {
        return Player1_x;
    }
    uint8_t get_y1()
    {
        return Player1_y;
    }
    uint8_t get_x2()
    {
        return Player2_x;
    }
    uint8_t get_y2()
    {
        return Player2_y;
    }
    uint8_t get_Ball_x()
    {
        return Ball_x;
    }
    uint8_t get_Ball_y()
    {
        return Ball_y;
    }
    uint8_t get_Score_x()
    {
        return Score_x;
    }
    uint8_t get_Score_y()
    {
        return Score_y;
    }
    uint8_t get_num()
    {
        return player_num;
    }
    void set_x1(uint8_t val)
    {
        Player1_x = val;
    }
    void set_y1(uint8_t val)
    {
        Player1_y = val;
    }
    void set_x2(uint8_t val)
    {
        Player2_x = val;
    }
    void set_y2(uint8_t val)
    {
        Player2_y = val;
    }
    void set_Ball_x(uint8_t val)
    {
        Ball_x = val;
    }
    void set_Ball_y(uint8_t val)
    {
        Ball_y = val;
    }
    void set_Score_x(uint8_t val)
    {
        Score_x = val;
    }
    void set_Score_y(uint8_t val)
    {
        Score_y = val;
    }
    void set_num(uint8_t val)
    {
        player_num = val;
    }

private:
    uint8_t Player1_x;
    uint8_t Player1_y;
    uint8_t Player2_x;
    uint8_t Player2_y;
    uint8_t Ball_x;
    uint8_t Ball_y;
    uint8_t Score_x;
    uint8_t Score_y;
    uint8_t player_num;
    char info[256];
};

#endif