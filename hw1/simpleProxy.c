#include <stdio.h>
#include <sys/types.h>                                                                    
#include <sys/socket.h>                                                                   
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>                                                                     
#include <string.h>                                                                       
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "parseReqResp.h"

int main(int argc, char **argv){

  int sock1;
  int sock2;
  int mysock1;
  struct sockaddr_in server;
  struct sockaddr_in proxy;
  struct hostent *host;
  char *hostname;
  char buf[4096];
  size_t buflen;
  char reqbuf[1024];
  int rval;
  time_t recv_time;
  ParsedRequest *parse = (ParsedRequest*)malloc(sizeof(ParsedRequest));

  /* create socket1 for client */
  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  if(sock1 < 0){
    perror("socket failed");
    exit(1);
  }
  /* create socket2 for server */
  sock2 = socket(AF_INET, SOCK_STREAM, 0);
  if(sock2 < 0){
    perror("socket failed");
    exit(1);
  }

  proxy.sin_family = AF_INET;
  proxy.sin_addr.s_addr = INADDR_ANY;
  proxy.sin_port = htons(5000);
  server.sin_family = AF_INET;
  /* call blind */
  if(bind(sock1, (struct sockaddr*)&proxy, sizeof(proxy))){
    perror("bind failed");
    exit(1);
  }
  
  /* listen */
  listen(sock1, 5);

  /* accept */
  do{
    mysock1 = accept(sock1, (struct sockaddr*)0, 0);
    if(mysock1 == -1){
      perror("accept failed");
    }else{
      memset(buf, 0, sizeof(buf)); 
      if((rval = recv(mysock1, buf, sizeof(buf), 0)) < 0){
	perror("reading stream message error");
      }else if(rval == 0){
	perror("Ending connection\n");
      }else{
	printf("MSG: %s\n", buf);
      }
      printf("Got the message (rval = %d)\n", rval);

      time(&recv_time);
      printf("Current local time and date: %s", asctime(localtime(&recv_time)));
      //printf("%s\n", inet_ntoa(proxy.sin_addr));
      buflen = strlen(buf);
      ParseRequest(parse, buf, buflen);
      /* get hostname */
      hostname = parse->host;
      host = gethostbyname(hostname);
      if(host == 0){
	perror("gethostbyname failed");
	exit(1);
      }

      memcpy(&server.sin_addr, host->h_addr, host->h_length);
      server.sin_port = htons(80);
      /* fill request buffer */
      sprintf(reqbuf, "%s %s %s\r\nHost:%s:%s\r\n\r\n", parse->method, parse->path, parse->version, parse->host, parse->port);

      if(connect(sock2, (struct sockaddr*)&server, sizeof(server)) < 0){
	perror("connect failed");
	exit(1);
      }

      if(send(sock2, reqbuf, sizeof(reqbuf), 0) < 0){
	perror("send failed");
	exit(1);
      }
      printf("Send %s\n", reqbuf);
      /* recv response */ 
      memset(buf, 0, sizeof(buf));
      if((rval = recv(sock2, buf, sizeof(buf), 0)) < 0){
	perror("reading stream message error");
      }else if(rval == 0){
	perror("Ending connection\n");
      }else{
	printf("MSG: %s\n", buf);
      }
      printf("Got the message (rval = %d)\n", rval);     
      
      close(mysock1);
    }
  }while(1);
  
  return 0;
}
  
