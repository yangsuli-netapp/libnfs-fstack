#ifndef _NETWORK_API_H_
#define _NETWORK_API_H_

#ifdef USE_FSTACK

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

#endif 


#endif