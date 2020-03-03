//
// Created by eduard on 03.03.2020.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
#include <arpa/inet.h>
#define PORT 8080

class Client {
private:
    const char * IP_ADDR = "127.0.0.1";
    const int SIZE = sizeof(struct sockaddr_in);
    int socket_descriptor;
    char a[2][40];
    char pid[4];
    char clientWrite[1];
    char numberBoard [3][3] = {
            {'1','2','3'},
            {'4','5','6'},
            {'7','8','9'}
    };
    char playBoard [3][3] =   {
            {' ',' ',' '},
            {' ',' ',' '},
            {' ',' ',' '}
    };
    static int win_handler;

    static void playerWinhandler(int signum);
    void set_signals();
    void set_socket(struct sockaddr_in & server);
    void establish_connection_with_server();
    void make_first_move();
    void find_out_the_winner_if_exists();
    void choose_position();
    int play();
    void mapBoard(char playBoard[][3]);
    void interact_with_the_server();
    int menu();

public:
    void start_game();
};



#endif //CLIENT_CLIENT_H
