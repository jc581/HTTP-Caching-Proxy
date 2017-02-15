#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

void first_line_extract(char** field, ParsedResponse* parse, char* tmp_buf, int flag) {
  if (flag == 0) {
    *field = strtok(parse->buf, " ");
  } else {
    *field = strtok(NULL, " "); 
  }
  if(*field == NULL) {
    perror("Parse error, incorrect response line");
    free(tmp_buf);
    free(parse->buf); 
    exit(1);
  }
}

int header_extract(char** field, const char* header, ParsedResponse* parse, char* tmp_buf) {
  char* curr = strstr(tmp_buf, header);
  if (curr != NULL) {
    curr = strstr(curr, ":") + 1;
    char* curr_end = strstr(curr, "\r\n");
    *field = (char *)malloc(curr_end - curr + 1);
    memcpy(*field, curr, curr_end - curr);
    (*field)[curr_end - curr] = '\0';

    return 0;
  }
  else {
    //    printf("Response parse: response does not include header /%s/\n", header);
    return -1;
  }
  
}

int ParseResponse(ParsedResponse *parse, string buffer) {

  char *index;
  char *tmp_buf;
  size_t len;
  
  if(parse->buf != NULL){
    perror("Parse error, already parsed");
    exit(1);
  }
  
  /* copy the buffer string into tmp_buf */
  size_t buflen = buffer.length();
  tmp_buf = (char *)malloc(buflen + 1);
  strcpy (tmp_buf, buffer.c_str());

  if(!(index = strstr(tmp_buf, "</html>"))){   //这样对吗？ 和google的respose结尾不符
    perror("Parse error, no end in messege body"); //这里假设headers之后html形式的messege body 以</html>结尾
    exit(1);
  }

  /* extract [response_line] */
  index = strstr(tmp_buf, "\r\n");
  len = index - tmp_buf;
  parse->response_line = string(tmp_buf, len);
  
  /* copy [response_line] into parse->buf for manipulation */
  parse->buflen = index - tmp_buf + 1;
  parse->buf = (char *)malloc(buflen);
  strcpy(parse->buf, (parse->response_line).c_str());
  
  char * curr;
  // extract [version] from parse->buf  
  int flag = 0; 
  first_line_extract(&(curr), parse, tmp_buf, flag);
  parse->version = string(curr);
  
  flag = 1;
  // extract [status_code] from parse->buf 
  first_line_extract(&(curr), parse, tmp_buf, flag);
  parse->status_code = string(curr);
  // extract [reason_phrase] from parse->buf
  first_line_extract(&(curr), parse, tmp_buf, flag);  
  parse->reason_phrase = string(curr);


  /* 接下来extract需要的header */
  // extract header [expires] from tmp_buf
  
  if (! header_extract(&curr, "Expires", parse, tmp_buf)) {
    parse->expires = string(curr);
    free(curr);
  }
  
  // extract header [cache_control] from tmp_buf
  if(! header_extract(&curr, "Cache-Control", parse, tmp_buf)) {   
    parse->cache_control = string(curr);
    free(curr);
  }
  
  // extract header [content_length] from tmp_buf 
  if (! header_extract(&curr, "Contentl-Length", parse, tmp_buf)) { 
    parse->content_length = string(curr);
    free(curr);
  }
  
  // extract header [e_tag] from tmp_buf 
  if (! header_extract(&curr, "E-Tag", parse, tmp_buf)) {   
    parse->e_tag = string(curr);
    free(curr);
  }
  
  if (! header_extract(&curr, "max_age", parse, tmp_buf)) {   
    parse->max_age = string(curr);
    free(curr);
  }  
  
  if (! header_extract(&curr, "Last_modified", parse, tmp_buf)) {   
    parse->last_modified = string(curr);
    free(curr);
  }
  
  if (! header_extract(&curr, "Date", parse, tmp_buf)) {   
    parse->date = string(curr);
    free(curr);
  }
  
  ////////////////////////////////////////////////////////   如何parse html messege ？？？

  free(tmp_buf);
  return 0;  
}

  /*
  parse->version = strtok(parse->buf, " ");
  if(parse->version == NULL){
    perror("Parse error, incorrect response line");
    free(parse->response_line);
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }
  */

  /*
    curr = strstr(tmp_buf, "Expires");
    if (curr != NULL) {
      curr = strstr(curr, ":") + 1;
      curr_end = strstr(curr, "\r\n");
      parse->expires = (char *)malloc(curr_end - curr + 1);
      memcpy(parse->expires, curr, curr_end - curr);
      parse->expires[curr_end - curr] = '\0';
      }
      else {
      printf("Response parse: response does not include header /Expires/\n");
      }
  */


























