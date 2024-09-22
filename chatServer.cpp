// chat server
// it receives messages from clients and show up messages to all clients via tcp/ip.
// written by H.H. Yoo

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <set>
#include "netSocket.h"

#define BUFFER_SIZE	128
//#define MAX_CLIENTS	256

using namespace std;

void* clientProcessing(void* arg);
void sendMessage(char* message, int len);

int client_count = 0;
//int clnt_socks[MAX_CLIENTS];
set<int> client_socks;
pthread_mutex_t mTex;

int main(int argc, char** argv)
{
	int port;

	std::cout << "Input port number : ";
	std::cin >> port;

	NetSocket servSock(" ", port, 5);
	pthread_t t_id;

	pthread_mutex_init(&mTex, NULL);	// initialize a mutex object
	servSock.InitializeSocket(TCP);
	servSock.SetServerAddress(SERVER);
	servSock.BindAddressToSocket();
	
	servSock.WaitConnectionRequest();

	while(true)
	{
		servSock.AcceptIncoming();

		pthread_mutex_lock(&mTex);
		//client_socks[client_count++] = servSock.GetClientSocket();
		client_socks.insert(servSock.GetClientSocket());
		client_count = client_socks.size();
		
		pthread_mutex_unlock(&mTex);

		pthread_create(&t_id, NULL, clientProcessing, (void*)servSock.GetClientSocketP());
		pthread_detach(t_id);
		printf("Connected client IP : %s \n", inet_ntoa(servSock.GetClientAddress().sin_addr));
	}

	return 0;
}

void* clientProcessing(void* arg)
{
	int client_sock = *((int*)arg);
	int str_len = 0;
	int i;
	char msg[BUFFER_SIZE];

	while((str_len = read(client_sock, msg, sizeof(msg))) != 0)
	{
		sendMessage(msg, str_len);
	}

	pthread_mutex_lock(&mTex);
	auto iter = client_socks.find(client_sock);
	client_socks.erase(iter);
	client_count = client_socks.size();
	pthread_mutex_unlock(&mTex);

	return NULL;
}

void sendMessage(char* message, int len)
{
	int i;
	pthread_mutex_lock(&mTex);
	for(auto iter = client_socks.begin(); iter != client_socks.end(); ++iter)
	{
		write(*iter, message, len);
	}
	pthread_mutex_unlock(&mTex);
}
