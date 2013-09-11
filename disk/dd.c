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
        ioctl(fd, BLKGETSIZE, &fd_blocks);
        close(fd);
        return fd_blocks * 512;
    }
    return 0; //implement me later
}

void copy_data(char * infile, char * outfile, long bytes)
{
    FILE * in =  fopen(infile, "rb");
    FILE * out = fopen(outfile, "wb"); //this will truncate it!
    short read = 0x0;
    
    while (in != NULL && (read = fgetc(in)) != EOF)
    {
        fputc(read, out);
    }
    
    fclose(in);
    fclose(out);
}

int main(int argc, char * * argv)
{
    ftype_t in_type, out_type;
    int in_index, out_index;
    long bytes = -1; //optional, <0=copy entire file
    int i;
    
    if (argc < 4)
    {
        printf("Usage: %s -i <infile> -o <outfile> [optional args]\n"
               "-i\tInput file\n"
               "-o\tOutput file\n"
               "-c\tNumber of bytes to copy\n", argv[0]);
        for (i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-i") == 0)
            {
                in_index = i + 1;
                i++;
            }
            if (strcmp(argv[i], "-o") == 0)
            {
                out_index = i + 1;
                i++;
            }
            if (strcmp(argv[i], "-c") == 0)
            {
                bytes = atol(argv[i + 1]);
                i++;
            }
        }
    }
    out_type = filetype(argv[out_index]);
    in_type = filetype(argv[in_index]);
    printf("Input\tType: %x\tSize: %llub\n", in_type, getsize(argv[in_index], in_type));
    printf("Outut\tType: %x\tSize: %llub\n", out_type, getsize(argv[out_index], out_type));
}
