#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ADDRESS "google.com"
#define PORT "80"

int main(void) {
    int err, sockfd;
    struct addrinfo hints, *res;

    // socket creation
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket opening error");
        exit(1);
    }
    
    //resolve google.com
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM;
    err = getaddrinfo(ADDRESS, PORT, &hints, &res);
    if (err) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
        close(sockfd);
        exit(1);
    }
    
    //connect socket to google.com
    err = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (err != 0) {
        perror("Connect error");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    //send request to google
    char *request = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
    size_t sent = send(sockfd, request, strlen(request), 0);
    if (sent != strlen(request)) {
        perror("Send error");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    //get response from google
    char buffer[4096];
    int bytes = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytes < 0) {
        perror("Recv error");
        close(sockfd);
        freeaddrinfo(res);
        exit(1); 
    }
    buffer[bytes] = '\0';
    printf("%s\n", buffer);

    freeaddrinfo(res);
    close(sockfd);

    return 0;
}