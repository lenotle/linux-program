#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>


int main(int argc, const char **argv)
{
	DIR *dir = NULL;
	struct dirent *d = NULL;

	if (argc != 2)
	{
		perror("usage: not filname..");
		return 1;
	}

	// open dir 
	if ((dir = opendir(argv[1])) == NULL)
	{
		perror("opendir");
		return 1;
	}

	// read dir 
	// struct dirent {
        //       ino_t          d_ino;       /* Inode number */
        //       off_t          d_off;       /* Not an offset; see below */
        //       unsigned short d_reclen;    /* Length of this record */
        //       unsigned char  d_type;      /* Type of file; not supported
        //                                      by all filesystem types */
        //       char           d_name[256]; /* Null-terminated filename */
        //};
	
	while(1)
	{
		if ((d = readdir(dir)) == NULL)
		{
			break;
		}

		printf("filename is: %s\t", d->d_name);
		switch(d->d_type)
		{
			case DT_BLK:
				printf("This is a block device");
				break;
			case DT_CHR:
				printf("This is a character device");
				break;
			case DT_DIR:
				printf("This is a directory");
				break;
			case DT_FIFO:
				printf("This is a name pipe");
				break;
			case DT_LNK:
				printf("This is a symbolic link");
				break;
			case DT_REG:
				printf("This is a regular file");
				break;
			case DT_SOCK:
				printf("This is a UNIX domain socket");
				break;
			default:
				printf("The file tpe could not be determined");	
	
		}
		putchar('\n');
	}

	// close dir
	closedir(dir);
	return 0;
}
