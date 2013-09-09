/* Disk wiper/zero utility -- Copyright © 2013 phyrrus9 */
#include <fcntl.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#ifndef __APPLE__
#include <linux/fs.h>
#else
#include <sys/disk.h>
#include <stdlib.h>
#define BLKGETSIZE DKIOCGETBLOCKCOUNT
#endif

void write_zero(char * file, unsigned long bytes)
{
	printf("Zeroing %s\n", file);
	unsigned long wrote = 0, total = 0;
	int fd, i, buf;
	unsigned long long obj = 0;
	FILE *out = fopen(file, "wb");
	unsigned long start, curr, written = 0;
	unsigned long o_wrote;
	double bps = 0, tot_time = 0, rem_time = 0;
	char first = 1;

	start = time(0);
	curr = start;
	while (wrote < bytes)
	{
		o_wrote = fwrite(&obj, sizeof(unsigned long long), 1, out);
		written += o_wrote;
		wrote += o_wrote;
		if ((time(0) - curr) > 9 || (first == 1 && (time(0) - curr) > 1)) //10 sec
		{
			first = 0;
			bps = (double)(time(0) - curr) / (double)written;
			written = 0;
			curr = time(0);
		}
		if (wrote % ((1024 * 1024)) == 0) //every 5MB
		{
			printf("\r%101s\r[", " "); //blank it
			total = ((double)((double)wrote / (double)bytes) * 100);
			for (i = 0; i < 100; i++)
			{
				if (i <= total)
				{
					printf("=");
				}
				else
				{
					printf(" ");
				}
			}
			printf("] %.2f%% ", ((double)wrote/(double)bytes * 100));
			rem_time = bps * (bytes - wrote);
			printf("time: %02.0f:%02.0f", (rem_time / 60), (double)((int)rem_time % 60));
			fflush(stdout);
		}
	}
	fflush(out);
	fclose(out);
	printf("\nDone!\n");
}

int main(int argc, char * * argv)
{
    int fd;
    unsigned long blocks = 0;
    char check = 0x0;
    
    printf("==============================================\n"
           "=Phyrrus9's drive eraser. Copyright (c) 2013 =\n"
           "=Will erase and write zeros over entire block=\n"
           "==============================================\n");
    
    if (argc < 2)
    {
        printf("Args: %s <device block>\nDevice in /dev/xxx form\n", argv[0]);
        return 1;
    }
    if (getuid() != 0)
    {
        printf("This program must be run as root!\n");
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    ioctl(fd, BLKGETSIZE, &blocks);
    close(fd);

    printf("Blocks: %lu\tBytes: %lu\tGB: %.2f\n",
                blocks, blocks * 512, (double)blocks * 512.0 / (1024 * 1024 * 1024));
    do
    {
        printf("Write 0x0 to %s? [y/N] ", argv[1]);
        fflush(stdout);
    }
    while (scanf("%c", &check) < 1);
    if (check == 'y')
    {
        write_zero(argv[1], blocks * 512);
    }
}
