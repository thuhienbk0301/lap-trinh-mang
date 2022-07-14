// Ứng dụng cần đọc giá trị các tham số, thực hiện phép tính và 
// trả lại kết quả cho trình duyệt là trang web thể hiện phép tính đó.
//


#include <stdio.h>

#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


DWORD WINAPI ClientThread(LPVOID);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9090);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;

	char req[2048];
	int ret = recv(client, req, sizeof(req), 0);
	req[ret] = 0;
	printf("%s\r\n", req);

	char method[8], uri[256];
	sscanf(req, "%s %s", method, uri);// tach phuong thuc GET voi uri

	printf("%s\n", method);
	printf("%s", uri);

	char op[16]; //phep toan(add/sub/mul/div)


	char a[10], b[10];
	sscanf(uri, "/op=%s&a=%s&b=%s\n", op, a, b);
	printf("phep tinh: %s", a);
    
	if (strcmp(op, "add") == 0)
	{
		int sum = atoi(a) + atoi(b);
		printf("sum=%s\r", sum);

		char sum1[10];
		sprintf(sum1, "%d", sum);

		char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>";
		send(client, header, strlen(header), 0);
		strcat(sum1, "</body></html>");
		send(client, sum1, strlen(sum1), 0);
	}
	else if (strcmp(op, "sub") == 0)
	{
		int sub = atoi(a) - atoi(b);
		printf("sub=%s\r", sub);

		char sub1[10];
		sprintf(sub1, "%d", sub);

		char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>";
		send(client, header, strlen(header), 0);
		strcat(sub1, "</body></html>");
		send(client, sub1, strlen(sub1), 0);
	}
	else if (strcmp(op, "mul") == 0)
	{
		int mul = atoi(a) * atoi(b);
		printf("mul=%s\r", mul);

		char mul1[10];
		sprintf(mul1, "%d", mul);

		char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>";
		send(client, header, strlen(header), 0);
		strcat(mul1, "</body></html>");
		send(client, mul1, strlen(mul1), 0);
	}
	else if (strcmp(op, "div") == 0)
	{
		int div = atoi(a) / atoi(b);
		printf("div=%s\r", div);

		char div1[10];
		sprintf(div1, "%d", div);

		char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>";
		send(client, header, strlen(header), 0);
		strcat(div1, "</body></html>");
		send(client, div1, strlen(div1), 0);
	}
	else
	{
		// Tra lai ket qua
		const char* res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Invalid!</h1></body></html>";
		send(client, res, strlen(res), 0);
	}

	closesocket(client);
	return 0;
}