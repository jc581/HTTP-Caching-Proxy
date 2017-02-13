#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_LENGTH 65535

/* struct that contains key information in request */
struct ParsedRequest_t{
  char *method;
  char *host;
  char *port;
  char *path;
  char *version;
  char *protocol;
  char *buf;
  size_t buflen;
};
typedef struct ParsedRequest_t ParsedRequest;

int ParseRequest(ParsedRequest *parse, const char *buf, size_t buflen){
  char *index;
  char *tmp_buf;
  
  tmp_buf = (char *)malloc(buflen + 1);
  /* copy the buffer string into temp buffer */ 
  memcpy(tmp_buf, buf, buflen);
  tmp_buf[buflen] = '\0';
 
  if(!(index = strstr(tmp_buf, "\r\n\r\n"))){
    perror("Parse error, no end in header");
    exit(1);
  }
  /* copy the request line into parse->buf */
  index = strstr(tmp_buf, "\r\n");
  parse->buf = (char *)malloc(index - tmp_buf + 1);
  parse->buflen = index - tmp_buf + 1;
  memcpy(parse->buf, tmp_buf, index - tmp_buf);
  parse->buf[index - tmp_buf] = '\0';
  /* parse the method */
  parse->method = strtok(parse->buf, " ");
  if(parse->method == NULL){
    perror("Parse error, incorrect request line");
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }
  /* parse the version of HTTP */
  char *full_addr = strtok(NULL, " ");
  parse->version = full_addr + strlen(full_addr) + 1;
  /* parse protocol */
  parse->protocol = strtok(full_addr, "://");
  if(parse->protocol[0] == 'h'|| parse->protocol[1] == 't'){
    parse->host = strtok(NULL, " ");
    parse->host += 2;
    parse->host = strtok(parse->host, ":");
  }else{   
  /* parse host and path */
    parse->port = strtok(NULL, ":");
    parse->host = strtok(full_addr, " ");
  }
  if(parse->port == NULL){
    parse->port = "80";
  }
  char *tmp_host = strtok(parse->host, "/");
  if(tmp_host == NULL){
    parse->host = strtok(parse->host, ":");
  }
  else{
    parse->path = strtok(NULL, "/");
  }
  if(parse->path == NULL){
    size_t rlen = strlen("/");
    parse->path = (char*)malloc(rlen + 1);
    memcpy(parse->path, "/", rlen);
    parse->path[rlen] = '\0'; 
  }else{
    size_t rlen = strlen("/");
    size_t plen = strlen(parse->path);
    char *tmp = parse->path;
    parse->path = (char*)malloc(rlen + plen + 1);
    memcpy(parse->path, "/", rlen);
    memcpy(parse->path + rlen, tmp, plen);
    parse->path[rlen + plen] = '\0';
  }  
  //char *currentHeader = strstr(tmp_buf, "\r\n") + 2;
  //index = strstr(currentHeader, "\r\n");
  //parse->buf = (char *)malloc(index - currentHeader + 1);
  //parse->buflen = index - currentHeader + 1;
  //memcpy(parse->buf, currentHeader, index - currentHeader);
  //parse->buf[index - currentHeader] = '\0';
  //parse->port = strtok(NULL, ":");
  //parse->port = strtok(NULL, ":");
 
  free(tmp_buf);
  
  return 0;
}

/*--------------------------------------------------------------------------------------------------------------------------*/
/* struct that contains information needed for cache */
struct ParsedResponse_t{
  char * version;
  char * status_code;
  char * reason_phrase;

  char * response_line;
  char * expires;
  char * cache_control;
  char * content_length;
  char * e_tag;

  char * buf;
  size_t buflen;
};
typedef struct ParsedResponse_t ParsedResponse;

int ParseResponse(ParsedResponse *parse, const char *buf, size_t buflen) {
  char *index;
  char *tmp_buf;

  if(parse->buf != NULL){
    perror("Parse error, already parsed");
    exit(1);
  }

  tmp_buf = (char *)malloc(buflen + 1);
  /* copy the buffer string into tmp_buf */
  memcpy(tmp_buf, buf, buflen);
  tmp_buf[buflen] = '\0';

  if(!(index = strstr(tmp_buf, "</html>"))){   //这样对吗？ 和google的respose结尾不符
    perror("Parse error, no end in messege body"); //这里假设headers之后html形式的messege body 以</html>结尾
    exit(1);
  }

  /* extract [response_line] */
  index = strstr(tmp_buf, "\r\n");
  parse->response_line = (char *)malloc(index - tmp_buf + 1);
  memcpy(parse->response_line, tmp_buf, index - tmp_buf);
  parse->response_line[index - tmp_buf] = '\0';

  /* copy [response_line] into parse->buf for manipulation*/
  parse->buflen = index - tmp_buf + 1;
  parse->buf = (char *)malloc(buflen);
  memcpy(parse->buf, parse->response_line, parse->buflen);

  /* extract [version] from parse->buf */
  parse->version = strtok(parse->buf, " ");
  if(parse->version == NULL){
    perror("Parse error, incorrect response line");
    free(parse->response_line);
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }

  /* extract [status_code] from parse->buf */
  parse->status_code = strtok(NULL, " ");
  if(parse->status_code == NULL){
    perror("Parse error, incorrect response line");
    free(parse->response_line);
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }

  /* extract [reason_phrase] from parse->buf*/
  parse->reason_phrase = strtok(NULL, " ");
  if(parse->reason_phrase == NULL){
    perror("Parse error, incorrect response line");
    free(parse->response_line);
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }

  //   接下来extract需要的header
  char * curr = NULL;
  char * curr_end = NULL;
  /* extract header [expires] from tmp_buf */
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

  /* extract header [cache_control] from tmp_buf */
  curr = strstr(tmp_buf, "Cache-Control");
  if (curr != NULL) {
    curr = strstr(curr, ":") + 1;
    curr_end = strstr(curr, "\r\n");
    parse->cache_control = (char *)malloc(curr_end - curr + 1);
    memcpy(parse->cache_control, curr, curr_end - curr);
    parse->cache_control[curr_end - curr] = '\0';
  }
  else {
    printf("Response parse: response does not include header /Cache-Control/\n");
  }


  /* extract header [content_length] from tmp_buf */
  curr = strstr(tmp_buf, "Contentl-Length");
  if (curr != NULL) {
    curr = strstr(curr, ":") + 1;
    curr_end = strstr(curr, "\r\n");
    parse->content_length = (char *)malloc(curr_end - curr + 1);
    memcpy(parse->content_length, curr, curr_end - curr);
    parse->content_length[curr_end - curr] = '\0';
  }
  else {
    printf("Response parse: response does not include header /Contentl-Length/\n");
  }

  /* extract header [e_tag] from tmp_buf */
  curr = strstr(tmp_buf, "E-Tag");
  if (curr != NULL) {
    curr = strstr(curr, ":") + 1;
    curr_end = strstr(curr, "\r\n");
    parse->e_tag = (char *)malloc(curr_end - curr + 1);
    memcpy(parse->e_tag, curr, curr_end - curr);
    parse->e_tag[curr_end - curr] = '\0';
  }
  else {
    printf("Response parse: response does not include header /E-Tag/\n");
  }

  //   如何parse html messege ？？？
  return 0;
}
