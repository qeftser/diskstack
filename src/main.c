
#include "diskstack.h"

int main(void) {

   char str[30];
   int count = 100000000;
;

   struct diskstack d;
   d.path = "/home/qeftser/projects/diskstack/bin/stack.d";
   init_diskstack(DS_CREAT|DS_ATCH,&d);

   clock_t tstart = clock();

   for (int i = 0; i < count; i++) {
      sprintf(str,"hello #%d",i);
      psh_diskstack(str, &d);
   }

   printf("MARK ADD: %f\n", (double)(clock() - tstart) / CLOCKS_PER_SEC);
   tstart = clock();

   for (int i = 0; i < count; i++) {
      pop_diskstack(str,&d);
   }

   printf("MARK REM: %f\n", (double)(clock() - tstart) / CLOCKS_PER_SEC);

   sync_global_diskstack(&d);
   clean_diskstack(DS_SILENT,&d);

   return 0;
}
