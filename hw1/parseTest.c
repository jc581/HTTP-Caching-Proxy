#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parseReqResp.h"

int main(int argc, char **argv){
  char buf[512];
  int buflen;
  ParsedRequest *parse = (ParsedRequest*)malloc(sizeof(ParsedRequest));
  
  parse->method = NULL;
  strcpy(buf, "GET /ece590_02/index.html HTTP/1.1\r\n");
  strcat(buf, "Host:www.duke.edu:79\r\n");
  strcat(buf, "\r\n");
  buflen = (int)strlen(buf);
  ParseRequest(parse, buf, buflen);

  printf("method = %s, host = %s, port = %s, path = %s\n", parse->method, parse->host, parse->port, parse->path);

  return 0;
}
