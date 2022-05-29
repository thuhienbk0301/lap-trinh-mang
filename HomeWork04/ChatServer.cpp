// ChatServer.cpp :Lập trình ứng dụng Chat Server(sử dụng mô hình multi-thread)thực hiện các yêu cầu sau:
// Chuyển tiếp tin nhắn đến tất cả thành viên
// Chuyển tin nhắn đến một user khác
// Đăng nhập có kiểm tra tài khoản tồn tại hay không
// Đăng xuất
//  Lấy danh sách user đang đăng nhập
// Gửi thông báo khi có user đăng nhập, đăng xuất
// Gửi thông báo lỗi nếu user nhập sai lệnh / giao thức
// 

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS


typedef struct
{
    SOCKET client;
    char* id;
} CLIENT_INFO;

//Luu thong tin client khi dang nhap thanh cong
CLIENT_INFO clients[64];
int numClients = 0;

CRITICAL_SECTION cs; // khoi tao doi tuong cs de dung CRITICAL SECTION: dong bo va tranh xung dot trong thread 
DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);
boolean checkLogin(SOCKET, char*, char*);
void sendMessage(SOCKET, char*, char*);
void getList(SOCKET, char*);
boolean checkLogout(SOCKET, char*, char*);
void SendNotice(SOCKET, char*, int);

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    InitializeCriticalSection(&cs); //khoi tao doi tuong

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    printf("Dang cho ket noi moi...\n");
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);

    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("New client accepted: %d\n", client);
        CreateThread(0, 0, ClientThread, &client, 0, 0);
    }

    DeleteCriticalSection(&cs); // xoa doi tuong cs
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;

    int ret;
    char buf[256], tmp[256], id[256], syn[256]; //tmp la 1 bien de dung trong ham sscanf khong bi doc qua nhieu

    char* msg = (char*)calloc(256, sizeof(char));
    strcat(msg, "Dang nhap theo cu phap: CONNECT [your_id] \n");
    send(client, msg, strlen(msg), 0);

    while (1)
    {
        // Nhan du lieu tu client
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)// neu ma co loi thi xoa client do luon
        {
            printf("Error: %ld\n", WSAGetLastError());
            break;
        }

        // In ra man hinh
        buf[ret] = 0;
        printf("Received from client: %s\n", buf);

        int n = sscanf(buf, "%s %s %s", syn, id, tmp); // syn(syntax) = CONNECT, id =  id
        if (n == 2) // neu thanh cong (doc duoc 2 tu CONNECT va id
        {
            if (strcmp(syn, "CONNECT") == 0) // dung cu phap
            {
                strcat(msg, "Dang nhap thanh cong!\n");
                // gui cho client thong bao 
                send(client, msg, strlen(msg), 0);
               
                // Them client vao mang client
                clients[numClients].client = client;
                numClients++;
                break;
            }
            else // sai cu phap
            {
                strcat(msg, "Dang nhap that bai. Vui long dang nhap lai voi cu phap: CONNECT [your_id]\n");
                send(client, msg, strlen(msg), 0);
            }
        }
        else // sai cu phap
        {
            strcat(msg, "Ban nhap sai cu phap nen khong the dang nhap. Vui long dang nhap lai voi cu phap: CONNECT [your_id] \n");
            send(client, msg, strlen(msg), 0);
        } 
    }

    char* msgContinue = (char*)calloc(256, sizeof(char));
    strcat(msgContinue, "Nhap lenh de tiep tuc: SEND/LIST/DISCONNECT\n");
    send(client, msg, strlen(msg), 0);
    

    // xu ly du lieu
    // thuc hien lenh tu client
    while (1)
    {

        // Nhan du lieu tu client

        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)// neu ma co loi thi xoa client do luon
        {
            printf("Error: %ld\n", WSAGetLastError());
            //RemoveClient(client);
            //break;
            return 0; // ngat luon ctrinh con
        }

        // In ra man hinh
        buf[ret] = 0; //chen ky tu ket thuc xau
        printf("Received from client: %s\n", buf);
        sscanf(buf, "%s", syn); //

        if (strcmp(syn, "SEND") == 0)
            sendMessage(client, id, buf);
        else if (strcmp(syn, "LIST") == 0)
            getList(client, buf);
        else if (strcmp(syn, "DISCONNECT") == 0)
            checkLogout(client, id, buf);
        else
        {
            char* errorMsg = (char*)calloc(256, sizeof(char));
            strcat(errorMsg, "ERROR Lenh khong hop le.\n");
            send(client, errorMsg, strlen(errorMsg), 0);
        }
    }
    return 0;
}

