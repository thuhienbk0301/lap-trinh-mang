//Bai 1: Client ket noi den may chu, gui thong tin ve ten may, danh sach o dia trong may, kich thuoc o dia
// Địa chỉ (tên miền) và cổng kết nối đến server nhận vào từ tham số dòng lệnh. VD: clientinfo.exe localhost 1234

#include <stdio.h> //chuyen sang ngon ngu C de co the vao ra duoc cac ham cua C
#include <winsock2.h>
#include <ws2tcpip.h>

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

    // Phan giai ten mien
    addrinfo* info;
    struct sockaddr_in* sockaddr_ipv4;

    char ipAddress[100];
   
    int result = getaddrinfo("localhost", "http", NULL, &info);
    if (result != 0)
    {
        printf("Khong phan giai duoc ten mien\n");
        return -1;
    }
    else {
        struct addrinfo hints; // 
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        DWORD dwRetval;
        struct addrinfo* result = NULL;
        struct addrinfo* ptr = NULL;

        // localhost
        dwRetval = getaddrinfo(argv[1], NULL, &hints, &result);
        if (dwRetval != 0) {
            if (dwRetval == 11001) { // If the error code is 11001 => the input domain name is not valid
                printf("Not found information\n");
                return 0;
            }
            printf("getaddrinfo failed with error: %d\n", dwRetval); // Otherwise there was another error
            WSACleanup();
            return 1;
        }
        
        for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
            sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            strcpy(ipAddress, inet_ntoa(sockaddr_ipv4->sin_addr));
            /*printf("%s\n", ipAddress);*/
        
        }
    }
   

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipAddress); //localhost
    addr.sin_port = htons(atoi(argv[2])); //tuong tu nhu ben server


    ////De dam bao server luon chay truoc khi client connet
    //system("pause");

    int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));
    if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        printf("Connect failed: %d\n", ret);
        return 0;
    }

    char buf[2048];

    //Tao buffer chua ten may tinh
    char computerName[BUFSIZE]; 
    DWORD computerName_Size = sizeof(computerName);
    GetComputerNameA(computerName, &computerName_Size);

    printf("%s\n", computerName);

    strcpy(buf, "Computer Name: ");
    strcat(buf, computerName);
    strcat(buf, "\nDisk list:\n");

    DWORD cchBuffer;
    WCHAR* driveStrings;
    ULARGE_INTEGER freeSpace;
    //LPDWORD lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters;

    // Find out how big a buffer we needs
    cchBuffer = GetLogicalDriveStrings(0, NULL);

    driveStrings = (WCHAR*)malloc((cchBuffer + 1) * sizeof(TCHAR));
  
    if (driveStrings == NULL)
    {
        return -1;
    }

    // Fetch all drive strings    
    GetLogicalDriveStrings(cchBuffer, driveStrings);
    

    while (*driveStrings)
    {
        // Dump drive information
        GetDiskFreeSpaceEx(driveStrings, &freeSpace, NULL, NULL);
        /*printf("%s\n", driveStrings);*/

        char* driveInformation = (char*)malloc(BUFSIZE * sizeof(char));
        sprintf(driveInformation, "Drive Name: %S Size: %I64u GB free\n", driveStrings, freeSpace.QuadPart / 1024 / 1024 / 1024);

        strcat(buf, driveInformation);

        // Move to next drive string
        // +1 is to move past the null at the end of the string.
        driveStrings += lstrlen(driveStrings) + 1;
    }

    /*printf("%s\n", buf);*/

    send(client, buf, sizeof(buf), 0);


    closesocket(client);
    WSACleanup();

    return 0;

}