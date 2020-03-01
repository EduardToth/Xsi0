//
// Created by eduard on 01.03.2020.
//

#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void Server::execute() {
    struct sockaddr_in server = {AF_INET, 8001, INADDR_ANY};
    set_signals();
    set_socket(server);

    printf("Waiting for Players to join in TicTacToe Club.. \n");
    strcpy(a[0], "Waiting for the other Player to join\n");

    wait_for_players();
    listen_to_client_calls();
    close(newsockfd[0]);
    close(newsockfd[1]);
}

void Server::set_signals(){
static struct sigaction act;

act.sa_handler = catcher;
sigfillset(&(act.sa_mask));
sigaction(SIGPIPE, &act, 0);
sigaction(SIGTSTP, &act, 0);
sigaction(SIGINT, &act, 0);
}

void Server::set_socket(struct sockaddr_in & server) {
    if ((sockfd[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket Call Failed");
        exit(1);
    }

    if (bind(sockfd[0], (struct sockaddr *)&server, SIZE) == -1)
    {
        perror("Bind Call Failed");
        exit(1);
    }

}

void Server::catcher(int sig)
{
    printf("Quitting\n");
}

void Server::check(char playBoard[][3])
{
    int i;
    char key = ' ';

    //prin for-urile astea doua se va verifica daca a castigat vreun jucator
    for (i = 0; i < 3; i++)
        if (playBoard[i][0] == playBoard[i][1] && playBoard[i][0] == playBoard[i][2] && playBoard[i][0] != ' ')
            key = playBoard[i][0];
    for (i = 0; i < 3; i++)
        if (playBoard[0][i] == playBoard[1][i] && playBoard[0][i] == playBoard[2][i] && playBoard[0][i] != ' ')
            key = playBoard[0][i];
    if (playBoard[0][0] == playBoard[1][1] && playBoard[1][1] == playBoard[2][2] && playBoard[1][1] != ' ')
        key = playBoard[1][1];
    if (playBoard[0][2] == playBoard[1][1] && playBoard[1][1] == playBoard[2][0] && playBoard[1][1] != ' ')
        key = playBoard[1][1];


    if (key == 'X')
    {

        printf("Player 1 Wins\n\n");
        kill(pid[0], SIGUSR1);
        kill(pid[1], SIGUSR1);
        running = false;
    }

    if (key == 'O')
    {

        printf("Player 2 Wins\n\n");
        kill(pid[0], SIGUSR2);
        kill(pid[1], SIGUSR2);
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

        read(newsockfd[ctr], serverRead, sizeof(serverRead));
        choice = atoi(serverRead);
        printf("Server side the Integer received is: %d\n", choice);
        row = --choice / 3;
        column = choice % 3;
        playBoard[row][column] = (ctr == 0) ? 'X' : 'O';
        if (ctr == 1)
            ctr = 0;
        else
            ctr = 1;

        write(newsockfd[ctr], playBoard, sizeof(playBoard));
        check(playBoard);
    }
}

void Server::wait_for_players() {
    int usr = 0;
    char x[4];
    while (usr < 2)
    {
        if (listen(sockfd[0], 5) == -1)
        {
            perror("Listen Call Failed\n");
            exit(1);
        }

        newsockfd[usr] = accept(sockfd[0], 0, 0);
        usr++;

        if (usr == 1)
        {
            strcpy(a[1], "0");
            write(newsockfd[0], a, sizeof(a));
            read(newsockfd[0], x, sizeof(x));
            pid[0] = atoi(x);
        }

        printf("No. of Players who joined the Club: %d\n", usr);

        if (usr == 2)
        {
            strcpy(a[0], "Let's play Tic Tac Toe!!");
            strcpy(a[1], "1");
            write(newsockfd[0], a, sizeof(a));
            strcpy(a[1], "2");
            write(newsockfd[1], a, sizeof(a));
            read(newsockfd[1], x, sizeof(x));
            pid[1] = atoi(x);
        }
    }
}
