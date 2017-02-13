#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "parseReqResp.h"
#include "unordered_map.h"

int main(int argc, char **argv){
  /* socket variable */
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
  ParsedRequest *parse_req = (ParsedRequest*)malloc(sizeof(ParsedRequest));
  ParsedResponse *parse_res = (ParsedResponse*)malloc(sizeof(ParsedResponse));
  
  /* cache variable */
  time_t recv_time;
  char *uri;
  char *TIME;
  char *IPFROM;
  char *REQUEST;
  char *SERVER;
  char *RESPONSE;
  char *REASON;
  char *EXPIRES;
  Cache cache = Cache(40);
  
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
      /* clear buffer */
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
      TIME = asctime(localtime(&recv_time));
      printf("Current local time and date: %s", asctime(localtime(&recv_time)));

      IPFROM = inet_ntoa(proxy.sin_addr);
      printf("Ipfrom: %s\n", inet_ntoa(proxy.sin_addr));

      buflen = strlen(buf);
      ParseRequest(parse_req, buf, buflen);
      sprintf(REQUEST, "%s %s%s %s\r\n", parse_req->method, parse_req->host, parse_req->path, parse_req->version);
      printf("Request-line: %s\n", REQUEST);
      sprintf(uri, "%s%s", parse_req->host, parse_req->path);
      
      /* get hostname */
      hostname = parse_req->host;
      host = gethostbyname(hostname);
      if(host == 0){
	perror("gethostbyname failed");
	close(sock1);
	exit(1);
      }

      memcpy(&server.sin_addr, host->h_addr, host->h_length);
      server.sin_port = htons(80);
      /* method */
      if(strcmp(parse_req->method, "CONNECT") == 0){
	if(con_flag != 0){
	  continue;
	}
	if(connect(sock2, (struct sockaddr*)&server, sizeof(server)) < 0){
	  perror("connect failed");
	  exit(1);
	}
	con_flag = 1;
	continue;
      }
      if(strcmp(parse_req->method, "GET") == 0){
	/* check cache status */
	/* ID: not in cache ID: in cache, but expired at EXPIREDTIME ID: in cache, requires validation ID: in cache, valid */
	
	if(cache.getValue(uri) == NULL){
	  /* send request to server */
	  /* ID: Requesting "REQUEST from SERVER */
	  sprintf(reqbuf, "%s", buf);
	  if(send(sock2, reqbuf, sizeof(reqbuf), 0) < 0){
	    perror("send failed");
	    exit(1);
	  }
	  
	  /* recv response */
	  memset(buf, 0, 8192);
	  if((rval = recv(sock2, buf, 8192, 0)) < 0){
	    perror("reading stream message error");
	  }else if(rval == 0){
	    perror("Ending connection\n");
	  }else{
	    /* ID: Received "RESPONSE from SERVER */
	    buflen = strlen(buf);
	    ParseResponse(parse_res, buf, buflen);
	    REASON = /* ?? */;
	    EXPIRES = parse_res->expires; 
	    if(strcmp(parse_res->status_code, "200") == 0){
	      /* ID: not cacheable because REASON ID: cached, expires at EXPIRES ID: cached, but requires re-validation */
	      sprintf(uri, "%s%s", parse_req->host, parse_req->path);
	      cache.setValue(uri, buf);
	    }
	  }
	}else{
	  /* send cache-ed response back */
	  if(send(sock1, cache.getValue(uri), sizeof(cache.getValue(uri)), 0) < 0){
	    perror("send response failed");
	    exit(1);
	  }
	}
      }else{
	/* send request to server */
	sprintf(reqbuf, "%s", buf);
	if(send(sock2, reqbuf, sizeof(reqbuf), 0) < 0){
	  perror("send failed");
	  exit(1);
	}

	/* recv response */
	memset(buf, 0, 8192);
	if((rval = recv(sock2, buf, 8192, 0)) < 0){
	  perror("reading stream message error");
	}else if(rval == 0){
	  perror("Ending connection\n");
	}else{
	  //printf("MSG: %s\n", buf);
	}	
      }
      
      /* send response back */
      if(send(sock1, buf, sizeof(buf), 0) < 0){
	perror("send response failed");
	exit(1);
      }
      /* log ID: Responding " RESPONSE" */

      close(mysock1);
      close(sock1);
    }
  }while(1);

  return 0;
}
