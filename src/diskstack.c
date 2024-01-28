
#include "diskstack.h"

/* private functions */
int load_buffer_diskstack(struct diskstack *);
int unload_buffer_diskstack(int, struct diskstack *);

void init_diskstack(int opts, struct diskstack *d) {
   if (opts & 1 && 0 == access(d->path,F_OK)) {
      int fptr = open(d->path,O_RDONLY);
      struct stat fst;
      if (-1 == fstat(fptr,&fst)) {
         printf("failed to stat file\n");
         perror(NULL); exit(1);
      }
      d->length = fst.st_size/DISKSTACK_ELEMENTSIZE;
      close(fptr);
   }
   else {
      creat(d->path,PROT_READ|PROT_WRITE);
      chmod(d->path,S_IWUSR|S_IRUSR|S_IWGRP|S_IRGRP|S_IROTH|S_IWOTH);
      d->length = 0;
   }
   d->bufferlen = 0;
}

void clean_diskstack(int opts, struct diskstack *d) {
   char c;
   if (opts) {
      printf("DISKSTACK: %s\n",d->path);
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!! PROGRAM HAS REQUESTED DELETION OF STACK !!!\n");
      printf("!!! PLEASE CONFIRM THIS ACTION              !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
request:
      {
         printf("!!! [ y / n ] >>> ");
         c = getchar();
         if (c == '\n') goto request;
         if (c == 'n') return;
         if (c != 'y') {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            printf("!!! BAD INPUT. ENTER ONE OF:  y  |  n       !!!\n");
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            goto request;
         }
      }
   }
   char cmd[strlen(d->path)+4];
   strcpy(cmd,"rm "); strcat(cmd,d->path);
   int res = system(cmd);
   if (!WIFEXITED(res)) {
      printf("clean command failed\n");
      perror(NULL);
   }
}

void sync_global_diskstack(struct diskstack *d) {
   unload_buffer_diskstack(d->bufferlen,d);
}

int load_buffer_diskstack(struct diskstack *d) {
   int grabnum = DISKSTACK_BUFFERSIZE - d->bufferlen;
   if (grabnum > d->length) grabnum = d->length;
   int fptr = open(d->path,O_RDWR);
   lseek(fptr,(d->length-grabnum)*DISKSTACK_ELEMENTSIZE,SEEK_SET);
   if (-1 == read(fptr,d->buffer+(d->bufferlen*DISKSTACK_ELEMENTSIZE),grabnum*DISKSTACK_ELEMENTSIZE)) {
      printf("failed reading in data\n");
      perror(NULL); exit(1);
   }
   lseek(fptr,0,SEEK_SET);
   if (-1 == ftruncate(fptr,(d->length-grabnum)*DISKSTACK_ELEMENTSIZE)) {
      printf("failed to truncate file\n");
      perror(NULL); exit(1);
   }
   close(fptr);
   return grabnum;
}

int unload_buffer_diskstack(int num, struct diskstack *d) {
   if (num > d->bufferlen) num = d->bufferlen;
   int fptr = open(d->path,O_RDWR);
   lseek(fptr,0,SEEK_END);
   if (-1 == write(fptr,d->buffer+((d->bufferlen-num)*DISKSTACK_ELEMENTSIZE),num*DISKSTACK_ELEMENTSIZE)) {
      printf("failed to write data\n");
      perror(NULL); exit(1);
   }
   close(fptr);
   return num;
}

void psh_diskstack(char *v, struct diskstack *d) {
   if (d->bufferlen == DISKSTACK_BUFFERSIZE) {
      int dumped = unload_buffer_diskstack(DISKSTACK_BUFFERSIZE,d);
      d->bufferlen -= dumped;
      d->length += dumped;
   }
   memset(d->buffer+((d->bufferlen)*DISKSTACK_ELEMENTSIZE),3,DISKSTACK_ELEMENTSIZE);
   int pos = 0;
   while (*(v+pos) != '\0') {
      d->buffer[(d->bufferlen)*DISKSTACK_ELEMENTSIZE+pos] = *(v+pos);
      pos++;
   }
   d->bufferlen++;
}

char *pop_diskstack(char *buf, struct diskstack *d) {
   if (d->bufferlen == 0) {
      if (d->length == 0) { *buf = '\0'; return buf; }
      int grabbed = load_buffer_diskstack(d);
      d->length -= grabbed;
      if (!d->length) d->length = 0;
      d->bufferlen += grabbed;
   }
   d->bufferlen--;
   char c;
   for (int i = 0; i < 100; i++) {
      c = d->buffer[d->bufferlen*DISKSTACK_ELEMENTSIZE+i];
      if (c == 3) { buf[i] = '\0'; break; }
      buf[i] = c;
   }
   return buf;
}

void print_diskstack(int pe, struct diskstack *d) {
   printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   printf("DISKSTACK\n");
   printf("IN BUFFER: %d\n", d->bufferlen);
   printf("IN FILE:   %ld\n", d->length);
   printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   if (pe) {
      printf("ELEMENTS:\n");
      for (int i = 0; i < d->bufferlen; i++) {
         for (int j = 0; j != DISKSTACK_ELEMENTSIZE; j++) {
            putchar(d->buffer[i*DISKSTACK_ELEMENTSIZE+j]);
         }
         printf("\n");
      }
   }
}

