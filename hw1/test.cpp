#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char **argv){
  char *buf = "HelloWorld";
  char *buffer = "Hi";
  char *x;

  string str;
  str.append(buffer);
  str.append(buf);
  x = &(str[0]);

  cout << str << endl;
  printf("%s\n", x);
  
  return 0;
}
