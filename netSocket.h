// network socket class for versatile usage
// written by H.H. Yoo

#ifndef __NET_SOCKET_H__

#define __NET_SOCKET_H__

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define TCP     0  
#define UDP     1

#define SERVER  0
#define CLIENT  1

void ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

class NetSocket
{
public:
    NetSocket();
    NetSocket(std::string _ip, int _pt, int _backlog);
    ~NetSocket();
    NetSocket(const NetSocket& rhs) = delete;
    NetSocket& operator=(const NetSocket& rhs) = delete;

public:
    void InitializeSocket(int flag);
    void SetServerAddress(int flag);
    void BindAddressToSocket();
    void WaitConnectionRequest();
    void AcceptIncoming();
    void ConnectToServer();

    int GetServerSocket();
    int GetClientSocket();
    int* GetClientSocketP();
    struct sockaddr_in  GetClientAddress();

private:
    int mServer_sock;
    int mClient_sock;
    struct sockaddr_in mServer_addr;
    struct sockaddr_in mClient_addr;
    int mClient_addr_sz;

    std::string mIP;
    int mPort;
    int mBackLogQueue_sz;       // size of the backlog queue 

};


NetSocket::NetSocket()
{

}

NetSocket::NetSocket(std::string _ip, int _pt, int _backlog)
{
    mIP = _ip;
    mPort = _pt;
    mBackLogQueue_sz = _backlog;
}

NetSocket::~NetSocket()
{
    if(mServer_sock > 2)
    {
        close(mServer_sock);
    }
    if(mClient_sock > 2)
    {
        close(mClient_sock);
    }
}

void NetSocket::InitializeSocket(int flag)
{
    if(flag == TCP)
    {
        mServer_sock = socket(PF_INET, SOCK_STREAM, 0);
    }
    else if(flag == UDP)
    {
        mServer_sock = socket(PF_INET, SOCK_DGRAM, 0);
    }
}

void NetSocket::SetServerAddress(int flag)
{
    memset(&mServer_addr, 0, sizeof(mServer_addr));
    mServer_addr.sin_family = AF_INET;
    if(flag == SERVER)
    {
        mServer_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else if(flag == CLIENT)
    {
        mServer_addr.sin_addr.s_addr = inet_addr(mIP.c_str());
    }
    mServer_addr.sin_port = htons(mPort);
}

void NetSocket::BindAddressToSocket()
{
    if(bind(mServer_sock, (struct sockaddr*)&mServer_addr, sizeof(mServer_addr))==-1)
    {
        ErrorHandling("bind(...) failed. This program is terminated...");
    }
}

void NetSocket::WaitConnectionRequest()
{
    if(listen(mServer_sock, mBackLogQueue_sz) == -1)
    {
        ErrorHandling("listen(...) failed. This program is terminated...");
    }
}

void NetSocket::AcceptIncoming()
{
    mClient_addr_sz = sizeof(mClient_addr);
    mClient_sock = accept(mServer_sock, (struct sockaddr*)&mClient_addr, (socklen_t*)&mClient_addr_sz);
}

void NetSocket::ConnectToServer()
{
    if(connect(mServer_sock, (struct sockaddr*)&mServer_addr, sizeof(mServer_addr)) == -1)
    {
        ErrorHandling("connect(...) failed. This program is terminated...");
    }
}

int NetSocket::GetServerSocket()
{
    return mServer_sock;
}

int NetSocket::GetClientSocket()
{
    return mClient_sock;
}

int* NetSocket::GetClientSocketP()
{
    return &mClient_sock;
}

struct sockaddr_in NetSocket::GetClientAddress()
{
    return mClient_addr;
}


#endif
