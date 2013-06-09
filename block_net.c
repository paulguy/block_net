#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#define __USE_GNU
#include <dlfcn.h>

#include <netinet/in.h>

#ifndef LIBDL_PATH
#define LIBDL_PATH "/lib/libdl.so.2"
#endif

#ifndef LIBC_PATH
#define LIBC_PATH "/lib/libc.so.6"
#endif

#ifdef __LP64__
#define GLIBC_VER	"GLIBC_2.2.5"
#define PTR_INT		unsigned long long int
#define PTR_PRINTF	"%llX"
#else /* 32bit */
#define GLIBC_VER	"GLIBC_2.0"
#define PTR_INT		unsigned int
#define PTR_PRINTF	"%X"
#endif

/* This definition is required to get dlvsym(). */
#define _GNU_SOURCE

static void *(*dlsym_real)(void *handle, const char *symbol) = NULL;
static int (*connect_real)(int, const struct sockaddr*, socklen_t) = NULL;
static int silent = 2;
static void *libdl, *libc;

void *dlsym(void *handle, const char *symbol) {
	if(silent == 2) {
		if(getenv("BN_SILENT"))
			silent = 0;
		else
			silent = 1;
	}

	if(!dlsym_real) {
		if(getenv("BN_NODEFAULT")) {
			if(getenv("BN_LIBDL_PATH"))
				libdl = dlopen(getenv("BN_LIBDL_PATH"), RTLD_LAZY);
			else
				libdl = dlopen(LIBDL_PATH, RTLD_LAZY);
			if(libdl) {
				dlsym_real = dlvsym(libdl, "dlsym", GLIBC_VER);
				if(!dlsym_real) {
					if(silent != 0) {
						if(getenv("BN_LIBDL_PATH"))
							fprintf(stderr, "Failed to load real dlsym() from %s, the application will probably fail.\n",
							        getenv("BN_LIBDL_PATH"));
						else
							fprintf(stderr, "Failed to load real dlsym() from " LIBDL_PATH ", the application will probably fail.\n");
					}
					return(NULL);
				}
			} else {
				if(silent != 0) {
					if(getenv("BN_LIBDL_PATH"))
						fprintf(stderr, "Failed to open %s, the application will probably fail.\n", getenv("BN_LIBDL_PATH"));
					else
						fprintf(stderr, "Failed to open " LIBDL_PATH ", the application will probably fail\n");
				}
			}
		} else {
			dlsym_real = dlvsym(RTLD_DEFAULT, "dlsym", GLIBC_VER);
			if(!dlsym_real) {
				if(silent != 0)
					fprintf(stderr, "Failed to find real dlsym(), the application will probably fail.\n");
				return(NULL);
			}
		}
	}

	if(!strncmp(symbol, "connect", 7))
		return((void *)connect);

	return(dlsym_real(handle, symbol));
}

int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen) {
	if(silent == 2) {
		if(getenv("BN_SILENT"))
			silent = 0;
		else
			silent = 1;
	}

	if (!connect_real) {
		if(getenv("BN_NODEFAULT")) {
			if(getenv("BN_LIBC_PATH"))
				libc = dlopen(getenv("BN_LIBC_PATH"), RTLD_LAZY);
			else
				libc = dlopen(LIBC_PATH, RTLD_LAZY);
			if(libdl) {
				connect_real = dlvsym(libc, "connect", GLIBC_VER);
				if(!connect_real) {
					if(silent != 0) {
						if(getenv("BN_LIBC_PATH"))
							fprintf(stderr, "Failed to load real connect() from %s, the application will probably fail.\n",
							        getenv("BN_LIBC_PATH"));
						else
							fprintf(stderr, "Failed to load real connect() from " LIBC_PATH ", the application will probably fail.\n");
					}
					errno = EACCES;
					return(-1);
				}
			} else {
				if(silent != 0) {
					if(getenv("BN_LIBC_PATH"))
						fprintf(stderr, "Failed to open %s, the application will probably fail.\n", getenv("BN_LIBC_PATH"));
					else
						fprintf(stderr, "Failed to open " LIBC_PATH ", the application will probably fail\n");
				}
			}
		} else {
			connect_real = dlvsym(RTLD_DEFAULT, "connect", GLIBC_VER);
			if(!connect_real) {
				if(silent == 0)
					fprintf(stderr, "Failed to find real connect(), the application will be unable to connect any sockets.\n");
				errno = EACCES;
				return(-1);
			}
		}
	}

	if(!serv_addr) {
		if(silent == 0)
			fprintf(stderr, "serv_addr is NULL?  Passing to connect() anyway.\n");
		return connect_real(sockfd, serv_addr, addrlen);
	}

	if(serv_addr->sa_family != AF_INET) {
		return connect_real(sockfd,serv_addr,addrlen);
	}

	// Allow all contacts with localhost
	if((((struct sockaddr_in *)serv_addr)->sin_addr.s_addr & 0x000000FF) == 0x0000007F)
		return connect_real(sockfd,serv_addr,addrlen);

	if(silent == 0)
		fprintf(stderr,"connect() denied to address %X\n", ((struct sockaddr_in *)serv_addr)->sin_addr.s_addr);

	errno = EACCES;
	return(-1);
}
