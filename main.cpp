
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE sizeof(struct sockaddr_in)

//Check verifica daca a castigat vreun jucator sau nu
//Daca a castigat, se vor trimite semnalele aferente
void check(char playBoard[][3]);
//Contine doar un print
void catcher(int sig);
//sirurile astea de jos sunt pipe-urile pentru a comunica intre procese
int newsockfd[2];
int pid[2];

int main()
{
    int sockfd[2];
    char serverRead[1];
    char ans[1];
    int usr = 0;
    int ctr = 1;
    int row = 0;
    int column = 0;
    int choice = 0;
    char x[4];
    //tabloul asta tine minte mutarile de la ambii jucatori
    char a[2][40];

    //tabla
    char playBoard[3][3] = {
            {' ', ' ', ' '},
            {' ', ' ', ' '},
            {' ', ' ', ' '}};

    //se populeaza structura cu date campul server
    struct sockaddr_in server = {AF_INET, 8001, INADDR_ANY};
    strcpy(ans, "");
    static struct sigaction act, act2;
    //cred ca liniile astea de mai jos care incep cu sig mascheaza niste semnale ca sa nu cumva sa
    //cauzeze terminarea programului in mod anormal
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, 0);
    sigaction(SIGTSTP, &act, 0);
    sigaction(SIGINT, &act, 0);

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

    printf("Waiting for Players to join in TicTacToe Club.. \n");

    strcpy(a[0], "Waiting for the other Player to join\n");

    //se va iesi din while abia dupa ce se vor autentifica doi jucatori
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

    //din moment ce e nevoie de doi jucatori, cu fork, se vor crea doua procese
    //Unul pentru primul jucator si unul pentru al doilea
    //primul proces va fi cel parinte
    if (fork() == 0)
    {
        int count = 0;

        //In while-ul asta se va alege unde sa se puna x si 0
        while (count == 0)
        {

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

        close(newsockfd[0]);
        exit(0);
    }
    int status;
    wait(&status);
    close(newsockfd[1]);
    //Ultimele linii sunt facute pentru a iesi din executie
}

void check(char playBoard[][3])
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

    //se verifica si se trimite semnalul SIGUSR1 daca a castigat primul jucator. Cel de al doilea program
    //va fi receptiv la acel semnal si va stii ca a castigat primul jucator
    if (key == 'X')
    {

        printf("Player 1 Wins\n\n");
        kill(pid[0], SIGUSR1);
        kill(pid[1], SIGUSR1);
        exit(0);
    }

    //Acelasi lucru dar pentru al doilea jucator si se va trimite SIGUSR2
    //Astea sunt semnale predefinite care nu inseamna in sine un lucru anume
    //Se pot folosi in mod liber
    if (key == 'O')
    {

        printf("Player 2 Wins\n\n");
        kill(pid[0], SIGUSR2);
        kill(pid[1], SIGUSR2);
        exit(0);
    }
}

void catcher(int sig)
{
    printf("Quitting\n");
}