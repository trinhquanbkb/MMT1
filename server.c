#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int a;
    sscanf(argv[1], "%d", &a);
    char file[256];
    sprintf(file, "%s", argv[2]);

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(a);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }
    printf("Waiting for clients...\n");

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);

    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("Client IP: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    char *msg = "Hello client\n";
    send(client, msg, strlen(msg), 0);

    char buf[256];
    char current_time[100];
    time_t now;
    time(&now);
    strftime(current_time, sizeof(current_time), "%Y-%m-%d %H:%M:%S", localtime(&now));
    current_time[strlen(current_time)] = 0;

    while (1)
    {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            printf("Ket noi bi dong.\n");
            break;
        }
        if (ret < sizeof(buf))
            buf[ret] = 0;

        printf("%d bytes received\n", ret);
        printf("%s\n", buf);

        FILE *f = fopen(file, "a");
        fprintf(f, "%s %s %s\n", inet_ntoa(clientAddr.sin_addr), current_time, buf);
        fclose(f);
    }

    close(client);
    close(listener);
}