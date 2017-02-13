#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parseReqResp.h"

int main(int argc, char **argv){
  char buf[512];
  int buflen;
  ParsedRequest *parse = (ParsedRequest*)malloc(sizeof(ParsedRequest));
  
  parse->method = NULL;
  strcpy(buf, "CONNECT api-glb-ash.smoot.apple.com:443 HTTP/1.1");
  strcat(buf, "Host: www.google.com\r\n");
  strcat(buf, "\r\n");
  buflen = (int)strlen(buf);
  ParseRequest(parse, buf, buflen);

  printf("method = %s, host = %s, port = %s, path = %s\n", parse->method, parse->host, parse->port, parse->path);
  printf("protocol = %s\n", parse->protocol);
  return 0;
}



