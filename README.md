# diskstack
## Overview   
This is a conventional stack datastructure that additionally writes into a file as the stack fills to allow it to handle large numbers of entries without blowing the heap or stack or wherever it would normally be allocated.   
## Usage
Create a new diskstack with:   
```
struct diskstack my_diskstack;
   
my_diskstack.path = "/path/to/store/diskstack/file";
   
init_diskstack(DS_CREAT,&d); /* create a new file regardless of if one already existed */   
   
init_diskstack(DS_ATCH,&d); /* use an existing file for the diskstack, fail if no file exists */
   
init_diskstack(DS_CREAT|DS_ATCH,&d) /* only create a new file for a diskstack if no file previously existed */
```
Add items with:   
```
psh_diskstack("new item", &my_diskstack);
```
Remove items with:
```
char[100] item;
pop_diskstack(item, &my_diskstack);
```
Dump any cached items to the file with:   
```
sync_global_diskstack(&my_diskstack); /* you should do this before exiting the program to ensure all elements are saved on the disk */
```
Clean up and remove the diskstack file with:   
```
clean_diskstack(DS_SILENT,&my_diskstack); /* executes with no warning */
   
clean_diskstack(DS_WARN,&my_diskstack); /* prompts the user to confirm before removing diskstack */
```
Print diskstack with:   
```
print_diskstack(0,&my_diskstack); /* print number of elements in buffer and in file */
   
print_diskstack(1,&my_diskstack); /* same as above, but also print all elements currently in the buffer */
```
## Performance
I was getting ~5 seconds for 100,000,000 push operations and ~3 seconds for the same number of pops. I would say that is decently fast. These stats are from running the main.c function in src.