void sendMessage(SOCKET client, char* buf, char* id) 
{
    char sendBuf[256];
    char target[256];
    char* errorMsg = (char*)calloc(256, sizeof(char));
    char* contentMsg = (char*)calloc(256, sizeof(char));
    char* successMsg = (char*)calloc(256, sizeof(char));
    int ret = sscanf(buf + strlen("SEND"), "%s", target); //buf nhan duoc co dang SEND .... .  Do do bo qua den SEND roi moi doc tiep luu vao target
    if (ret <= 0) // neu that bai (ko co bien nao duoc dien)
    {
        
        strcat(errorMsg, "SEND ERROR Gui tin nhan that bai.\n");
        send(client, errorMsg, strlen(errorMsg), 0);
    }
    else
    {
        if (strcmp(target, "ALL") == 0) // neu client gui lenh SEND ALL: gui cho cac user khac
        {
            char* contentMsg = buf + strlen("SEND") + strlen(target) + 2; // cong 2 vi co 1 space nen phai tro den 2 dia chi tiep theo moi co noi dung
            sprintf(sendBuf, "MESSAGE_ALL %s: %s", id, contentMsg); // gui id + noi dung id do gui vao sendBuf
            // gui cho cac user con lai (tru user dang gui len)
            for (int i = 0; i < numClients; i++)
                if (clients[i].client != client) //tru client gui ra thi gui cho tat ca cac user con lai
                    send(clients[i].client, sendBuf, strlen(sendBuf), 0);

            strcat(successMsg, "SEND OK\n");
            send(client, successMsg, strlen(successMsg), 0);
        }
        else //if(strcmp(target, clients[i].client)) // gui cho user cu the
        {
            sprintf(sendBuf, "MESSAGE %s %s", id, contentMsg);
            int i;
            //

            for (i = 0; i < numClients; i++)
            {
                if (strcmp(clients[i].id, target) == 0) // neu trung voi id cua user dang gui len, khong tu gui cho chinh minh
                {
                    break;
                }
                else
                {
                    send(clients[i].client, sendBuf, strlen(sendBuf), 0);
                    send(client, successMsg, strlen(successMsg), 0);
                }
            }
        }

    }
}

void getList(SOCKET client, char* buf) 
{
    char cmd[256], tmp[256];
    char* errorMsg = (char*)calloc(256, sizeof(char));
    char* successMsg = (char*)calloc(256, sizeof(char));
    int ret = sscanf(buf, "%s %s", cmd, tmp); // cmd = LIST, tmp rỗng
    if (ret == 1 && tmp == "\0") // neu doc duoc 1 bien va tmp bang rong thi dang xuat thanh cong
    {
        //SendNotice(client, id, 2);
        strcat(successMsg, "LIST OK\n");
        for (int i = 0; i < numClients; i++)
        {
            strcat(successMsg, clients[i].id);
            strcat(successMsg, "\n");
        }
        send(client, successMsg, strlen(successMsg), 0);
    }
    else // sai cu phap
    {
        strcat(errorMsg, "LIST ERROR gui tin nhan that bai\n");
        send(client, errorMsg, strlen(errorMsg), 0);
     
    }
}


