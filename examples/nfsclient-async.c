/* 
   Copyright (C) by Ronnie Sahlberg <ronniesahlberg@gmail.com> 2010
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

/* Example program using the highlevel async interface.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include <win32/win32_compat.h>
#pragma comment(lib, "ws2_32.lib")
WSADATA wsaData;
#else
#include <sys/stat.h>
#endif
 
#ifdef HAVE_POLL_H
#include <poll.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define SERVER "172.31.46.194"
#define EXPORT "/home/ubuntu/nfs_export"
#define NFSFILE "/BOOKS/Classics/Dracula.djvu"
#define NFSDIR "/BOOKS/Classics/"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <assert.h>

#include <fcntl.h>
#include "libnfs.h"
#include "libnfs-raw.h"
#include "libnfs-raw-mount.h"
#include "nfs4/libnfs-raw-nfs4.h"

#include "network-api.h"

struct rpc_context *mount_context;

struct client {
       char *server;
       char *export;
       uint32_t mount_port;
       struct nfsfh *nfsfh;
       int is_finished;
};

void mount_export_cb(struct rpc_context *mount_context, int status, void *data, void *private_data)
{
	struct client *client = private_data;
	exports export = *(exports *)data;

	if (status < 0) {
		printf("MOUNT/EXPORT failed with \"%s\"\n", rpc_get_error(mount_context));
		exit(10);
	}

	printf("Got exports list from server %s\n", client->server);
	while (export != NULL) {
	      printf("Export: %s\n", export->ex_dir);
	      export = export->ex_next;
	}

	mount_context = NULL;

	client->is_finished = 1;
}

void nfs_opendir_cb(int status, struct nfs_context *nfs, void *data, void *private_data)
{
	struct client *client = private_data;
	struct nfsdir *nfsdir = data;
	struct nfsdirent *nfsdirent;

	if (status < 0) {
		printf("opendir failed with \"%s\"\n", (char *)data);
		exit(10);
	}

	printf("opendir successful\n");
	while((nfsdirent = nfs_readdir(nfs, nfsdir)) != NULL) {
		printf("Inode:%d Name:%s\n", (int)nfsdirent->inode, nfsdirent->name);
	}
	nfs_closedir(nfs, nfsdir);

	mount_context = rpc_init_context();
	if (mount_getexports_async(mount_context, client->server, mount_export_cb, client) != 0) {
		printf("Failed to start MOUNT/EXPORT\n");
		exit(10);
	}
}

void nfs_close_cb(int status, struct nfs_context *nfs, void *data, void *private_data)
{
	struct client *client = private_data;

	if (status < 0) {
		printf("close failed with \"%s\"\n", (char *)data);
		exit(10);
	}

	printf("close successful\n");
	printf("call opendir(%s)\n", NFSDIR);
	if (nfs_opendir_async(nfs, NFSDIR, nfs_opendir_cb, client) != 0) {
		printf("Failed to start async nfs close\n");
		exit(10);
	}
}

void nfs_fstat64_cb(int status, struct nfs_context *nfs, void *data, void *private_data)
{
	struct client *client = private_data;
	struct nfs_stat_64 *st;
 
	if (status < 0) {
		printf("fstat call failed with \"%s\"\n", (char *)data);
		exit(10);
	}

	printf("Got reply from server for fstat(%s).\n", NFSFILE);
	st = (struct nfs_stat_64 *)data;
	printf("Mode %04o\n", (int)st->nfs_mode);
	printf("Size %d\n", (int)st->nfs_size);
	printf("Inode %04o\n", (int)st->nfs_ino);

	printf("Close file\n");
	if (nfs_close_async(nfs, client->nfsfh, nfs_close_cb, client) != 0) {
		printf("Failed to start async nfs close\n");
		exit(10);
	}
}

void nfs_read_cb(int status, struct nfs_context *nfs, void *data, void *private_data)
{
	struct client *client = private_data;
	char *read_data;
	int i;

	if (status < 0) {
		printf("read failed with \"%s\"\n", (char *)data);
		exit(10);
	}

	printf("read successful with %d bytes of data\n", status);
	read_data = data;
	for (i=0;i<16;i++) {
		printf("%02x ", read_data[i]&0xff);
	}
	printf("\n");
	printf("Fstat file :%s\n", NFSFILE);
	if (nfs_fstat64_async(nfs, client->nfsfh, nfs_fstat64_cb, client) != 0) {
		printf("Failed to start async nfs fstat\n");
		exit(10);
	}
}

void nfs_open_cb(int status, struct nfs_context *nfs, void *data, void *private_data)
{
	struct client *client = private_data;
	struct nfsfh *nfsfh;

	if (status < 0) {
		printf("open call failed with \"%s\"\n", (char *)data);
		exit(10);
	}

	nfsfh         = data;
	client->nfsfh = nfsfh;
	printf("Got reply from server for open(%s). Handle:%p\n", NFSFILE, data);
	printf("Read first 64 bytes\n");
	if (nfs_pread_async(nfs, nfsfh, 0, 64, nfs_read_cb, client) != 0) {
		printf("Failed to start async nfs open\n");
		exit(10);
	}
}

void nfs_stat64_cb(int status, struct nfs_context *nfs, void *data, void *private_data)
{
	struct client *client = private_data;
	struct nfs_stat_64 *st;
 
	if (status < 0) {
		printf("stat call failed with \"%s\"\n", (char *)data);
		exit(10);
	}

	printf("Got reply from server for stat(%s).\n", NFSFILE);
	st = (struct nfs_stat_64 *)data;
	printf("Mode %04o\n", (unsigned int) st->nfs_mode);
	printf("Size %d\n", (int)st->nfs_size);
	printf("Inode %04o\n", (int)st->nfs_ino);

	printf("Open file for reading :%s\n", NFSFILE);
	if (nfs_open_async(nfs, NFSFILE, O_RDONLY, nfs_open_cb, client) != 0) {
		printf("Failed to start async nfs open\n");
		exit(10);
	}
}

void nfs_mount_cb(int status, struct nfs_context *nfs, void *data, void *private_data)
{
	struct client *client = private_data;

	if (status < 0) {
		printf("mount/mnt call failed with \"%s\"\n", (char *)data);
		exit(10);
	}

	printf("Got reply from server for MOUNT/MNT procedure.\n");
	printf("Stat file :%s\n", NFSFILE);
	if (nfs_stat64_async(nfs, NFSFILE, nfs_stat64_cb, client) != 0) {
		printf("Failed to start async nfs stat\n");
		exit(10);
	}
}

struct nfs_context *nfs;

int loop(void *arg) {
	struct client* client = (struct client*)arg;
	struct pollfd pfds[2]; /* nfs:0 mount:1 */
	int num_fds;

	pfds[0].fd = nfs_get_fd(nfs);
	pfds[0].events = nfs_which_events(nfs);
	num_fds = 1;

	if(mount_context != 0 && rpc_get_fd(mount_context) != -1) {
		pfds[1].fd = rpc_get_fd(mount_context);
		pfds[1].events = rpc_which_events(mount_context);
		num_fds = 2;
	}

	if (Poll(&pfds[0], 2, -1) < 0) {
		printf("Poll failed\n");
		exit(10);
		return -1;
	}

	if (mount_context != NULL) {
		if (rpc_service(mount_context, pfds[1].revents) < 0) {
			printf("rpc_service failed\n");
                	exit(9);
			return -1;
		}
	}

	if (nfs_service(nfs, pfds[0].revents) < 0) {
		printf("nfs_service failed\n");
                exit(9);
		return -1;
	}

	if (client->is_finished) {
		printf("client is_finished. Should terminate now\n");
		exit(0);
		return 0;
	}

	return 0;
}


