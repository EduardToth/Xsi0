//
// Created by eduard on 01.03.2020.
//

#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;


void Server::execute() {
    struct sockaddr_in server = {AF_INET, PORT, INADDR_ANY};
    set_signals();
    set_socket(server);

    printf("Waiting for Players to join\n");
    strcpy(message_for_client[0], "Waiting for the other Player to join\n");

    wait_for_players();
    listen_to_client_calls();
    close(client_socket_pids[0]);
    close(client_socket_pids[1]);
}

void Server::set_signals(){
    struct sigaction action;
    action.sa_handler = execute_catch_block;
    sigfillset(&(action.sa_mask));
    sigaction(SIGPIPE, &action, 0);
    sigaction(SIGTSTP, &action, 0);
    sigaction(SIGINT, &action, 0);
}

void Server::set_socket(struct sockaddr_in & server) {
    if ((server_socket_fd[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket Call Failed");
        exit(1);
    }

    if (bind(server_socket_fd[0], (struct sockaddr *)&server, SIZE) == -1)
    {
        perror("Bind Call Failed");
        exit(1);
    }

}

void Server::execute_catch_block(int sig) {
    printf("Quitting\n");
}

char Server::identify_winner(char playBoard[][3]) {
    char key = ' ';

    for (int i = 0; i < 3; i++)
        if (playBoard[i][0] == playBoard[i][1] && playBoard[i][0] == playBoard[i][2] && playBoard[i][0] != ' ')
            key = playBoard[i][0];
    for (int i = 0; i < 3; i++)
        if (playBoard[0][i] == playBoard[1][i] && playBoard[0][i] == playBoard[2][i] && playBoard[0][i] != ' ')
            key = playBoard[0][i];
    if (playBoard[0][0] == playBoard[1][1] && playBoard[1][1] == playBoard[2][2] && playBoard[1][1] != ' ')
        key = playBoard[1][1];
    if (playBoard[0][2] == playBoard[1][1] && playBoard[1][1] == playBoard[2][0] && playBoard[1][1] != ' ')
        key = playBoard[1][1];

    return key;
}

void Server::someone_won(char playBoard[][3]) {

    char key = identify_winner(playBoard);
    if (key == 'X')
    {

        printf("Player 1 Wins\n\n");
        kill(client_pids[0], SIGUSR1);
        kill(client_pids[1], SIGUSR1);
        running = false;
    }
    if (key == 'O')
    {

        printf("Player 2 Wins\n\n");
        kill(client_pids[0], SIGUSR2);
        kill(client_pids[1], SIGUSR2);
        running = false;
    }
}

void Server::listen_to_client_calls() {
    int count = 0;
    int ctr = 1;
    int row = 0;
    int column = 0;
    int choice = 0;
    char serverRead[1];
    char playBoard[3][3] = {
            {' ', ' ', ' '},
            {' ', ' ', ' '},
            {' ', ' ', ' '}};
    while (count == 0)
    {
        if(!running) {
            break;
        }

        read(client_socket_pids[ctr], serverRead, sizeof(serverRead));
        choice = atoi(serverRead);
        printf("Server side the Integer received is: %d\n", choice);
        row = --choice / 3;
        column = choice % 3;
        playBoard[row][column] = (ctr == 0) ? 'X' : 'O';
        if (ctr == 1)
            ctr = 0;
        else
            ctr = 1;

        write(client_socket_pids[ctr], playBoard, sizeof(playBoard));
        someone_won(playBoard);
    }
}

void Server::wait_for_players() {
    int nr_users = 0;

    while (nr_users < 2)
    {
        if (listen(server_socket_fd[0], 5) == -1)
        {
            perror("Listen Call Failed\n");
            exit(1);
        }

        client_socket_pids[nr_users++] = accept(server_socket_fd[0], 0, 0);

        if (nr_users == 1)
        {
            join_first_player();
        }

        printf("No. of Players who joined the Club: %d\n", nr_users);

        if (nr_users == 2)
        {
            join_second_player();
        }
    }
}

void Server::join_second_player(){
    char pid_in_string_form[4];
    strcpy(message_for_client[0], "Game starts!!!");
    strcpy(message_for_client[1], "1");
    write(client_socket_pids[0], message_for_client, sizeof(message_for_client));
    strcpy(message_for_client[1], "2");
    write(client_socket_pids[1], message_for_client, sizeof(message_for_client));
    read(client_socket_pids[1], pid_in_string_form, sizeof(pid_in_string_form));
    client_pids[1] = atoi(pid_in_string_form);
}

void Server::join_first_player() {
    char pid_in_string_form[4];
    strcpy(message_for_client[1], "0");
    write(client_socket_pids[0], message_for_client, sizeof(message_for_client));
    read(client_socket_pids[0], pid_in_string_form, sizeof(pid_in_string_form));
    client_pids[0] = atoi(pid_in_string_form);
}
