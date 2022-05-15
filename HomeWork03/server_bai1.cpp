// Bài 1: Lập trình server chấp nhận kết nối từ các client 
// và vào vòng lặp hỏi tên client cho đến khi client 
// gửi đúng cú pháp “client_id: xxxxxx” trong đó xxxxxx là tên của client.
// Khi client gửi đúng cú pháp thì chuyển sang vòng lặp nhận dữ liệu từ client 
// và gửi dữ liệu đó đến các client còn lại, ví dụ : client có id “abc” gửi “xin chào” 
// thì các client khác sẽ nhận được : “abc: xin chao”.



#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS

DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);

SOCKET clients[64];
int numClients;

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    printf("Dang cho ket noi moi...\n");
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);
    
    numClients = 0;
    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("New client accepted: %d\n", client);
        CreateThread(0, 0, ClientThread, &client, 0, 0);
    }

    closesocket(listener);
    WSACleanup();
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;
    char buf[256], clientID[32], clientName[128];
    while (1)
    {
        // Nhan du lieu tu client
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)// neu ma co loi thi xoa client do luon
        {
            printf("Error: %ld\n", WSAGetLastError());
            RemoveClient(client);
            break;
        }

        // In ra man hinh
        buf[ret] = 0;
        printf("Received from client: %s\n", buf);
      

        // Xu ly du lieu
        // Doc tu dau tien va luu vao clientID: , clientName = xxxxxx
        int n = sscanf(buf, "%s %s", clientID, clientName); //doc cac ky tu lien tiep nhau den khi xuat hien space// tra ve so bien duoc dien
       
        char *msg = (char *) calloc(256, sizeof(char));

        if (n == 2) // neu thanh cong
        {
            if (strcmp(clientID, "client_id:") == 0) // dung cu phap
            {
                strcat(msg, "Dung cu phap. Hay nhap tin nhan de tiep tuc\n");
                // gui cho client thong bao msd
                send(client, msg, strlen(msg), 0);
                // Them client vao mang client
                clients[numClients] = client;
                numClients++;
                break;
            }
            else // sai cu phap
            {
                strcat(msg,"Sai cu phap. Xin hay nhap lai\n");
                send(client, msg, strlen(msg), 0);
            }
        }
        else // sai cu phap
        {
            strcat(msg,"Ban nhap sai cu phap. Xin hay nhap lai\n");
            send(client, msg, strlen(msg), 0);
        }
    }
    // chuyen tiep tin nhan
    while (1)
    {
        // Nhan du lieu tu client
        
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)// neu ma co loi thi xoa client do luon
        {
            RemoveClient(client);
            break;
        }
        buf[ret] = 0;
        char sendBuf[256]; // du lieu nhan duoc tu Client
        // luu du lieu nhan duoc vao sendBuf roi gui cho cac client con lai
        sprintf(sendBuf, "%s: %s", clientName, buf);
        printf("%s\n", sendBuf);
        for (int i = 0; i < numClients; i++)
            if (clients[i] != client)
            {
                send(clients[i], sendBuf, strlen(sendBuf), 0);
            }
    }
    return 0;
}

void RemoveClient(SOCKET client)
{
    // Tim vi tri can xoa
    int i = 0;
    while (clients[i] != client) i++;

    // Xoa socket client khoi mang
    if (i < numClients - 1)
        clients[i] = clients[numClients - 1];

    // Giam so phan tu di 1
    numClients--;
}

