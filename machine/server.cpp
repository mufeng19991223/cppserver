#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define BUFFER_SIZE 4096

// 主状态机
enum CHECK_STATE
{
    CHECK_STATE_REQUESTLINE = 0, // 请求行
    CHECK_STATE_HEADER           // 请求头
};

// 从状态机
enum LINE_STATUS
{
    LINE_OK = 0,                 // 读取到一个完整的行
    LINE_BAD,                    // 行出错
    LINE_OPEN                    // 行数据尚且不完整
};

// 服务器处理HTTP请求的结果
enum HTTP_CODE
{
    NO_REQUEST,                   // 请求不完整
    GET_REQUEST,                  // 表示获得了一个完整的客户请求
    BAD_REQUEST,                  // 客户请求有语法错误
    FORBIDDEN_REQUEST,            // 客户对资源没有足够的访问权限
    INTERNAL_ERROR,               // 服务器内部错误
    CLOSED_CONNECTION             // 客户端已经关闭连接
};


// 根据服务器处理结果发送成功或是失败信息
static const char* szret[] = {"I get a correct result\n", "Something wrong\n"};

// 从状态机,用于解析处一行内容
LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index)
{
    char temp;
    for (; checked_index < read_index; ++checked_index)
    {
        temp = buffer[ checked_index ];
        
        // 如果当前字符是'\r',即回车符,说明可能读取到一个完整行
        if (temp == '\r')
        {
            // 如果'\r'字符碰巧是目前buffer中最后一个已经被读入的客户数据,那么这次分析没有读取到一个完整行
            if ((checked_index + 1) == read_index)
            {
                return LINE_OPEN;
            }
            // 如果下一个字符是'\n',说明读取到了完整行
            else if (buffer[ checked_index + 1 ] == '\n')
            {
                buffer[ checked_index++ ] = '\0';
                buffer[ checked_index++ ] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        else if (temp == '\n')
        {
            if ((checked_index > 1) && buffer[ checked_index - 1 ] == '\r')
            {
                buffer[ checked_index - 1 ] = '\0';
                buffer[ checked_index++ ] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

// 分析请求行
HTTP_CODE parse_requestline(char* temp, CHECK_STATE& checkstate)
{
    char* url = strpbrk(temp, " \t");
    if (!url)
    {
        return BAD_REQUEST;
    }
    *url++ = '\0';   

    char* method = temp;
    if (strcasecmp(method, "GET") == 0) //仅支持GET方法
    {
        printf("The request method is GET\n");
    }
    else
    {
        return BAD_REQUEST;
    }

    url += strspn(url, " \t");
    char* version = strpbrk(url, " \t");
    if (!version)
    {
        return BAD_REQUEST;
    }
    *version++ = '\0';
    version += strspn(version, " \t");
    
    if (strcasecmp(version, "HTTP/1.1") != 0) // 仅支持HTTP/1.1
    {
        return BAD_REQUEST;
    }

    if (strncasecmp(url, "http://", 7) == 0)
    {
        url += 7;
        url = strchr(url, '/');
    }
    
    if (!url || url[ 0 ] != '/')
    {
        return BAD_REQUEST;
    }
    printf("The request URL is: %s\n", url);
    checkstate = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

// 分析头部字段
HTTP_CODE parse_headers(char* temp)
{
    // 空行说明得到正确的HTTP请求
    if (temp [ 0 ] == '\0')
    {
        return GET_REQUEST;
    }
    else if (strncasecmp(temp, "Host:", 5) == 0) //  处理"Host"头部字段
    {
        temp += 5;
        temp += strspn(temp, " \t");
        printf("the request host is: %s\n", temp);
    }
    else
    {
        printf("I can not handle this hander\n");
    }
    return NO_REQUEST;
}

// 分析HTTP请求的入口函数
HTTP_CODE parse_content(char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line)
{
    // 记录当前行的读取状态
    LINE_STATUS linestatus = LINE_OK;

    // 记录HTTP请求的处理结果
    HTTP_CODE retcode = NO_REQUEST;

    // 主状态机, 用于从buffer中取出所有完整的行
    while((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK)
    {
        char* temp = buffer + start_line;   // start_line是行在buffer中的起始位置
        start_line = checked_index;         // 记录下一行的起始位置
        // checkstate 记录主状态机当前的状态
        switch( checkstate )
        {
            case CHECK_STATE_REQUESTLINE:
            {
                retcode = parse_requestline(temp, checkstate);
                if (retcode == BAD_REQUEST)
                {
                    return BAD_REQUEST;
                }
                break;
            }
            case CHECK_STATE_HEADER:
            {
                retcode = parse_headers(temp);
                if (retcode == BAD_REQUEST)
                {
                    return BAD_REQUEST;
                }
                else if (retcode == GET_REQUEST)
                {
                    return GET_REQUEST;
                }
                break;
            }
            default:
            {
                return INTERNAL_ERROR;
            }
        }
    }

    // 若没有读到完整行,则表示需要继续读取客户数据才能进一步分析
    if (linestatus == LINE_OPEN)
    {
        return NO_REQUEST;
    }
    else
    {
        return BAD_REQUEST;
    }
}

int main(int artc, char* argv[])
{
    const char* ip = "192.168.152.128";
    int port = 9666;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);
    
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    listen(listenfd, 5);
    assert(ret != -1);

    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);
    int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlen);

    if (connfd < 0)
    {
        printf("errno is : %d\n", errno);
    }
    else
    {
        char buffer[ BUFFER_SIZE ];
        memset(buffer, '\0', BUFFER_SIZE);
        int data_read = 0;
        int read_index = 0;
        int checked_index = 0;
        int start_line = 0;

        // 设置主状态机的初始状态
        CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;
        while (1) 
        {
            data_read = recv(connfd, buffer + read_index, BUFFER_SIZE - read_index, 0);
            if (data_read == -1)
            {
                printf("reading failed\n");
                break;
            }
            else if (data_read == 0)
            {
                printf("remote client has closed the connection\n");
                break;
            }
            read_index += data_read;

            // 分析目前已经获得的所有客户数据
            HTTP_CODE result = parse_content(buffer, checked_index, checkstate, read_index, start_line);
            if (result == NO_REQUEST)
            {
                continue;
            }
            else if (result == GET_REQUEST)
            {
                send(connfd, szret[0], strlen(szret[0]), 0);
                break;
            }
            else
            {
                send(connfd, szret[1], strlen(szret[1]), 0);
            }
        }
        close(connfd);
    }

    close(listenfd);
    return 0;
}