#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parseReqResp.h"

/*
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
*/


int main(int argc, char **argv){
  char buf[512];
  int buflen;
  ParsedResponse *parse = (ParsedResponse*)malloc(sizeof(ParsedResponse));
  
  parse->buf = NULL;
  
  strcpy(buf, "HTTP/1.1 200 OK\r\n");
  strcat(buf, "E-Tag: 3f80f-1b6-3e1cb03b\r\n");
  strcat(buf, "Expires: Thu, 26 Jan 2012 04:16:13 GMT\r\n");
  strcat(buf, "Cache-Control: private, max-age=0\r\n");
  strcat(buf, "Contentl-Length: 138\r\n");
  strcat(buf, "\r\n");

  strcat(buf, "<html>\r\n<head>\r\n<title>An Example Page</title>\r\n</head>\r\n<body>\r\nHello World, this is a very simple HTML document.\r\n</body>\r\n</html>");
  
  buflen = (size_t)strlen(buf);
  ParseResponse(parse, buf, buflen);
  
  printf("Response Line: %s \n version: %s\n status-code: %s\n reason-phrase: %s\n\nHEADERs are as follows:\n Expires:%s\n Cache-Control:%s\n Contentl-Length:%s\n E-Tag:%s\n", parse->response_line, parse->version, parse->status_code, parse->reason_phrase, parse->expires, parse->cache_control, parse->content_length, parse->e_tag);
  
  //忘了free struct
  return 0;
}