int main(int argc, char *argv[])
{
	int ret;
	struct client client;
	struct pollfd pfds[2]; /* nfs:0  mount:1 */

	//assert(ff_init(argc, argv)==0);
	ret = ff_init(argc, argv);
	printf("yangsuli ff_init returned %d\n", ret);

#ifdef WIN32
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		printf("Failed to start Winsock2\n");
		exit(10);
	}
#endif
	
	client.server = SERVER;
	client.export = EXPORT;
	client.is_finished = 0;

	nfs = nfs_init_context();
	if (nfs == NULL) {
		printf("failed to init context\n");
		exit(10);
	}

	assert(nfs_set_version(nfs, NFS_V4) == 0);

	ret = nfs_mount_async(nfs, client.server, client.export, nfs_mount_cb, &client);
	if (ret != 0) {
		printf("Failed to start async nfs mount\n");
		exit(10);
	}

	ff_run(loop, &client);

	/*
	for (;;) {
		int num_fds;

		pfds[0].fd = nfs_get_fd(nfs);
		pfds[0].events = nfs_which_events(nfs);
		num_fds = 1;

		if (mount_context != 0 && rpc_get_fd(mount_context) != -1) {
			pfds[1].fd = rpc_get_fd(mount_context);
			pfds[1].events = rpc_which_events(mount_context);
			num_fds = 2;
		}
		if (Poll(&pfds[0], 2, -1) < 0) {
			printf("Poll failed");
			exit(10);
		}
		if (mount_context != NULL) {
			if (rpc_service(mount_context, pfds[1].revents) < 0) {
				printf("rpc_service failed\n");
				break;
			}
		}
		if (nfs_service(nfs, pfds[0].revents) < 0) {
			printf("nfs_service failed\n");
			break;
		}
		if (client.is_finished) {
			break;
		}
	}
	*/
	
	nfs_destroy_context(nfs);
	if (mount_context != NULL) {
		rpc_destroy_context(mount_context);
		mount_context = NULL;
	}
	printf("nfsclient finished\n");

	

	return 0;
}

