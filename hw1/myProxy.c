#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "parseReqResp.h"

#define PROXYPORT 5060 //proxy listen port
#define BUFSIZE 10240
#define true 1
#define false 0

int CLIENT_SOCK;
  
//using namespace std;

struct SocketPair_t{
  int user_proxy;
  int proxy_server;
  int IsUser_proxyClosed;
  int IsProxy_serverClosed;
};
typedef struct SocketPair_t SocketPair;

struct ProxyParam_t{
  char *host;
  SocketPair *pPair;
  char *port;
  char buf[BUFSIZE];
  int buflen;
};
typedef struct ProxyParam_t ProxyParam;

int ProxytoServerAfter(ProxyParam *proxyP){
  char buf[BUFSIZE]; //char*
  //char recvbuf[2048575];
  int buflen = proxyP->buflen;
  int rval;

  printf("After\n");
  memcpy(buf, proxyP->buf, BUFSIZE); 
  /* send request to server */
  /* ID: Requesting "REQUEST from SERVER */
  while(!proxyP->pPair->IsUser_proxyClosed && !proxyP->pPair->IsProxy_serverClosed){
    if((rval = send(proxyP->pPair->proxy_server, buf, buflen, 0)) < 0){
      perror("send failed");
      if(proxyP->pPair->IsProxy_serverClosed == false){
	close(proxyP->pPair->proxy_server);
	proxyP->pPair->IsProxy_serverClosed = true;
      }
      exit(1);
    }
    //setsockopt
    //how do i know the end of response
    /* recv response */
    rval = recv(proxyP->pPair->proxy_server, buf, sizeof(buf), 0);
    if(rval < 0){	
      perror("reading stream message error");
      //close(server_sock);
      close(proxyP->pPair->proxy_server);
      proxyP->pPair->IsProxy_serverClosed = true;
      break;
    }
    
    if(rval == 0){
      perror("Ending connection\n");
      //close(server_sock);
      proxyP->pPair->IsProxy_serverClosed = true;
      break;
    }
    
    buflen = rval;
    //printf("%zu\n", strlen(recvbuf));
    // debug
    printf("MSG received: %s\n", buf);
    
    //memcpy(proxyP->buf, recvbuf,  BUFSIZE);
    //proxyP->buflen = buflen;
    /* send response back */
    if((rval = send(proxyP->pPair->user_proxy, buf, buflen, 0)) < 0){
      perror("send failed");
      close(proxyP->pPair->user_proxy);
      proxyP->pPair->IsUser_proxyClosed = true;
      break;
    }
  }

  if(proxyP->pPair->IsUser_proxyClosed == false){
    close(proxyP->pPair->user_proxy);
    proxyP->pPair->IsUser_proxyClosed = true;
  }
  if(proxyP->pPair->IsProxy_serverClosed == false){
    close(proxyP->pPair->proxy_server);
    proxyP->pPair->IsProxy_serverClosed = true;
  }
  
  
  return 0;
}

int ProxytoServer(ProxyParam *proxyP){
  //char buf[BUFSIZE];
  //char recvbuf[65535];
  int buflen = proxyP->buflen;
  struct sockaddr_in server;
  struct hostent *hp;
  int server_sock;
  //int rval;

  //memcpy(buf, proxyP->buf, BUFSIZE);

  hp = gethostbyname(proxyP->host);
  if(hp == 0){
    perror("gethostbyname failed");
    exit(1);
  }

  memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_family = hp->h_addrtype;
  server.sin_port = htons(80);
  /* create server_sock for send and listen */
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(server_sock < 0){
    perror("socket failed");
    proxyP->pPair->IsProxy_serverClosed = true;
    exit(1);
  }
  if(connect(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0){
    perror("connect failed");
    proxyP->pPair->IsProxy_serverClosed = true;
    exit(1);
  }else{
    printf("connect success\n");
  }
  proxyP->pPair->proxy_server = server_sock;
  proxyP->pPair->IsProxy_serverClosed = false;

  ProxytoServerAfter(proxyP);

  return 0;
}

int UsertoProxy(void *pParam){
  char buf[BUFSIZE];
  int buflen;
  struct sockaddr_in client;
  int msg_socket;
  int rval;
  socklen_t addrlen;
  SocketPair sPair;
  ProxyParam proxyP;
  ParsedRequest *parse_req = (ParsedRequest*)malloc(sizeof(ParsedRequest));
  //ParsedResponse *parse_res = (ParsedResponse*)malloc(sizeof(ParsedResponse));

  addrlen = sizeof(client);
  msg_socket = accept(CLIENT_SOCK, (struct sockaddr*)&client, &addrlen);
  if(msg_socket == -1){
    perror("accept failed");
  }

  sPair.IsUser_proxyClosed = false;
  sPair.IsProxy_serverClosed = true;
  sPair.user_proxy = msg_socket;

  if((rval = recv(sPair.user_proxy, buf, sizeof(buf), 0)) < 0){
    perror("reading stream message error");
    if(sPair.IsUser_proxyClosed == false){
      close(sPair.user_proxy);
      sPair.IsUser_proxyClosed = true;
    }
  }
  if(rval == 0){
    perror("Ending connection\n");
    if(sPair.IsUser_proxyClosed == false){
      close(sPair.user_proxy);
      sPair.IsUser_proxyClosed = true;
    }
  }
  // debug
  printf("MSG: %s\n", buf);

  proxyP.pPair = &sPair;
  buflen = rval;
  ParseRequest(parse_req, buf, buflen);
  proxyP.host = parse_req->host;
  proxyP.port = parse_req->port;
  memcpy(proxyP.buf, buf, BUFSIZE);
  proxyP.buflen = buflen;

  ProxytoServer(&proxyP);
  while(sPair.IsUser_proxyClosed == false && sPair.IsProxy_serverClosed == false){
    //while(1){
    if((rval = recv(sPair.user_proxy, buf, sizeof(buf), 0)) < 0){
      perror("reading stream message error");
      if(sPair.IsUser_proxyClosed == false){
	close(sPair.user_proxy);
	sPair.IsUser_proxyClosed = true;
      }
    }
    if(rval == 0){
      perror("Ending connection\n");
      if(sPair.IsUser_proxyClosed == false){
	close(sPair.user_proxy);
	sPair.IsUser_proxyClosed = true;
      }
    }
    buflen = rval;
    ParseRequest(parse_req, buf, buflen);
    proxyP.host = parse_req->host;
    proxyP.port = parse_req->port;
    memcpy(proxyP.buf, buf, BUFSIZE);
    proxyP.buflen = buflen;
       
    ProxytoServerAfter(&proxyP);
  }
  return 0;
}

int Proxy_Init(){
  struct sockaddr_in local;
  int client_sock;

  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons(PROXYPORT);

  /* create client_sock for listen */
  client_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(client_sock < 0){
    perror("socket failed");
    exit(1);
  }

  /* call blind */
  if(bind(client_sock, (struct sockaddr*)&local, sizeof(local))){
    perror("bind failed");
    exit(1);
  }

  /* listen */
  listen(client_sock, 5);
  CLIENT_SOCK = client_sock;

  UsertoProxy(NULL);

  return 0;
}

int Proxy_Closer(){
  close(CLIENT_SOCK);
  return 0;
}

int main(int argc, char **argv){
  Proxy_Init();
  while(1){
    UsertoProxy(NULL);
  }
  Proxy_Closer();

  return 0;
}

  
