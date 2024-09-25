#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "netSocket.h"

#define BUFFER_SIZE    100
#define NAME_SIZE   20

void* sendMessage(void* arg);
void* recvMessage(void* arg);

char name[NAME_SIZE] = "[ANONYM]";
char message[BUFFER_SIZE];

int main(int argc, char** argv)
{
    pthread_t send_thread;
    pthread_t recv_thread;
    void* thread_return;

    std::string IP;
    std::string _name;
    int port;

    std::cout << "Input server IP : ";
    std::cin >> IP;
    std::cout << "Input port : ";
    std::cin >> port;
    std::cout << "Input name : ";
    std::cin >> _name;

    sprintf(name, "[%s]", _name.c_str());
	
	NetSocket sock(IP, port, 0);
	
	sock.InitializeSocket(TCP);
	sock.SetServerAddress(CLIENT);

	sock.ConnectToServer();

	pthread_create(&send_thread, NULL, sendMessage, (void*)&sock);
	pthread_create(&recv_thread, NULL, recvMessage, (void*)&sock);
	pthread_join(send_thread, &thread_return);
	pthread_join(recv_thread, &thread_return);
	

    return 0;
}

void* sendMessage(void* arg)
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUFFER_SIZE + 3];

    while(true)
    {
        fgets(message, BUFFER_SIZE, stdin);

        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s : %s", name, message);
        write(sock, name_msg, strlen(name_msg));
    }

    return NULL;
}

void* recvMessage(void* arg)
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUFFER_SIZE + 3];
    int str_len;

    while(true)
    {
        str_len = read(sock, name_msg, NAME_SIZE + BUFFER_SIZE + 2);
        if(str_len == -1)
        {
            return (void*) - 1;
        }
        name_msg[str_len] = '\0';
        fputs(name_msg, stdout);
    }

    return NULL;
}



