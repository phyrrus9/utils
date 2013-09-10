#include <fcntl.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <linux/fs.h>
#else
#include <sys/disk.h>
#include <stdlib.h>
#define BLKGETSIZE DKIOCGETBLOCKCOUNT
#endif

#define UNKNO 0X0 /*cant determine type*/
#define BLOCK 0x1 /*block device    S_ISBLK*/
#define CHR   0x2 /*character file  S_ISCHR*/
#define REGL  0x3 /*regular file    S_ISREG*/
#define DIR   0x4 /*directory       S_ISDIR*/
#define NOPRM 0x5 /*no permission   S_?????*/
#define LINK  0x6 /*symbolic link   S_ISLNK*/

typedef char ftype_t;

ftype_t filetype(char * filename)
{
    struct stat typeinfo;
    mode_t mode;
    stat((const char *)filename, &typeinfo);
    mode = typeinfo.st_mode;
    if (S_ISBLK(mode))
        return BLOCK;
    else if (S_ISCHR(mode))
        return CHR;
    else if (S_ISREG(mode))
        return REGL;
    else if (S_ISDIR(mode))
        return DIR;
    else if (S_ISLNK(mode))
        return LINK;
    return mode;
}

unsigned long long getsize(char * filename, ftype_t ftype)
{
    int fd; //used in checking block and character devices
    unsigned long long fd_blocks;
    FILE *in; //used in checking regular files
    char in_tmp; //used with FILE *in
    unsigned long long ret;
    
    if (ftype == REGL)
    {
        if ((in = fopen(filename, "rb")) == NULL)
            return 0LL;
        fseek(in, 0L, SEEK_END);
        ret = ftell(in);
        fclose(in);
        return ret;
    }
    
    if (ftype == BLOCK)
    {
        fd = open(filename, O_RDONLY);
        ioctl(fd, BLKGETSIZE, &blocks);
        close(fd);
        return blocks * 512;
    }
    return 0; //implement me later
}

int main(int argc, char * * argv)
{
    ftype_t a = filetype(argv[1]);
    printf("Type: %x\tSize: %llub\n", a, getsize(argv[1], a));
}
