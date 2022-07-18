// //FPT CLIENT


#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>

#pragma comment(lib, "ws2_32")

DWORD WINAPI ReceiverThread(LPVOID);

SOCKET _client;

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    _client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(21);

    connect(_client, (SOCKADDR*)&addr, sizeof(addr));

    CreateThread(0, 0, ReceiverThread, 0, 0, 0);

    char user[32], pass[32];
    char msgUser[256];
    char msgPass[256];
   



    while (true)
    {
        
        //dang nhap
        printf("Nhap username va password: ");
        scanf("%s%s", user, pass);
        sprintf(msgUser, "USER %s\r\n", user);
        send(_client, msgUser, strlen(msgUser), 0);
        sprintf(msgPass, "PASS %s\r\n", pass);
        send(_client, msgPass, strlen(msgPass), 0);

        //if(strcpm(user, "exit") == 0)

        //
    }


}

DWORD WINAPI ReceiverThread(LPVOID lpParam)
{
    char buf[2048];
    int ret;

    while (true)
    {
        ret = recv(_client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        printf("%s\n", buf);
    }
    closesocket(_client);
}
