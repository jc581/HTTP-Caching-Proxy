#include <string>

using namespace std;

class ParsedResponse {
 public:
  //resonse(first) line
  string response_line;
  string version;
  string status_code;
  string reason_phrase;

  // headers that might be useful for cache
  string expires;
  string cache_control;
  string max_age;
  string last_modified;
  string date;

  //other headers that might be useful for log
  string content_length;
  string e_tag;

  //buf and buflen fields are used internally to maintain the parsed request line
  char * buf;
  size_t buflen;
  
  ~ParsedResponse() {
    if (buf != NULL) {
      free(buf);
    }  
  }





};
