#ifndef __PUB_H__
#define __PUB_H__
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <ctype.h>

char *get_mime_type(char *name);

int get_line(int sock, char *buf, int size);

int hexit(char c);//16进制转10进制

void strencode(char* to, size_t tosize, const char* from);//编码

void strdecode(char *to, char *from);//解码

#endif