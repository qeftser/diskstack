
#ifndef DISKSTACK

#define DISKSTACK
#define DISKSTACK_BUFFERSIZE 300
#define DISKSTACK_ELEMENTSIZE 25
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

enum diskstack_init_options { DS_CREAT, DS_ATCH };
enum diskstack_clean_options { DS_SILENT, DS_WARN };

struct diskstack { long int length; int bufferlen; char *path; 
                   char buffer[DISKSTACK_BUFFERSIZE*DISKSTACK_ELEMENTSIZE]; };
void init_diskstack(int, struct diskstack *);
char *pop_diskstack(char *buf, struct diskstack *);
void psh_diskstack(char *, struct diskstack *);
void print_diskstack(int pe, struct diskstack *);
void sync_global_diskstack(struct diskstack *);
void clean_diskstack(int, struct diskstack *);

#endif
