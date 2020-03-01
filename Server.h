//
// Created by eduard on 01.03.2020.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#include <netinet/in.h>


class Server{
private:
    const int SIZE = sizeof(struct sockaddr_in);
    char a[2][40];
    int sockfd[2];
    bool running = true;
    int newsockfd[2];
    int pid[2];
    void wait_for_players();
    void listen_to_client_calls();
    void check(char playBoard[][3]);
    static void catcher(int sig);
    void set_socket(struct sockaddr_in & server);
    static void set_signals();
public:
    void execute();
};

#endif //SERVER_SERVER_H
