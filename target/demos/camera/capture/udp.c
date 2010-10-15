/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * Copyright (c) 2006, Chips & Media.  All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
 * port		server port#
 * addr		NULL -> this is the server, else server INADDR for client
 * taddr2	address returned for use in sendto by client
 */
int udp_open(int port, char *addr, struct sockaddr_in *taddr2)
{
	int rt;
	int fd;
	struct sockaddr_in taddr;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		return -1;
	}

	if (addr) {
		taddr2->sin_family = AF_INET;
		taddr2->sin_port = htons(port);
		taddr2->sin_addr.s_addr = inet_addr(addr);
		port = 0;
	}

	taddr.sin_family = AF_INET;
	taddr.sin_port = htons(port);
	taddr.sin_addr.s_addr = htonl(INADDR_ANY);
	rt = bind(fd, (struct sockaddr *)&taddr, sizeof(taddr));
	if (rt) {
		close(fd);
		return -2;
	}

	return fd;
}

//#define TEST
#ifdef TEST
#include <stdio.h>

int main(void)
{
	struct sockaddr_in	taddr;
	char	buf[1024];
	int	h, h2;
	int	len;
	int	pid;

	h = udp_open(6666, 0, &taddr);
	if (h < 0)
	{
		printf("open failed\n");
		return 1;
	}
	pid = fork();
	if (!pid)
	{
		sleep(1);
		h2 = udp_open(6666, "127.0.0.1", &taddr);
		if (h2 < 0)
		{
			printf("open failed\n");
			return 1;
		}
		strcpy(buf, "hello\n");
		len = sendto(h2, buf, sizeof(buf), 0, (struct sockaddr*)&taddr, sizeof(taddr));
		printf("sent %d bytes\n", len);
		close(h2);
	}
	else
	{
		len = sizeof(taddr);
		len = recvfrom(h, buf, sizeof(buf), 0, 
				(struct sockaddr *)&taddr, &len);
		printf("rx-ed %d bytes\n", len);
		printf(buf);
	}
	close(h);
	return 0;
}
#endif
