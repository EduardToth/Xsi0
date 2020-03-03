//
// Created by eduard on 03.03.2020.
//

#include "Client.h"

int Client::win_handler = 0;

void Client::playerWinhandler(int signum) {
    if (signum == SIGUSR1)
    {
        win_handler=1;
    }

    if (signum == SIGUSR2)
    {
        win_handler=2;
    }
}

void Client::set_signals() {
    static struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigfillset(&(act.sa_mask));
    sigaction(SIGTSTP, &act, 0);

    signal(SIGUSR1,playerWinhandler);
    signal(SIGUSR2,playerWinhandler);
}

void Client::set_socket(struct sockaddr_in & server) {
    server.sin_addr.s_addr = inet_addr(IP_ADDR);
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket Call Failed");
        exit(1);
    }

    if (connect (socket_descriptor, (struct sockaddr *)&server, SIZE) == -1)
    {
        perror("Connect Call Failed");
        exit(1);
    }
}

void Client::establish_connection_with_server() {
    if(strcmp(a[1],"0")==0)
    {
        int num1 = getpid();
        sprintf(pid,"%d",num1);
        write(socket_descriptor, pid, sizeof(pid));
        read(socket_descriptor, a, sizeof(a));
        printf("%s\n",a[0]);

    }

    if(strcmp(a[1],"2")==0)
    {
        int num2 = getpid();
        sprintf(pid,"%d",num2);
        write(socket_descriptor, pid, sizeof(pid));
    }
}

void Client::make_first_move() {

    int row,column,choice;
    if (strcmp(a[1],"1")!=0)
    {


        mapBoard(numberBoard);
        printf("\n    Number Board\n\n");
        for(;;)
        {
            printf("\nPlayer %d,Please enter the number of the square where you want to place your '%c': \n",(strcmp(a[1], "1")==0)?1:2,(strcmp(a[1], "1")==0)?'X':'O');
            scanf("%s",clientWrite);

            choice = atoi(clientWrite);
            row = --choice/3;
            column = choice%3;

            if(choice<0 || choice>9 || playBoard [row][column]>'9'|| playBoard [row][column]=='X' || playBoard [row][column]=='O')
                printf("Invalid Input. Please Enter again.\n\n");

            else
            {
                playBoard[row][column] = (strcmp(a[1], "1")==0)?'X':'O';
                break;
            }
        }

        write(socket_descriptor, clientWrite, sizeof(clientWrite));
        system("clear");
        mapBoard(playBoard);
        printf("\nCurrent Play Board\n\n");
    }
}

void Client::find_out_the_winner_if_exists() {
    if (win_handler == 1)
    {
        printf("Player 1 Wins!!\n");
        exit(0);
    }
    if (win_handler == 2)
    {
        printf("Player 2 Wins!!\n");
        exit(0);
    }
}

void Client::choose_position() {
    int row,column,choice;
    while(true)
    {
        if (win_handler == 0)
        {
            printf("\nPlayer %d, Now your turn .. Please enter the number of the square where you want to place your '%c': \n",(strcmp(a[1], "1")==0)?1:2,(strcmp(a[1], "1")==0)?'X':'O');
            scanf("%s",clientWrite);

            choice = atoi(clientWrite);
            row = --choice/3;
            column = choice%3;

            if(choice<0 || choice>9 || playBoard [row][column]>'9'|| playBoard [row][column]=='X' || playBoard [row][column]=='O')
                printf("Invalid Input. Please Enter again.\n\n");

            else
            {
                playBoard[row][column] = (strcmp(a[1], "1")==0)?'X':'O';
                break;
            }
        }
    }
}

int Client::play()
{
    struct sockaddr_in server = {AF_INET, PORT};

    set_signals();
    set_socket(server);

    read(socket_descriptor, a, sizeof(a));
    printf("%s\n",a[0]);

    establish_connection_with_server();
    make_first_move();
    interact_with_the_server();
}

void Client::mapBoard(char playBoard[][3])
{
    printf(" _________________\n");
    printf("|     |     |     | \n");
    printf("|  %c  |  %c  |  %c  |\n",playBoard[0][0],playBoard[0][1],playBoard[0][2]);
    printf("|_____|_____|_____|\n");
    printf("|     |     |     |\n");
    printf("|  %c  |  %c  |  %c  |\n",playBoard[1][0],playBoard[1][1],playBoard[1][2]);
    printf("|_____|_____|_____|\n");
    printf("|     |     |     |\n");
    printf("|  %c  |  %c  |  %c  |\n",playBoard[2][0],playBoard[2][1],playBoard[2][2]);
    printf("|_____|_____|_____|\n");

}

void Client::interact_with_the_server() {
    char clientRead[3][3];
    for(char input = 'x';;)
    {
        if (input == '\n')
        {	if (win_handler == 0)
            {
                mapBoard(numberBoard);
                printf("\n    Number Board\n\n");
            }
            choose_position();

            write(socket_descriptor, clientWrite, sizeof(clientWrite));
            system("clear");
            mapBoard(playBoard);
            printf("\nCurrent Play Board\n\n");
            find_out_the_winner_if_exists();
        }

        if (read(socket_descriptor, clientRead, sizeof(clientRead)) > 0)
        {
            system("clear");
            memcpy(playBoard, clientRead, sizeof(playBoard));
            mapBoard(playBoard);
            printf("\nCurrent Play Board\n\n");
            input = '\n';
            find_out_the_winner_if_exists();
        }
        else
        {
            printf("You Win!! Thank You, Please Play Again :D\n");
            close(socket_descriptor);
            exit(1);
        }

    }
}

int Client::menu()
{
    int reply;

    printf("--------------------------------------------------------------------------\n");
    printf("			    Play Grid Menu  				  \n ");
    printf("--------------------------------------------------------------------------\n");


    printf("Enter 1 to Play.\n\n");
    printf("Enter 2 to Quit.\n\n");


    scanf("%d", &reply);

    return reply;
}

void Client::start_game()
{
    while(true)
    {
        switch(menu())
        {
            case 1:
            {
                printf("\nYou selected Play Option\n\n");
                play();
                break;
            }
            case 2:
            {
                printf("\nYou are quitting\n\n");
                exit(0);
                break;
            }
            default:
            {
                printf("\nInvalid menu choice\n\n");
                break;
            }
        }
    }
}