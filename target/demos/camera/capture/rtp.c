/*
 * Implements H.264 streaming over RTP (RFC 3984)
 *
 * Copyright (c) 2009-2010, Armadeus Systems.
 *
 * Based on source code which is:
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <sys/time.h>
#include <errno.h>

#include "udp.h"
#include "rtp.h"

/* RTP Flags MSB: */
#define RTP_VER		(2 << 6)
#define RTP_PAD		(1 << 5)
#define RTP_EXT		(1 << 4)
#define RTP_CC(count)	((count) << 0)
/* RTP Flags LSB: */
#define RTP_MRK		(1 << 7)
#define RTP_TYP(typ)	((typ) << 0)


typedef struct {
	unsigned char flag_msb;
	unsigned char flag_lsb;
	unsigned short seq;
	unsigned int time;
	unsigned int ssrc;
} __attribute__((packed)) rtph_t;


int rtp_init(rtp_t * t, int bufsize)
{
	rtph_t *h;
	int rt;

	if (t->size < 4 || t->clock < 1000) {
		return -2;
	}

	rt = t->size + sizeof(rtph_t);
	t->rxbuf = calloc(2, rt);
	if (!t->rxbuf) {
		return -1;
	}
	t->txbuf = t->rxbuf + rt;
	h = (rtph_t *) t->txbuf;
	h->flag_msb = RTP_VER;
	h->flag_lsb = RTP_TYP(t->type);

	t->sock = udp_open(t->port, t->ip, &t->to);
	if (t->sock < 0) {
		free(t->rxbuf);
		return -3;
	}

	if (bufsize > 0) {
		bufsize *= rt;
		setsockopt(t->sock, SOL_SOCKET, SO_RCVBUF, &bufsize, 4);
	}

	return 0;
}

int rtp_exit(rtp_t * t)
{
	free(t->rxbuf);
	close(t->sock);
	return 0;
}

#define chk_gen(buf, len)	0

extern int g_state;

static inline int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

#define DEBUG(fmt, args...) do { ; } while (0)

#define IS_ACCESS_UNIT(x) (((x) > 0x00) && ((x) < 0x06))

int rtp_tx(rtp_t * t, char *buf, int len)
{
	rtph_t *h;
	int tlen, hlen;
	int rt;
	struct timeval tv;
	static unsigned short seq_num = 0;
	int nal_type, mtu;
	unsigned char nal_header;
	unsigned char* payload;
	int start, end;

	h = (rtph_t *) t->txbuf;
	h->flag_lsb &= ~RTP_MRK;
	gettimeofday(&tv, 0);
	h->time = htonl(((tv.tv_sec * 1000000) + tv.tv_usec)/10); /* important ? */
	hlen = t->size;

	/* Here we receive NALU one by one from codec,
	   with 0x00000001 header, so skip it */
	buf += 4;
	len -= 4;

	nal_type = buf[0] & 0x1f;
	DEBUG("--- Processing packet with NAL type=%d\n", nal_type);
	mtu = t->size;
	if (len < mtu) {
		DEBUG("NAL Unit fit in one packet datasize=%d mtu=%d\n", len, mtu);
		memcpy(h + 1, buf, hlen);
		rt = len + sizeof(rtph_t);
		rt = sendto(t->sock, h, rt, 0,
			    (struct sockaddr *)&t->to, sizeof(t->to));
		if (rt < 0) {
			printf("%s: rtp_tx err %d %d\n", t->name, rt, errno);
			return rt;
		}

		seq_num++;
		h->seq = htons(seq_num);
		return len;
	} else {
		/* Divide NALU according to Fragmentation Unit A spec */
		DEBUG("NAL Unit DOES NOT fit in one packet datasize=%d mtu=%d\n", len, mtu);
		/* We keep 2 bytes for FU indicator and FU Header */
		hlen -= 2;
		start = 1;
		end = 0;
		/* skip NAL header in first frame as it will be reconstructed */
		nal_header = *buf;
		buf++;
		len--;
		for (tlen = 0; len > 0; tlen += hlen) {
			if (len <= t->size) {
				hlen = len;
				h->flag_lsb |= RTP_MRK;
				end = 1;
				DEBUG("Last ");
			}
			payload = (unsigned char*)(h+1);
			/* FU indicator (FU-A) */
			payload[0] = (nal_header & 0x60) | 28;
			/* FU Header */
			payload[1] = (start << 7) | (end << 6) | (nal_header & 0x1f);
			memcpy(&payload[2], buf, hlen);

			rt = min((len+2), t->size) + sizeof(rtph_t);
			rt = sendto(t->sock, h, rt, 0,
				(struct sockaddr *)&t->to, sizeof(t->to));
			if (rt < 0) {
				printf("%s: rtp_tx err %d %d\n", t->name, rt, errno);
				return rt;
			}
			DEBUG("Packet 0x%04x of %d bytes sent (%02x %02x %02x)\n", seq_num, rt,
				buf[0], buf[1], buf[2]);
	
			seq_num++;
			h->seq = htons(seq_num);
			buf += hlen;
			len -= hlen;
			start = 0;
		}
		return tlen;
	}
}

