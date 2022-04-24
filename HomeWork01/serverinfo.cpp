// BTVN ngay 18/04/2022
// serverinfo đợi kết nối từ clientinfo và thu nhận thông tin từ client, 
//hiện kết quả ra màn hình. Cổng mà server đợi kết nối nhận vào từ tham số dòng lệnh. VD: serverinfo.exe 1234

#include <stdio.h> 
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32")
#pragma warning(disable:4996)


int main(int argc, char* argv[])
{

    // Khoi tao
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);
    if (WSAStartup(wVersion, &wsaData)) {
        printf("Version not supported");
    }

    //Tao doi tuong Socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));// serverinfo.exe 1234 --> argv[1] == "1234", atoi: chuyen chuoi-->so

    //Gan dia chi vao socket
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));

    // Chuyen sang trang thai cho ket noi
    listen(listener, 5);

    printf("Dang cho ket noi moi..\n");
    SOCKET client = accept(listener, NULL, NULL); // chap nhan ket noi, luu vao client

    printf("Chap nhan ket noi: %d\n", client);

    char buf[2048];
    int ret;

     
    ret = recv(client, buf, sizeof(buf), 0);
    printf("Thong tin ve may tinh:\n%s", buf);
      
    closesocket(client);
    closesocket(listener);
    WSACleanup();
}
