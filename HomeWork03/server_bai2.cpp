// Bài 2:Sử dụng mô hình đa luồng, viết chương trình telnet server làm nhiệm vụ sau:
// Khi đã kết nối với 1 client nào đó, yêu cầu client gửi user và pass, so sánh với
// file cơ sở dữ liệu là một file text, mỗi dòng chứa một cặp user + pass ví dụ :
// “admin admin
// guest nopass

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS

DWORD WINAPI ClientThread(LPVOID);

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
    char buf[256], user[32], pass[32], fileBuf[256], accountBuf[256];

    //xu ly dang nhap
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


        // Xu ly du lieu
        // Doc buf va luu lan luot vao user va pass thuhien 123
        int n = sscanf(buf, "%s %s", user, pass); //doc cac ky tu lien tiep nhau den khi xuat hien space// tra ve so bien duoc dien

        //printf("username là: %s\n", user);
        //printf("password là: %s", pass);

        char* msg = (char*)calloc(256, sizeof(char));
        char currentUser[256];
        char currentPass[256];

        if (n == 2) // neu thanh cong, so sanh voi CSDL trong file
        {
            char input[1000] = { '\0' };
            // mo file de doc file
            FILE* f;
            if ((f = fopen("c:\\test\\accounts.txt", "r")) == NULL) {
                printf("Error opening file\n");
                return 1;
            }
            int temp = 0;  // bien khoi tao, luc nay chua so sanh user va pass trong file CSDL
            while (true) // khi nao chua co ki tu ket thuc eof trong file thi van thuc hien doc cac ki tu trong file
            //printf("Debug: %d", temp);
            {
                printf("Debug: %d\n", temp);
                if (fgets(input, 999, f) == NULL) { // doc tung dong, neu khong co dong tiep theo thi break
                    break;
                }; // doc file f va luu vao buffer fileBuf, sizeof(buf): so ki tu toi da duoc doc
                
                sscanf(input, "%s %s\n", currentUser, currentPass);

                if (strcmp(user, currentUser) == 0 && strcmp(pass, currentPass) == 0) // buf nhan duoc tu client chua user va pass, so sanh voi fileBuf neu bang nhau tra ve 0
                {
                    temp = 1; // bien dem tang len khi so sanh user va pass hop le voi CSDL trong file
                    //printf("Debug: %d", temp );
                    break;
                }
            }
            fclose(f);

            if (temp == 1) // tim thay user va pass hop le
            {
                // Tim thay tai khoan
                strcat(msg,"Dang nhap thanh cong. Hay nhap lenh de thuc hien.\n");
                send(client, msg, strlen(msg), 0);
                break;
            }
            else
            {
                // Sai tai khoan
                strcat(msg,"Sai tai khoan. Hay nhap lai.\n");
                send(client, msg, strlen(msg), 0);
            }
        }
        else // sai cu phap
        {
            strcat(msg, "Ban nhap sai cu phap. Xin hay nhap lai\n");
            send(client, msg, strlen(msg), 0);
        }
    }
    // thuc hien lenh tu client
    while (1)
    {

        // Nhan du lieu tu client

        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)// neu ma co loi thi xoa client do luon
        {
            closesocket(client);
            break;
        }

       
        buf[ret] = 0;
        
        char cmd[256];

        // gui buf nhan duoc tu client den cmd
        sprintf(cmd, "%s > c:\\temp\\out.txt", buf); // buf la du lieu nhan duoc tu client (VD: dir c:\temp). 
                                                        // ==> luc nay cmd = dir c:\temp > c:\\test\\output.txt  
                                                        // c:\\test\\output.txt: dinh huong kqua lenh dir vao file van ban

        printf("%s\n", buf);
        system(buf); // thuc hien lenh do client gui den

        // Doc file ket qua va gui cho client
        FILE* f = fopen("c:\\temp\\out.txt", "r");
        while (!feof(f))
        {
            fgets(fileBuf, sizeof(buf), f);
            send(client, fileBuf, strlen(fileBuf), 0);
        }
        fclose(f);
    }
    return 0;
}


