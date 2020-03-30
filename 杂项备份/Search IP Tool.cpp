#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

int main(){
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
	char hostbyname[50] = "";
	
	printf("please input the wanted Hostname:");
	gets(hostbyname);
	
    struct hostent *host = gethostbyname(hostbyname);
    if(!host){
        puts("Get IP address error!");
        system("pause");
        exit(0);
    }

    //����
    for(int i=0; host->h_aliases[i]; i++){
        printf("Aliases %d: %s\n", i+1, host->h_aliases[i]);
    }

    //��ַ����
    printf("Address type: %s\n", (host->h_addrtype==AF_INET) ? "AF_INET": "AF_INET6");

    //IP��ַ
    for(int i=0; host->h_addr_list[i]; i++){
        printf("IP addr %d: %s\n", i+1, inet_ntoa( *(struct in_addr*)host->h_addr_list[i] ) );
    }

    system("pause");
    return 0;
}
