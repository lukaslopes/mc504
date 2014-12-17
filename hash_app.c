#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
 
#include "hash_ioctl.h"

void myhash(char *h, const char *str) {
	char *s = h;
	int i = 0;
    while (str[i] != '\0'){
		s[i] = str[i] + HASH_CODE;
		i++;
	}
}
 
void get_vars(int fd)
{
    hash_arg_t h;
 
    if (ioctl(fd, GET_HASH, &h) == -1)
    {
        perror("hash_apps ioctl get");
    }
    else
    {
		if (strcmp(h.hash, "") == 0)        
			printf("Empty Hash\n");
		else
			printf("Hash Password: %s\n", h.hash);
    }
}
void ers_vars(int fd)
{
	if (ioctl(fd, ERS_HASH) == -1)
	{
		perror("hash_apps ioctl erase");
	}
	else{
		printf("Hash Erased\n");
	}
}
void set_vars(int fd)
{
    hash_arg_t h;
 
    printf("Enter Password: ");
    scanf("%s", &h.hash);
 
    if (ioctl(fd, SET_HASH, &h) == -1)
    {
        perror("hash_apps ioctl set");
    }
}

void cmp_vars(int fd)
{
    hash_arg_t h;
	char newhash[MAXSIZE] = ""; 
	char pass[MAXSIZE] = ""; 
    
	printf("Enter Password: ");
    scanf("%s", pass);

	myhash(newhash, pass);
	newhash[8] = '\0';

	if (ioctl(fd, GET_HASH, &h) == -1)
    {
        perror("hash_apps ioctl get");
    }
    else
    {
		if (strcmp(h.hash, newhash) == 0)        
			printf("Password Matched with Hash\n");
		else
			printf("Password differs from hash\n");
    }

}
 
int main(int argc, char *argv[])
{
    char *file_name = "/dev/hash";
    int fd;
    enum
    {
        e_get,
        e_ers,
        e_set,
		e_cmp
    } option;
 
    if (argc == 1)
    {
        option = e_get;
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-g") == 0)
        {
            option = e_get;
        }
        else if (strcmp(argv[1], "-e") == 0)
        {
            option = e_ers;
        }
        else if (strcmp(argv[1], "-s") == 0)
        {
            option = e_set;
        }
		else if (strcmp(argv[1], "-c") == 0)
        {
            option = e_cmp;
        }
        else
        {
            fprintf(stderr, "Usage: %s [-g | -e | -c | -s]\n", argv[0]);
            return 1;
        }
    }
    else
    {
		fprintf(stderr, "Usage: %s [-g | -e | -c | -s]\n", argv[0]);
        return 1;
    }
    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("hash_apps open");
        return 2;
    }
 
    switch (option)
    {
        case e_get:
            get_vars(fd);
            break;
        case e_ers:
            ers_vars(fd);
            break;
        case e_set:
            set_vars(fd);
            break;
		case e_cmp:
            cmp_vars(fd);
            break;
        default:
            break;
    }
 
    close (fd);
 
    return 0;
}
