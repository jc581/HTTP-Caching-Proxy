#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "parseRes.h"
#include "parseRes.cpp"

using namespace std;

int main() {
  
  ParsedResponse *parse = new ParsedResponse();

  parse->buf = NULL;

  string buffer = string("HTTP/1.1 200 OK\r\nE-Tag: 3f80f-1b6-3e1cb03b\r\nExpires: Thu, 26 Jan 2012 04:16:13 GMT\r\nCache-Control: private, max-age=0\r\nContentl-Length: 138\r\n\r\n<html>\r\n<head>\r\n<title>An Example Page</title>\r\n</head>\r\n<body>\r\nHello World, this is a very simple HTML document.\r\\n</body>\r\n</html>");

  ParseResponse(parse, buffer);
  
  cout << "Response Line:" << parse->response_line << endl;
  cout << "version:" << parse->version << endl;
  cout << "status-code:" << parse->status_code << endl;
  cout << "reason_phrase:" << parse->reason_phrase << endl;

  cout << "HEADERs are as follows:" << endl;
  
  cout << "expires:" << parse->expires << endl;
  cout << "cache_control:" << parse->cache_control << endl;
  cout << "content_length:" << parse->content_length << endl;
  cout << "e_tag:" << parse->e_tag << endl;

  free(parse);
  return 0;
}
