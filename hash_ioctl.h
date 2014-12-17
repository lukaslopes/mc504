#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H
#include <linux/ioctl.h>
 
#define MAXSIZE 8
#define HASH_CODE 20

typedef struct
{
	char hash[MAXSIZE+1];
	int hash_match;
} hash_arg_t;
 
#define GET_HASH _IOR('h', 1, hash_arg_t *)
#define ERS_HASH _IO('h', 2)
#define SET_HASH _IOW('h', 3, hash_arg_t *)
#define CMP_HASH _IOWR('h', 4, hash_arg_t *)
 
#endif
