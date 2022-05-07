//BTVN ngay 08/5/2022: TCPClient, kết nối đến một máy chủ xác
// định bởi tên miền hoặc địa chỉ IP.Sau đó nhận dữ liệu từ bàn
// phím và gửi đến server.Tham số được truyền vào từ dòng
// lệnh có dạng TCPClient.exe <Địa chỉ IP / Tên miền> <Cổng>


#include <stdio.h> //chuyen sang ngon ngu C de co the vao ra duoc cac ham cua C
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>


#define BUFSIZE 1000

#pragma comment(lib,"ws2_32")
#pragma warning(disable:4996)

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);
    if (WSAStartup(wVersion, &wsaData)) {
        printf("Version not supported");
    }

    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]); //localhost/IP
    addr.sin_port = htons(atoi(argv[2])); //tuong tu nhu ben server

    int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));
    if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        printf("Connect failed: %d\n", ret);
        return 0;
    }

    // nhan xau chao tu Server
    char buf[256];
    int result = recv(client, buf, sizeof(buf), 0);

    //neu ket noi bi ngat
    if (result <= 0)
    {
        printf("Connect closed!\n");
        return 1;
    }

    //neu co du lieu thi them ki tu ket thuc xau
    if (result < sizeof(buf))
   
        buf[result] = 0;
        // in cau chao ra man hinh
    printf("From server: %s\n", buf);

    //doc du lieu tu ban phim, gui len server
    while (1)
    {
        printf("Nhap du lieu:");
        gets_s(buf, sizeof(buf));

        // gui cho server
        send(client, buf, strlen(buf), 0);
        // neu nguoi dung go exit thi thoat
        if (strcmp(buf, "exit") == 0)
        {
            break;
        }
    }
    closesocket(client);
    WSACleanup();


}