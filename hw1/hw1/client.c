#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA1 "CONNECT https://www.google.com/:443 HTTP/1.1\r\nHost:www.google.com:80\r\nConnection: keep-alive\r\n\r\n"
#define DATA2 "GET https://www.google.com/:443 HTTP/1.1\r\nHost:www.google.com:80\r\nConnection: keep-alive\r\n\r\n"

int main(int argc, char *argv[]){

  int sock;
  struct sockaddr_in server;
  struct hostent *hp;
  char buff[1024];

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0){
    perror("socket failed");
    exit(1);
  }

  server.sin_family =AF_INET;
  hp = gethostbyname(argv[1]);
  if(hp == 0){
    perror("gethostbyname failed");
    exit(1);
  }

  memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_port = htons(5060);

  if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0){
    perror("connect failed");
    exit(1);
  }

  if(send(sock, DATA2, sizeof(DATA2), 0) < 0){
    perror("send failed");
    exit(1);
  }
  printf("Send %s\n", DATA2);

  close(sock);

  return 0;
}
