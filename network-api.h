#ifndef _NETWORK_API_H_
#define _NETWORK_API_H_

// Note: this header file must be put below all other included files
// as it redefines some macros that may be defined in other header files. 
// This also implies that network-api.h shouldn't be included in a header file
// just .c files.

#if HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef USE_FSTACK

#include <stddef.h>
#include <stdint.h>

#include <ff_config.h>
#include <ff_api.h>

#define Ioctl ff_ioctl
#define Fcntl ff_fcntl
#define Socket ff_socket
#define Setsockopt ff_setsockopt
#define Getsockopt ff_getsockopt
#define Send ff_send
#define Recv ff_recv
#define Recvfrom ff_recvfrom
#define Bind ff_bind
#define Connect ff_connect
#define Poll ff_poll

#define Sockaddr linux_sockaddr

#ifdef __linux__

// re-define macro values that are inconsistent in Linux and FreeBSD, 
// as f-stack uses FreeBSD values

// Note: re-define a macro is actually very bad programing practice, 
// so it's just a temp solution.
// Eventually we need to modify f-stack to resolve the incompatibility
// (by define LINUX_xxx, and have f-stack convert the values when appropriate).

#undef  O_NONBLOCK
#define	O_NONBLOCK		0x0004		

#undef SOCK_CLOEXEC
#define	SOCK_CLOEXEC	0x10000000

#undef SOCK_NONBLOCK
#define	SOCK_NONBLOCK	0x20000000

#undef SO_TYPE
#define	SO_TYPE		0x1008

#undef MSG_DONTWAIT
#define	MSG_DONTWAIT	0x00000080

#undef EAGAIN
#define EAGAIN 			35

#undef EINPROGRESS
#define EINPROGRESS		36

// below is defined in Linux but not in f-stack (no need to deal with them?)
/*
#define SO_BINDTODEVICE	 25
#define TCP_SYNCNT		 7
*/

// no need to do below because f-stack defines LINUX_xxx to address the incompatibility
/*
#define AF_INET 	2
#define AF_INET6    28

#define	SOL_SOCKET	0xffff
#define	SO_LINGER	0x00000080
#define	SO_BROADCAST	0x00000020
#define SO_ERROR	0x1007

#define POLLIN            0x0001
#define POLLPRI           0x0002
#define POLLOUT           0x0004
#define POLLERR           0x0008
#define POLLHUP           0x0010
#define POLLNVAL          0x0020
#define POLLRDNORM        0x0040
#define POLLRDBAND        0x0080
#define POLLWRNORM        0x0100
#define POLLWRBAND        0x0200
#define POLLMSG           0x0400
*/

#endif // #ifdef __linux__

#else

#define Ioctl ioctl
#define Fcntl fcntl
#define Socket socket
#define Setsockopt setsockopt
#define Getsockopt getsockopt
#define Send send
#define Recv recv
#define Recvfrom recvfrom
#define Bind bind
#define Connect connect
#define Poll poll

#define Sockaddr sockaddr

#endif // end of #else (#ifdef USE_FSTACK)


#endif // #ifndef _NETWORK_API_H_

