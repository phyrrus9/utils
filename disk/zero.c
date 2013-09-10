/* Disk wiper/zero utility -- Copyright © 2013 phyrrus9 */
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

#define pat0 0x0000000000000000 /*0x0--*/
#define pat1 0x0101010101010101 /*pattern 1*/
#define pat2 0x1010101010101010 /*pattern 2*/
#define pat3 0x1100110011001100 /*0x1100*/
#define pat4 0x0011001100110011 /*0x0011*/
#define pat5 0x1101001011010010 /*0x1101 0010*/
#define pat6 0x1111111111111111 /*0x1--*/
#define pat7 0x0123456789012345 /*numeric*/
#define pat8 0xABCDEFABCDEFABCD /*alpha*/

FILE *out;

void sighandler(int sig)
{
    printf("\nSignal caught! Oh no! Shutting down\n");
    if (out != NULL)
        fclose(out);
    exit(2);
}

void write_zero(char * file, unsigned long bytes, int run, unsigned long long obj)
{
    signal(SIGINT, sighandler);
	unsigned long wrote = 0, total = 0;
	int fd, i, buf;
	unsigned long start, curr, written = 0;
	unsigned long o_wrote;
	double bps = 0, tot_time = 0, rem_time = 0;
	char first = 1;
    int hours = 0, mins = 0, secs = 0,remainder = 0;

    printf("Writing 0x%llx to %s (pass #%d)\n", obj, file, run);
    out = fopen(file, "wb");
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
            hours = rem_time / 3600;
            remainder = (int)rem_time % 3600;
            mins = remainder / 60;
            secs = remainder % 60;
            if (hours < 1)
                printf("ETA: %02d:%02d", mins, secs);
            else if (hours < 24)
                printf("ETA: %02d:%02d:%02d", hours, mins, secs);
            else
                printf("no eta (too long)");
			fflush(stdout);
		}
	}
	fflush(out);
	fclose(out);
	printf("\n");
}

void set_pattern(int pat, unsigned long long * obj)
{
    int i;
    switch (pat)
    {
        case 0:
            *obj = pat0;
            break;
        case 1:
            *obj = pat1;
            break;
        case 2:
            *obj = pat2;
            break;
        case 3:
            *obj = pat3;
            break;
        case 4:
            *obj = pat4;
            break;
        case 5:
            *obj = pat5;
            break;
        case 6:
            *obj = pat6;
            break;
        case 7:
            *obj = pat7;
            break;
        case 8:
            *obj = pat8;
            break;
    }
}

void print_patterns(void)
{
    printf("#define pat0 0x0000000000000000\n"
           "#define pat1 0x0101010101010101\n"
           "#define pat2 0x1010101010101010\n" /*pattern 2*/
           "#define pat3 0x1100110011001100\n" /*0x1100*/
           "#define pat4 0x0011001100110011\n" /*0x0011*/
           "#define pat5 0x1101001011010010\n" /*0x1101 0010*/
           "#define pat6 0x1111111111111111\n" /*0x1--*/
           "#define pat7 0x0123456789012345\n" /*numeric*/
           "#define pat8 0xABCDEFABCDEFABCD\n" /*alpha*/);
}

int main(int argc, char * * argv)
{
    signal(SIGINT, sighandler);
    int fd;
    unsigned long blocks = 0;
    char check = 0x0;
    int nruns = 1, i, pat;
    char loop = 0x0;
    unsigned long long obj = 0x0;
    
    printf("==============================================\n"
           "=Phyrrus9's drive eraser. Copyright (c) 2013 =\n"
           "=Will erase and write zeros over entire block=\n"
           "==============================================\n");
    
    if (argc < 2)
    {
    hlp:
        printf("Args: %s <device block> [opts]\n"
               "Device in /dev/xxx form\n"
               "-h\tPrints this message\n"
               "-hp\tPrint patterns\n"
               "-c\tNumber of passes\n"
               "-p\tPattern select\n"
               "-l\tLoop through patterns\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "-hp") == 0)
    {
        print_patterns();
        return 0;
    }
    if (strcmp(argv[1], "-h") == 0)
        goto hlp;
    if (getuid() != 0)
    {
        printf("This program must be run as root!\n");
        return 1;
    }
    
    if (argc > 2)
    {
        for (i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "-c") == 0)
            {
                nruns = atoi(argv[i + 1]);
                i++;
            }
            if (strcmp(argv[i], "-p") == 0)
            {
                pat = atoi(argv[i + 1]);
                i++;
            }
            if (strcmp(argv[i], "-l") == 0)
            {
                loop = 0x1;
            }
        }
    }

    fd = open(argv[1], O_RDONLY);
    ioctl(fd, BLKGETSIZE, &blocks);
    close(fd);

    set_pattern(pat, &obj);
    printf("Blocks: %lu\tBytes: %lu\tGB: %.2f\n",
                blocks, blocks * 512, (double)blocks * 512.0 / (1024 * 1024 * 1024));
    do
    {
        printf("Write 0x%llx to %s with %d passes? [y/N] ", obj, argv[1], nruns);
        fflush(stdout);
    }
    while (scanf("%c", &check) < 1);
    if (check == 'y')
    {
        for (i = 0; i < nruns; i++)
        {
            set_pattern(pat, &obj);
            write_zero(argv[1], blocks * 512, i, obj);
            if (loop == 0x1)
            {
                if (pat < 8)
                    pat++;
                else
                    pat = 0;
            }
        }
    }
    printf("Done!\n");
}
