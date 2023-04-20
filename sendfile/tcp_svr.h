#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

class Tcpsvr
{
public:
    Tcpsvr()
    {
        this->_sockfd = -1;
    }
    ~Tcpsvr() {}

    bool Createsocket()
    {
        this->_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (this->_sockfd < 0)
        {
            printf("Createsocket Error\n");
            return false;
        }   
        return true;
    }

    bool Bind(const char* ip, const char* port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(port));
        addr.sin_addr.s_addr = inet_addr(ip);
        if (bind(this->_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            printf("Bind Error\n");
            return false;
        }
        return true;
    }

    bool Listen(int value = 5)
    {
        if (listen(this->_sockfd, value) < 0)
        {
            printf("Listen Error\n");
            return false;
        }
        return true;
    }

    bool Accept(Tcpsvr* client, struct sockaddr_in *addr)
    {
        socklen_t len = sizeof(struct sockaddr_in);
        int newfd = accept(this->_sockfd, (struct sockaddr*)addr, &len);
        if (newfd < 0)
        {
            printf("Accept Error\n");
            return false;
        }
        client->_sockfd = newfd;
        return true;
    }


    bool Connect(const char* ip, uint16_t port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);
        if (connect(this->_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            printf("Connect Error\n");
            return false;
        }
        return true;
    }

    int Recv(int connfd, char* buf)
    {
        int len = recv(connfd, buf, sizeof(buf), 0);
        if (len < 0)
        {
            printf("Recv Error\n");
            return 0;
        }
        return len;
    }

    bool Send(int connfd, char* buf)
    {
        int len = send(connfd, buf, sizeof(buf), 0);
        if (len < 0)
        {
            printf("Send Error\n");
            return false;
        }
        return true;
    }

    void Splice(Tcpsvr* client)
    {   
        int pipefd[2];
        pipe(pipefd);

        int res = splice(STDIN_FILENO, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        splice(pipefd[0], NULL, client->_sockfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
    }

    void Close()
    {
        close(this->_sockfd);
    }
    int GetFd()
    {
        return this->_sockfd;
    }
public:
    int _sockfd;
};