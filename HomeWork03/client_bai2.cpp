// // Bài 1: Lập trình server chấp nhận kết nối từ các client 
// và vào vòng lặp hỏi tên client cho đến khi client 
// gửi đúng cú pháp “client_id: xxxxxx” trong đó xxxxxx là tên của client.
// Khi client gửi đúng cú pháp thì chuyển sang vòng lặp nhận dữ liệu từ client 
// và gửi dữ liệu đó đến các client còn lại, ví dụ : client có id “abc” gửi “xin chào” 
// thì các client khác sẽ nhận được : “abc: xin chao”.

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
    addr.sin_port = htons(9000);

    connect(_client, (SOCKADDR*)&addr, sizeof(addr));

    CreateThread(0, 0, ReceiverThread, 0, 0, 0);

    char buf[256];

    printf("Nhap username và password: ");
    while (true)
    {
        gets_s(buf, sizeof(buf));
        send(_client, buf, strlen(buf), 0);
    }
}

DWORD WINAPI ReceiverThread(LPVOID lpParam)
{
    char buf[256];
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
