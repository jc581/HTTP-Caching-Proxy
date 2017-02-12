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
  
  if(parse->buf != NULL){
    perror("Parse error, already parsed");
    exit(1);
  }
  
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
  if(parse->protocol == NULL){
    perror("Parse error, incorrect protocol");
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }
  /* parse host and path */
  parse->host = strtok(NULL, "/");
  if(parse->host == NULL){
    perror("Parse error, incorrect host");
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }
  
  parse->path = strtok(NULL, " ");
  //printf("%s\n", parse->path);
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
 
  char *currentHeader = strstr(tmp_buf, "\r\n") + 2;
  index = strstr(currentHeader, "\r\n");
  parse->buf = (char *)malloc(index - currentHeader + 1);
  parse->buflen = index - currentHeader + 1;
  memcpy(parse->buf, currentHeader, index - currentHeader);
  parse->buf[index - currentHeader] = '\0';
  /* parse host and port */
  parse->port = strtok(parse->buf, ":");
  parse->port = strtok(NULL, ":");
  parse->port = strtok(NULL, ":");
  if(parse->port == NULL){
    parse->port = "80";
  }
  free(tmp_buf);

  return 0;
}
