//BTVN ngay 8/5: TCPServer, đợi kết nối ở cổng xác định
// bởi tham số dòng lệnh.Mỗi khi có client kết nối đến, thì gửi
// xâu chào được chỉ ra trong một tệp tin xác định, sau đó ghi
// toàn bộ nội dung client gửi đến vào một tệp tin khác được chỉ
// ra trong tham số dòng lệnh

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
    addr.sin_port = htons(atoi(argv[1]));

    //Gan dia chi vao socket
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));

    // Chuyen sang trang thai cho ket noi
    listen(listener, 5);

    printf("Dang cho ket noi moi..\n");
    SOCKET client = accept(listener, NULL, NULL); // chap nhan ket noi, luu vao client

    char buf[256];
    int ret;

    // Doc xau chao tu file
    FILE* f;
    if ((f = fopen(argv[2], "r")) == NULL) {
        printf("Error opening text file.\n");
    }

    fgets(buf, 256, f);

    printf("%s\n", buf);

    fclose(f);

    // Gui sang client
    send(client, buf, strlen(buf), 0);

    // Nhan du lieu tu client va ghi vao file
    FILE* f2;

     
    // TCPServer.exe <Cổng> <Tệp tin chứa câu chào> <Tệp tin lưu nội dung client gửi đến> --> argv[3] = <tên tệp tin lưu ND client gửi đến
  
    printf("%s\n", argv[3]);

    while (1)
    {
        if ((f2 = fopen(argv[3], "a")) == NULL) {
            printf("Error opening text file.\n");
        }
        char recvBuf[256];
        ret = recv(client, recvBuf, sizeof(recvBuf), 0);
        if (ret <= 0) {
            return -1;
        }
        recvBuf[ret] = '\0';
        printf("%s\n", recvBuf);
       
        fprintf(f2, "%s\n", recvBuf);
        fclose(f2);
    }



    closesocket(client);
    closesocket(listener);
    WSACleanup();

}