// Ham kiem tra user dang xuat thanh cong hay khong
// Tra ve TRUE (neu dang xuat thanh cong), FALSE (neu dang xuat that bai)
// Tham so: client, id, buf (client's request)
boolean checkLogout(SOCKET client, char* buf, char* id)
{
    char cmd[256], tmp[256];
    char* errorMsg = (char*)calloc(256, sizeof(char));
    char* successMsg = (char*)calloc(256, sizeof(char));
    int ret = sscanf(buf, "%s %s", cmd, tmp); // cmd = DISCONNECT, tmp rỗng
    if (ret == 1 && tmp == "\0") // neu doc duoc 1 bien va tmp bang rong thi dang xuat thanh cong
    {
        SendNotice(client, id, 2);
        strcat(successMsg, "DISCONNECT OK\n");
        send(client, successMsg, strlen(successMsg), 0);
        RemoveClient(client);
        return TRUE;
        
    }
    else // sai cu phap
    {
        strcat(errorMsg, "DISCONNECT ERROR dang xuat that bai\n");
        send(client, errorMsg, strlen(errorMsg), 0);
        return FALSE;
    }

}

//

// Ham kiem tra user dang nhap thanh cong hay khong
// Tra ve TRUE (neu dang xuat thanh cong), FALSE (neu dang xuat that bai)
// Tham so: client, id, buf (client's request)
boolean checkLogin(SOCKET client, char* buf, char* id)
{

    char* errorMsg = (char*)calloc(256, sizeof(char));
    char* ms = (char*)calloc(256, sizeof(char));
    strcat(ms, "CONNECT");
    char* successMsg = (char*)calloc(256, sizeof(char));
    
  
    int ret = sscanf(buf + strlen("ms"), "%s", id);  // bo qua CONNECT, doc den id, vi client gui lenh: CONNECT [your_id]
    if (ret == 1) // neu doc duoc 1 tu
    {
        int i;
        for (i = 0; i < numClients; i++)
            if (strcmp(id, clients[i].id) == 0) //neu id 
                break;

        if (i < numClients)
        {
            strcat(errorMsg, "ID da duoc su dung. Hay chon ID khac");
            send(client, errorMsg, strlen(errorMsg), 0);
            return FALSE;
        }
        else if (strcmp(id, "ALL") == 0)
        {
            strcat(errorMsg, "ID khong hop le. Hay chon ID khac");
            send(client, errorMsg, strlen(errorMsg), 0);
            return FALSE;
        }
        else
        {
            strcat(successMsg, "CONNECT OK\n");
            send(client, successMsg, strlen(successMsg), 0);

            EnterCriticalSection(&cs);
            clients[numClients].id = id;
            clients[numClients].client = client;
            numClients++;
            LeaveCriticalSection(&cs);
            SendNotice(client, id, 1);
            return TRUE;
        }
    }
    else
    {
        strcat(errorMsg, "CONNECT ERROR Sai cu phap lenh CONNECT");
        send(client, errorMsg, strlen(errorMsg), 0);
        return FALSE;
    }
}

// Thong bao den cho nguoi dung khac neu 1 nguoi dung khac dang nhap thanh cong (flag == 1) hoac dang xuat thanh cong (flag == 0)
void SendNotice(SOCKET client, char* id, int flag)
{
    char sendBuf[32];
    if (flag == 1) // neu dang nhap thanh cong
    {
        sprintf(sendBuf, "USER_CONNECT: %s\n", id); //  VD: USER_CONNECT: 152
        for (int i = 0; i < numClients; i++)
            if (clients[i].client != client)
                send(clients[i].client, sendBuf, strlen(sendBuf), 0); 
    }
    else if (flag == 0) // dang xuat thanh cong
    {
        sprintf(sendBuf, "USER_DISCONNECT: %s\n", id);
        for (int i = 0; i < numClients; i++)
            if (clients[i].client != client)
                send(clients[i].client, sendBuf, strlen(sendBuf), 0);
    }
}

void RemoveClient(SOCKET client)
{
    // Tim vi tri can xoa
    int i = 0;
    while (clients[i].client != client) i++; //hoac: for(; i < numClients; i++) if(clients[i] != client) break;


    EnterCriticalSection(&cs); //truy nhap vao vung tranh chap
    // Xoa socket client khoi mang
    if (i < numClients - 1)
        clients[i] = clients[numClients - 1];

    // Giam so phan tu di 1
    numClients--;
    LeaveCriticalSection(&cs); //roi khoi vung tranh chap
}




