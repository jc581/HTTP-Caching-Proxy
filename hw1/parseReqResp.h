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
  /* parse the path */
  parse->path = strtok(NULL, " ");
  if(parse->path == NULL){
    perror("Parse error, incorrect request line");
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }
  /* parse the version of HTTP */
  parse->version = strtok(NULL, " ");
  if(parse->version == NULL){
    perror("Parse error, incorrect request line");
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }
  
  char *currentHeader = strstr(tmp_buf, "\r\n") + 2;
  index = strstr(currentHeader, "\r\n");
  parse->buf = (char *)malloc(index - currentHeader + 1);
  parse->buflen = index - currentHeader + 1;
  memcpy(parse->buf, currentHeader, index - currentHeader);
  parse->buf[index - currentHeader] = '\0';
  /* parse host and port */
  parse->host = strtok(parse->buf, ":");
  parse->host = strtok(NULL, ":");
  if(parse->host == NULL){
    perror("Parse error, incorrect header file");
    free(tmp_buf);
    free(parse->buf);
    exit(1);
  }
  parse->port = strtok(NULL, ":");
  if(parse->port == NULL){
    parse->port = "80";
  }
  free(tmp_buf);

  return 0;
}
