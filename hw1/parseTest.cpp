#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include "parseReqResp.h"

using namespace std;

int main(int argc, char **argv){
  char *buf = NULL;
  int buflen;
  string method;
  string path;
  string host;
  string port;
  string protocol;
  string version;
  ParsedRequest *parse = (ParsedRequest*)malloc(sizeof(ParsedRequest));

  parse->method = NULL;
  buf = "CONNECT www.google.com:443 HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    //strcpy(buf, "CONNECT http://www.com:443 HTTP/1.1");
    //strcat(buf, "Host: www.google.com\r\n");
    //strcat(buf, "\r\n");
  buflen = (int)strlen(buf);
  ParseRequest(parse, buf, buflen);

  method.assign(parse->method);
  path.assign(parse->path);
  host.assign(parse->host);
  port.assign(parse->port);
  //protocol.assign(parse->protocol);
  version.assign(parse->version);

  cout << "method = " << method << " host = " << host << " port = " << port << " path = " << path << endl;
  cout << "protocol = " << protocol << " version = " << version << endl;
  //printf("method = %s, host = %s, port = %s, path = %s\n", method, host, port, path);
  //printf("protocol = %s, version = %s\n", protocol, version);
  
  return 0;
}