int rtp_rx(rtp_t * t, char *buf, int len)
{
	rtph_t *h;
	int tlen;
	int rt;
	unsigned short seq;

	tlen = 0;
	h = (rtph_t *) t->rxbuf;
	seq = h->seq;
	do {
		int slen;

		slen = sizeof(t->to);
		rt = recvfrom(t->sock, h, t->size + sizeof(rtph_t),MSG_WAITALL, (struct sockaddr *)&t->to, (unsigned int *)&slen);
		if (rt < 0) {
			printf("%s: rtp_rx err %d\n", t->name, rt);
			return rt;
		}
		if (!rt) {
			printf("%s: rtp_rx warn 0-size-pkt\n", t->name);
			t->err++;
			continue;
		}

		rt = chk_gen((char *)h, t->size + sizeof(rtph_t));
		if (rt) {
			t->err++;
//		      lp_skip:
			h->flag_lsb &= ~RTP_MRK;
			continue;
		}

		if (seq != h->seq) {
			t->skip++;
			/* skip remaining frags of pkt */
			while (!(h->flag_lsb & RTP_MRK)) {
				slen = sizeof(t->to);
				rt = recvfrom(t->sock, h,t->size + sizeof(rtph_t),MSG_WAITALL,(struct sockaddr *)&t->to,(unsigned int *)&slen);
				if (rt < 0) 
				{
					return rt;
				}
				rt = chk_gen((char *)h,
					     t->size + sizeof(rtph_t));
				if (rt) {
					printf("%s: rtp_rx(skip) warn pkt "
					       "corrupt chk=%02X\n",
					       t->name, rt);
					h->flag_lsb &= ~RTP_MRK;
					t->err++;
				} else {
					t->skip++;
				}
			}
			tlen = 0;
			h->flag_lsb &= ~RTP_MRK;
			h->seq++;
			tlen = 0;	/* discard previous frags of pkt */
			seq = h->seq;
			continue;
		}

		h->seq++;
		seq = h->seq;
		if (tlen > len) {
			/* more data to read */
			printf("%s: rtp_rx warn size %d < %d\n",
			       t->name, len, tlen);
			return tlen;
		}
	}
	while (!(h->flag_lsb & RTP_MRK));
	return tlen;
}

rtp_t* rtp_session_open()
{
	rtp_t *rtp = (rtp_t *) malloc(sizeof(rtp_t));
	memset(rtp, 0, sizeof(rtp_t));
	rtp->name = "rec";
	rtp->ip = RTP_ADDRESS;
	rtp->port = RTP_PORT;
	rtp->type = RTP_TYPE_DYNAMIC;
	rtp->size = 512;
	rtp->clock = 1000000;
	rtp_init(rtp, 0);
	return rtp;
}

void rtp_send(char *packet, int size)
{
	static rtp_t* rtp_session = NULL;
	if (rtp_session==NULL) 
		rtp_session = rtp_session_open();
	rtp_tx(rtp_session, packet, size);
}

