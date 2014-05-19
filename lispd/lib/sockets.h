/*
 * sockets.h
 *
 * This file is part of LISP Mobile Node Implementation.
 *
 * Copyright (C) 2012 Cisco Systems, Inc, 2012. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Please send any bug reports or fixes you make to the email address(es):
 *    LISP-MN developers <devel@lispmob.org>
 *
 * Written or modified by:
 *    Alberto Rodriguez Natal   <arnatal@ac.upc.edu>
 *    Albert López              <alopez@ac.upc.edu>
 *    Florin Coras              <fcoras@ac.upc.edu>
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include "defs.h"
#include "sockets-util.h"
#include "packets.h"
#include "lisp_address.h"
#include "lbuf.h"




typedef enum {
    SOCK_READ,
    SOCK_WRITE,
} sock_type_e;

/*
 * inspired by quagga thread.c
 * It might be a little bit of an overkill for now
 * but it could prove useful in the future
 */

typedef struct sock sock_t;
typedef struct sock_list sock_list_t;

struct sock_list {
    struct sock *head;
    struct sock *tail;
    int count;
    int maxfd;
};

struct sock {
    sock_type_e type;
    int (*recv_cb)(struct sock *);
    void *arg;
    int fd;
    struct sock *next;
    struct sock *prev;
};

typedef struct uconn {
    /* TODO: decide if la, ra should be IP */
    lisp_addr_t la;     /* local address */
    lisp_addr_t ra;     /* remote address */
    uint16_t lp;        /* local port */
    uint16_t rp;        /* remote port */
} uconn_t;

typedef struct sock_master {
    sock_list_t read;
//    struct sock_list *write;
//    struct sock_list *netlink;
    fd_set readfds;
//    fd_set *writefds;
//    fd_set *netlinkfds;
} sock_master_t;

union sockunion {
    struct sockaddr_in s4;
    struct sockaddr_in6 s6;
};



extern struct sock_master *sock_master_new();
extern struct sock *sock_register_read_listener(struct sock_master *m,
        int (*)(struct sock *), void *arg, int fd);
extern void sock_process_all(struct sock_master *m);
extern void sock_fdset_all_read(struct sock_master *m);

int open_data_input_socket(int afi);
int open_control_input_socket(int afi);



int sock_send(uconn_t *uc, struct lbuf *b);
int sock_recv(int, struct lbuf *, uconn_t *);

int sock_recv_data_packet(int sock, lbuf_t *b, uint8_t *ttl, uint8_t *tos);

static inline int uconn_init(uconn_t *uc, int lp, int rp, lisp_addr_t *la,
        lisp_addr_t *ra)
{
    uc->lp = lp;
    uc->rp = rp;
    la ? lisp_addr_copy(&uc->la, la) :
            lisp_addr_set_afi(&uc->la, LM_AFI_NO_ADDR);
    ra ? lisp_addr_copy(&uc->ra, ra) :
            lisp_addr_set_afi(&uc->ra, LM_AFI_NO_ADDR);
    return(GOOD);
}
#endif /*SOCKETS_H_*/
