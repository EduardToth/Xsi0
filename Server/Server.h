//
// Created by eduard on 01.03.2020.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#include <netinet/in.h>
#define PORT 8080

class Server{
private:
    const int SIZE = sizeof(struct sockaddr_in);
    char message_for_client[2][40];
    int server_socket_fd[2];
    bool running = true;
    int client_socket_pids[2];
    int client_pids[2];
    void wait_for_players();
    void listen_to_client_calls();
    void someone_won(char playBoard[][3]);
    static void execute_catch_block(int sig);
    void set_socket(struct sockaddr_in & server);
    static void set_signals();
    char identify_winner(char playBoard[][3]);
    void join_first_player();
    void join_second_player();
public:
    void execute();
};

#endif //SERVER_SERVER_H
